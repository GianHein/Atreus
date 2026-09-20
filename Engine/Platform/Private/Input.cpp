#include "Atreus/Platform/Input.h"

#include "Atreus/Platform/PlatformEvent.h"

#include <cstring>

namespace Atreus
{
  static InputState input[MAX_WINDOWS];

  static InputState *ResolveInput(WindowHandle window)
  {
    if (!IsValidWindow(window)) return nullptr;
    return &input[window.index];
  }

  void UpdateInput(WindowHandle window, const PlatformEvent *events, u32 event_count)
  {
    InputState *window_input = ResolveInput(window);

    if (window_input == nullptr) return;

    memcpy(
        window_input->keys_down_previous, window_input->keys_down, sizeof(window_input->keys_down)
    );

    memcpy(
        window_input->mouse_down_previous,
        window_input->mouse_down,
        sizeof(window_input->mouse_down)
    );

    window_input->mouse_delta_x = 0;
    window_input->mouse_delta_y = 0;
    window_input->scroll_delta  = 0;

    for (u32 i = 0; i < event_count; ++i)
    {
      PlatformEvent event = events[i];
      if (!IsSameWindow(event.window, window)) continue;

      switch (event.type)
      {
      case PlatformEventType::KeyPress:
      {
        window_input->keys_down[(u8)event.key_press.key] = true;
        break;
      }

      case PlatformEventType::KeyRelease:
      {
        window_input->keys_down[(u8)event.key_release.key] = false;
        break;
      }

      case PlatformEventType::MouseButtonPress:
      {
        window_input->mouse_down[(u8)event.mouse_button_press.button] = true;
        break;
      }

      case PlatformEventType::MouseButtonRelease:
      {
        window_input->mouse_down[(u8)event.mouse_button_release.button] = false;
        break;
      }

      case PlatformEventType::MouseMove:
      {
        window_input->mouse_x = event.mouse_move.x;
        window_input->mouse_y = event.mouse_move.y;
        window_input->mouse_delta_x += event.mouse_move.delta_x;
        window_input->mouse_delta_y += event.mouse_move.delta_y;
        break;
      }

      case PlatformEventType::MouseWheel:
      {
        window_input->scroll_delta += event.mouse_wheel.delta;
        break;
      }

      case PlatformEventType::FocusChange:
      {
        if (event.focus_change.focused == false)
        {
          memset(window_input->keys_down, 0, sizeof(window_input->keys_down));
          memset(window_input->mouse_down, 0, sizeof(window_input->mouse_down));
        }
        break;
      }
      }
    }
  }

  b8 IsKeyDown(WindowHandle window, Key key)
  {
    InputState *window_input = ResolveInput(window);
    if (window_input == nullptr) return false;

    return window_input->keys_down[(u8)key];
  }

  b8 WasKeyPressed(WindowHandle window, Key key)
  {
    InputState *window_input = ResolveInput(window);
    if (window_input == nullptr) return false;

    return !window_input->keys_down_previous[(u8)key] && window_input->keys_down[(u8)key];
  }

  b8 WasKeyReleased(WindowHandle window, Key key)
  {
    InputState *window_input = ResolveInput(window);
    if (window_input == nullptr) return false;

    return window_input->keys_down_previous[(u8)key] && !window_input->keys_down[(u8)key];
  }

  b8 IsMouseButtonDown(WindowHandle window, MouseButton button)
  {
    InputState *window_input = ResolveInput(window);
    if (window_input == nullptr) return false;

    return window_input->mouse_down[(u8)button];
  }

  b8 WasMouseButtonPressed(WindowHandle window, MouseButton button)
  {
    InputState *window_input = ResolveInput(window);
    if (window_input == nullptr) return false;

    return !window_input->mouse_down_previous[(u8)button] && window_input->mouse_down[(u8)button];
  }

  b8 WasMouseButtonReleased(WindowHandle window, MouseButton button)
  {
    InputState *window_input = ResolveInput(window);
    if (window_input == nullptr) return false;

    return window_input->mouse_down_previous[(u8)button] && !window_input->mouse_down[(u8)button];
  }

  void GetMousePosition(WindowHandle window, i32 *out_x, i32 *out_y)
  {
    InputState *window_input = ResolveInput(window);
    if (window_input == nullptr) return;

    if (out_x) *out_x = window_input->mouse_x;
    if (out_y) *out_y = window_input->mouse_y;
  }

  void GetMouseDelta(WindowHandle window, i32 *out_delta_x, i32 *out_delta_y)
  {
    InputState *window_input = ResolveInput(window);
    if (window_input == nullptr) return;

    if (out_delta_x) *out_delta_x = window_input->mouse_delta_x;
    if (out_delta_y) *out_delta_y = window_input->mouse_delta_y;
  }

  i32 GetScrollDelta(WindowHandle window)
  {
    InputState *window_input = ResolveInput(window);
    if (window_input == nullptr) return 0;

    return window_input->scroll_delta;
  }
}