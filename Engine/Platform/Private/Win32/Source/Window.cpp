#include "Atreus/Platform/Window.h"

#include "Atreus/Log.h"

#if !defined(WIN32_LEAN_AND_MEAN)
#  define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>

namespace
{
  constexpr usize EVENT_CAPACITY = 2048;

  const char *const window_class_name = "AtreusWindow";
}

struct Window
{
  HWND            hwnd;
  bool            closing;
  bool            focused;
  bool            fullscreen;
  bool            borderless;
  bool            cursor_visible;
  u32             client_width;
  u32             client_height;
  WINDOWPLACEMENT window_placement;
  LONG_PTR        windowed_style;
  WindowEvent     events[EVENT_CAPACITY];
  u32             event_head;
  u32             event_count;
};