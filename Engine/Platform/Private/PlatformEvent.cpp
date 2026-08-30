#include "Atreus/Platform/PlatformEvent.h"

#include "Atreus/Platform/Window.h"

#include <cstdio>

namespace Atreus
{
  constexpr u32 MAX_PLATFORM_EVENTS = 512;

  static PlatformEvent platform_event_queue[MAX_PLATFORM_EVENTS];
  static u32           platform_event_count = 0;

  void PushPlatformEvent(PlatformEvent event)
  {
    if (event.type == PlatformEventType::MouseMove && platform_event_count > 0)
    {
      PlatformEvent &last = platform_event_queue[platform_event_count - 1];
      if (last.type == PlatformEventType::MouseMove && SameWindow(last.window, event.window))
      {
        last.mouse_move.x = event.mouse_move.x;
        last.mouse_move.y = event.mouse_move.y;
        last.mouse_move.delta_x += event.mouse_move.delta_x;
        last.mouse_move.delta_y += event.mouse_move.delta_y;
        return;
      }
    }

    if (platform_event_count >= MAX_PLATFORM_EVENTS)
    {
      fprintf(stderr, "[Atr::Platform] PlatformEvent queue full, dropping event\n");
      return;
    }

    platform_event_queue[platform_event_count++] = event;
  }

  PlatformEventBuffer DrainPlatformEvents()
  {
    PlatformEventBuffer result = {platform_event_queue, platform_event_count};
    platform_event_count       = 0;
    return result;
  }
}
