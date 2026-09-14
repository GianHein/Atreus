#pragma once

#include "Atreus/Definitions.h"
#include "Atreus/Platform/Input.h"
#include "Atreus/Platform/Window.h"

namespace Atreus
{
  enum class PlatformEventType : u8
  {
    None = 0,
    Quit,
    KeyPressed,
    KeyReleased,
    MouseMove,
    MouseButtonPressed,
    MouseButtonReleased,
    MouseWheel,
    WindowResize,
    WindowClose,
    FocusChanged
  };

  struct KeyPressedData
  {
    Key key = Key::Unknown;
  };

  struct KeyReleasedData
  {
    Key key = Key::Unknown;
  };

  struct MouseMoveData
  {
    i32 x       = 0;
    i32 y       = 0;
    i32 delta_x = 0;
    i32 delta_y = 0;
  };

  struct MouseButtonPressedData
  {
    MouseButton button = MouseButton::Unknown;
  };

  struct MouseButtonReleasedData
  {
    MouseButton button = MouseButton::Unknown;
  };

  struct MouseWheelData
  {
    i32 delta = 0;
  };

  struct WindowResizeData
  {
    u32 width  = 0;
    u32 height = 0;
  };

  struct FocusChangedData
  {
    b8 focused = false;
  };

  struct PlatformEvent
  {
    PlatformEventType type   = PlatformEventType::None;
    WindowHandle      window = {UINT32_MAX, UINT32_MAX};
    union
    {
      KeyPressedData          key_pressed;
      KeyReleasedData         key_released;
      MouseMoveData           mouse_move;
      MouseButtonPressedData  mouse_button_pressed;
      MouseButtonReleasedData mouse_button_released;
      MouseWheelData          mouse_wheel;
      WindowResizeData        window_resize;
      FocusChangedData        focus_changed;
    };
  };

  struct PlatformEventBuffer
  {
    PlatformEvent *buffer      = nullptr;
    u32            buffer_size = 0;
  };

  PlatformEventBuffer PollEvents();
  void                PushPlatformEvent(PlatformEvent event);
  PlatformEventBuffer DrainPlatformEvents();
}