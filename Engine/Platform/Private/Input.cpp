#include "Atreus/Platform/Input.h"

#include "Atreus/Platform/PlatformEvent.h"
#include "Atreus/Platform/Window.h"

#include <cstring>

namespace Atreus
{
  void
  UpdateInputState(InputState *state, const PlatformEventBuffer &events, WindowHandle target_window)
  {
    memset(state->keys_pressed, 0, sizeof(state->keys_pressed));
    memset(state->keys_released, 0, sizeof(state->keys_released));
    memset(state->mouse_pressed, 0, sizeof(state->mouse_pressed));
    memset(state->mouse_released, 0, sizeof(state->mouse_released));
    state->mouse_delta_x     = 0;
    state->mouse_delta_y     = 0;
    state->mouse_wheel_delta = 0;

    for (u32 i = 0; i < events.buffer_size; ++i)
    {
      const PlatformEvent &event = events.buffer[i];
      if (!SameWindow(event.window, target_window)) continue;

      switch (event.type)
      {
      case PlatformEventType::KeyPressed:
      {
        u32 key = (u32)event.key_pressed.key;
        // Windows re-sends WM_KEYDOWN while a key is held (auto-repeat); only
        // count it as a fresh "pressed" edge if it wasn't already down.
        if (!state->keys_held[key]) state->keys_pressed[key] = true;
        state->keys_held[key] = true;
        break;
      }

      case PlatformEventType::KeyReleased:
      {
        u32 key                   = (u32)event.key_released.key;
        state->keys_released[key] = true;
        state->keys_held[key]     = false;
        break;
      }

      case PlatformEventType::MouseMove:
      {
        state->mouse_x = event.mouse_move.x;
        state->mouse_y = event.mouse_move.y;
        state->mouse_delta_x += event.mouse_move.delta_x;
        state->mouse_delta_y += event.mouse_move.delta_y;
        break;
      }

      case PlatformEventType::MouseButtonPressed:
      {
        u32 button = (u32)event.mouse_button_pressed.button;
        if (!state->mouse_held[button]) state->mouse_pressed[button] = true;
        state->mouse_held[button] = true;
        break;
      }

      case PlatformEventType::MouseButtonReleased:
      {
        u32 button                    = (u32)event.mouse_button_released.button;
        state->mouse_released[button] = true;
        state->mouse_held[button]     = false;
        break;
      }

      case PlatformEventType::MouseWheel:
      {
        state->mouse_wheel_delta += event.mouse_wheel.delta;
        break;
      }

      case PlatformEventType::FocusChanged:
      {
        if (!event.focus_changed.focused)
        {
          for (u32 i = 0; i < (u32)Key::COUNT; ++i)
          {
            if (state->keys_held[i] || state->keys_pressed[i])
            {
              state->keys_released[i] = true;
              state->keys_held[i]     = false;
              state->keys_pressed[i]  = false;
            }
          }

          for (u32 i = 0; i < (u32)MouseButton::COUNT; ++i)
          {
            if (state->mouse_held[i] || state->mouse_pressed[i])
            {
              state->mouse_released[i] = true;
              state->mouse_held[i]     = false;
              state->mouse_pressed[i]  = false;
            }
          }
        }
        break;
      }

      default:
        break;
      }
    }
  }
}
