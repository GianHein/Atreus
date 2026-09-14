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
    Key key;
  };

  struct KeyReleasedData
  {
    Key key;
  };

  struct MouseMoveData
  {
    i32 x;
    i32 y;
    i32 delta_x;
    i32 delta_y;
  };

  struct MouseButtonPressedData
  {
    MouseButton button;
  };

  struct MouseButtonReleasedData
  {
    MouseButton button;
  };

  struct MouseWheelData
  {
    i32 delta;
  };

  struct WindowResizeData
  {
    u32 width;
    u32 height;
  };

  struct FocusChangedData
  {
    b8 focused;
  };

  struct PlatformEvent
  {
    PlatformEventType type;
    WindowHandle      window;
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