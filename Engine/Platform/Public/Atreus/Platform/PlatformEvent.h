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
    KeyPress,
    KeyRelease,
    MouseMove,
    MouseButtonPress,
    MouseButtonRelease,
    MouseWheel,
    WindowResize,
    WindowClose,
    FocusChange
  };

  struct QuitData
  {
  };

  struct KeyPressData
  {
    Key key;
  };

  struct KeyReleaseData
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

  struct MouseButtonPressData
  {
    MouseButton button;
  };

  struct MouseButtonReleaseData
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

  struct WindowCloseData
  {
  };

  struct FocusChangeData
  {
    b8 focused;
  };

  struct PlatformEvent
  {
    PlatformEventType type;
    WindowHandle      window;

    union
    {
      QuitData               quit;
      KeyPressData           key_press;
      KeyReleaseData         key_release;
      MouseMoveData          mouse_move;
      MouseButtonPressData   mouse_button_press;
      MouseButtonReleaseData mouse_button_release;
      MouseWheelData         mouse_wheel;
      WindowResizeData       window_resize;
      WindowCloseData        window_close;
      FocusChangeData        focus_change;
    };
  };

  u32 PollEvents(PlatformEvent *out_events, u32 max_events);
}