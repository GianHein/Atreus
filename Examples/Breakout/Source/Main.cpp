#include <Atreus/Definitions.h>
#include <Atreus/Platform/Input.h>
#include <Atreus/Platform/PlatformEvent.h>
#include <Atreus/Platform/Time.h>
#include <Atreus/Platform/Window.h>

using namespace Atreus;

#include <cstdio>

void ProcessInput(WindowHandle window, b8 &is_running, InputState &input)
{
  PlatformEventBuffer frame_events = PollEvents();

  for (u32 i = 0; i < frame_events.buffer_size; ++i)
  {
    const PlatformEvent &event = frame_events.buffer[i];
    if (event.type == PlatformEventType::WindowClose || event.type == PlatformEventType::Quit)
    {
      is_running = false;
    }
  }

  UpdateInputState(&input, frame_events, window);

  if (IsKeyPressed(input, Key::Escape))
  {
    is_running = false;
  }
}

void Update(u64 &accumulator, u64 &last_frame_time, const u64 fixed_delta_time)
{
  u64 current_time = GetTicks();
  accumulator += current_time - last_frame_time;
  last_frame_time = current_time;

  while (accumulator >= fixed_delta_time)
  {
    // Update(fixed_delta_time);
    accumulator -= fixed_delta_time;
  }
}

int main(int argc, char **argv)
{
  b8 is_running = true;

  WindowHandle window;

  if (!CreateWindow(
          {.title      = "Atreus",
           .width      = 1920,
           .height     = 1080,
           .fullscreen = false,
           .borderless = false,
           .resizable  = true},
          &window
      ))
    return 1;

  const u64 ticks_per_second = GetTicksPerSecond();

  u64       accumulator      = 0;
  const u64 fixed_delta_time = (ticks_per_second + 144 / 2) / 144;
  u64       last_frame_time  = GetTicks();

  InputState frame_input = {};

  // Diagnostic: how many outer-loop iterations we manage per second, i.e. the
  // uncapped frame rate the loop could sustain (no render, no vsync yet).
  u64 loop_iterations   = 0;
  u64 last_report_ticks = GetTicks();

  while (is_running)
  {
    ProcessInput(window, is_running, frame_input);
    Update(accumulator, last_frame_time, fixed_delta_time);

    ++loop_iterations;

    u64 now           = GetTicks();
    u64 elapsed_ticks = now - last_report_ticks;
    if (elapsed_ticks >= ticks_per_second)
    {
      f64 iterations_per_second = (f64)loop_iterations * (f64)ticks_per_second / (f64)elapsed_ticks;
      printf("loop: %.0f iterations/sec\n", iterations_per_second);

      loop_iterations   = 0;
      last_report_ticks = now;
    }
  }

  DestroyWindow(&window);
  return 0;
}