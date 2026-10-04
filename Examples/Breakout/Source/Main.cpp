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

    switch (event.type)
    {
    case PlatformEventType::Quit:
    case PlatformEventType::WindowClose:
    {
      is_running = false;
      break;
    }

    default:
      break;
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

void Render(RendererHandle renderer)
{
  Clear(renderer, Color{22, 22, 22, 255});
  // DrawRectangle(
  // renderer, Vector2D(390.0f, 320.0f), Vector2D(500.0f, 200.0f), Color(1.0f, 0.0f, 0.0f, 1.0f)
  // );
  // DrawCircle(renderer, Vector2D(640.0f, 360.0f), 300.0f, Color(0.0f, 0.0f, 1.0f, 1.0f));
  Present(renderer);
}

int main(int argc, char **argv)
{
  b8 is_running = true;

  InitializeLog();

  WindowHandle window;

  if (!CreateWindow(
          {.title      = "Breakout",
           .width      = 1920,
           .height     = 1080,
           .fullscreen = false,
           .borderless = false,
           .resizable  = true},
          &window
      ))
  {
    ATR_LOG_FATAL("Failed to create window");
    return 1;
  }

  RendererHandle renderer;

  if (!InitializeRenderer2D(window, {.logical_width = 1280, .logical_height = 720}, &renderer))
  {
    ATR_LOG_FATAL("Failed to initialize Renderer2D");
    DestroyWindow(&window);
    ShutdownLog();
    return 1;
  }

  const u64 ticks_per_second = GetTicksPerSecond();

  u64       accumulator      = 0;
  u64       last_frame_time  = GetTicks();
  const u64 fixed_delta_time = (ticks_per_second + 144 / 2) / 144;

  while (is_running)
  {
    ProcessInput(window, is_running);
    Update(accumulator, last_frame_time, fixed_delta_time);
    Render(renderer);
  }

  DestroyRenderer2D(&renderer);
  DestroyWindow(&window);
  ShutdownLog();
  return 0;
}