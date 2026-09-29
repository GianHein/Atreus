#include <Atreus/Definitions.h>
#include <Atreus/Graphics/Renderer2D.h>
#include <Atreus/Log.h>
#include <Atreus/Platform/Input.h>
#include <Atreus/Platform/PlatformEvent.h>
#include <Atreus/Platform/Time.h>
#include <Atreus/Platform/Window.h>

using namespace Atreus;

void ProcessInput(WindowHandle window, b8 &is_running)
{
  constexpr u32 max_frame_events = 512;
  PlatformEvent frame_events[max_frame_events];
  u32           event_count = PollEvents(frame_events, max_frame_events);

  for (u32 i = 0; i < event_count; ++i)
  {
    const PlatformEvent &event = frame_events[i];
    if (event.type == PlatformEventType::WindowClose || event.type == PlatformEventType::Quit)
    {
      is_running = false;
    }
  }

  UpdateInput(window, frame_events, event_count);

  if (WasKeyPressed(window, Key::Escape))
  {
    is_running = false;
  }

  if (WasKeyPressed(window, Key::F11))
  {
    SetWindowFullscreen(window, !IsWindowFullscreen(window));
  }

  if (WasKeyPressed(window, Key::F10))
  {
    SetWindowBorderless(window, !IsWindowBorderless(window));
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

void Render(WindowHandle window)
{
  ClearBackground(window, 1.0f, 0.0f, 1.0f, 1.0f);
  Present(window);
}

int main(int argc, char **argv)
{
  b8 is_running = true;

  InitializeLog();

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

  InitializeGraphics(window);

  const u64 ticks_per_second = GetTicksPerSecond();

  u64       accumulator      = 0;
  const u64 fixed_delta_time = (ticks_per_second + 144 / 2) / 144;
  u64       last_frame_time  = GetTicks();

  // Diagnostic: how many outer-loop iterations we manage per second, i.e. the
  // uncapped frame rate the loop could sustain (no render, no vsync yet).
  u64 loop_iterations   = 0;
  u64 last_report_ticks = GetTicks();

  while (is_running)
  {
    ProcessInput(window, is_running);
    Update(accumulator, last_frame_time, fixed_delta_time);
    Render(window);

    ++loop_iterations;

    u64 now           = GetTicks();
    u64 elapsed_ticks = now - last_report_ticks;
    if (elapsed_ticks >= ticks_per_second)
    {
      f64 iterations_per_second = (f64)loop_iterations * (f64)ticks_per_second / (f64)elapsed_ticks;

      loop_iterations   = 0;
      last_report_ticks = now;
    }
  }

  ShutdownGraphics(window);
  DestroyWindow(&window);
  ShutdownLog();
  return 0;
}