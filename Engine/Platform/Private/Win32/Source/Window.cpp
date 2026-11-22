#include "Atreus/Platform/Window.h"

#include "Atreus/Array.h"
#include "Atreus/Log.h"
#include "Atreus/Memory.h"

#if !defined(WIN32_LEAN_AND_MEAN)
#  define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>
#include <Xinput.h>

namespace
{
  constexpr usize EVENT_CAPACITY = 2048;

  const char *WINDOW_CLASS_NAME = "AtreusWindow";
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

namespace
{
  KeyCode translate_key(WPARAM virtual_key, LPARAM flags)
  {
    UINT scancode = (flags >> 16) & 0xFF;

    if (scancode == 0x29) return KeyCode::GraveAccent;

    if (virtual_key >= 'A' && virtual_key <= 'Z')
      return static_cast<KeyCode>(static_cast<u16>(KeyCode::A) + (virtual_key - 'A'));

    if (virtual_key >= '0' && virtual_key <= '9')
      return static_cast<KeyCode>(static_cast<u16>(KeyCode::Num0) + (virtual_key - '0'));

    if (virtual_key >= VK_F1 && virtual_key <= VK_F12)
      return static_cast<KeyCode>(static_cast<u16>(KeyCode::F1) + (virtual_key - VK_F1));

    bool extended = (flags & (1 << 24)) != 0;
    switch (virtual_key)
    {
    case VK_LEFT:
      return KeyCode::Left;
    case VK_RIGHT:
      return KeyCode::Right;
    case VK_UP:
      return KeyCode::Up;
    case VK_DOWN:
      return KeyCode::Down;
    case VK_SPACE:
      return KeyCode::Space;
    case VK_RETURN:
      return KeyCode::Enter;
    case VK_ESCAPE:
      return KeyCode::Escape;
    case VK_TAB:
      return KeyCode::Tab;
    case VK_BACK:
      return KeyCode::Backspace;
    case VK_DELETE:
      return KeyCode::Delete;
    case VK_INSERT:
      return KeyCode::Insert;
    case VK_HOME:
      return KeyCode::Home;
    case VK_END:
      return KeyCode::End;
    case VK_PRIOR:
      return KeyCode::PageUp;
    case VK_NEXT:
      return KeyCode::PageDown;
    case VK_SHIFT:
    {
      UINT scan = (flags >> 16) & 0xFF;
      return MapVirtualKeyW(scan, MAPVK_VSC_TO_VK_EX) == VK_RSHIFT ? KeyCode::RightShift
                                                                   : KeyCode::LeftShift;
    }
    case VK_LSHIFT:
      return KeyCode::LeftShift;
    case VK_RSHIFT:
      return KeyCode::RightShift;
    case VK_CONTROL:
      return extended ? KeyCode::RightCtrl : KeyCode::LeftCtrl;
    case VK_LCONTROL:
      return KeyCode::LeftCtrl;
    case VK_RCONTROL:
      return KeyCode::RightCtrl;
    case VK_MENU:
      return extended ? KeyCode::RightAlt : KeyCode::LeftAlt;
    case VK_LMENU:
      return KeyCode::LeftAlt;
    case VK_RMENU:
      return KeyCode::RightAlt;
    case VK_OEM_COMMA:
      return KeyCode::Comma;
    case VK_OEM_PERIOD:
      return KeyCode::Period;
    case VK_OEM_MINUS:
      return KeyCode::Minus;
    case VK_OEM_PLUS:
      return KeyCode::Equals;
    case VK_OEM_2:
      return KeyCode::Slash;
    case VK_OEM_5:
      return KeyCode::Backslash;
    case VK_OEM_1:
      return KeyCode::Semicolon;
    case VK_OEM_7:
      return KeyCode::Apostrophe;
    case VK_OEM_4:
      return KeyCode::LeftBracket;
    case VK_OEM_6:
      return KeyCode::RightBracket;
    case VK_OEM_3:
      return KeyCode::GraveAccent;
    default:
      return KeyCode::Unknown;
    }
  }

  void push_event(Window *window, const WindowEvent &event)
  {
    if (window->event_count == EVENT_CAPACITY)
    {
      window->event_head = (window->event_head + 1) % EVENT_CAPACITY;
      window->event_count -= 1;
    }

    window->events[(window->event_head + window->event_count) % EVENT_CAPACITY] = event;
    window->event_count += 1;
  }

  WindowEvent make_event(WindowEventType type)
  {
    WindowEvent event = {};
    event.type        = type;
    return event;
  }

  void push_mouse_button(Window *window, MouseButton button, bool down, LPARAM l_param)
  {
    WindowEvent event =
        make_event(down ? WindowEventType::MousePressed : WindowEventType::MouseReleased);

    if (down)
    {
      event.mouse_pressed.button = button;
      event.mouse_pressed.x      = LOWORD(l_param);
      event.mouse_pressed.y      = HIWORD(l_param);
    }
    else
    {
      event.mouse_released.button = button;
      event.mouse_released.x      = LOWORD(l_param);
      event.mouse_released.y      = HIWORD(l_param);
    }

    push_event(window, event);
    if (down)
    {
      SetCapture(window->hwnd);
    }
    else if (
        (GetKeyState(VK_LBUTTON) & 0x8000) == 0 && (GetKeyState(VK_RBUTTON) & 0x8000) == 0 &&
        (GetKeyState(VK_MBUTTON) & 0x8000) == 0
    )
    {
      ReleaseCapture();
    }
  }

  LRESULT CALLBACK WindowProc(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
  {
    Window *window = reinterpret_cast<Window *>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    if (window == nullptr)
    {
      return DefWindowProc(hwnd, Msg, wParam, lParam);
    }

    switch (Msg)
    {
    case WM_CLOSE:
      window->closing = true;
      push_event(window, make_event(WindowEventType::CloseRequested));
      return 0;

    case WM_SIZE:
    {
      if (wParam != SIZE_MINIMIZED)
      {
        window->client_width  = LOWORD(lParam);
        window->client_height = HIWORD(lParam);

        WindowEvent event           = make_event(WindowEventType::Resized);
        event.window_resized.width  = window->client_width;
        event.window_resized.height = window->client_height;

        push_event(window, event);
      }
      return 0;
    }

    case WM_SETFOCUS:
    case WM_KILLFOCUS:
    {
      window->focused   = Msg == WM_SETFOCUS;
      WindowEvent event = make_event(
          Msg == WM_SETFOCUS ? WindowEventType::FocusGained : WindowEventType::FocusLost
      );
      push_event(window, event);
      return 0;
    }

    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
    case WM_KEYUP:
    case WM_SYSKEYUP:
    {
      bool    down = Msg == WM_KEYDOWN || Msg == WM_SYSKEYDOWN;
      KeyCode key  = translate_key(wParam, lParam);

      if (key != KeyCode::Unknown)
      {
        WindowEvent event =
            make_event(down ? WindowEventType::KeyPressed : WindowEventType::KeyReleased);

        if (down)
        {
          event.key_pressed.key    = key;
          event.key_pressed.repeat = (lParam & (1 << 30)) != 0;
        }
        else
        {
          event.key_released.key = key;
        }

        push_event(window, event);
      }

      if (Msg == WM_SYSKEYDOWN || Msg == WM_SYSKEYUP)
        return DefWindowProcA(hwnd, Msg, wParam, lParam);

      return 0;
    }

    case WM_MOUSEMOVE:
    {
      WindowEvent event   = make_event(WindowEventType::MouseMoved);
      event.mouse_moved.x = LOWORD(lParam);
      event.mouse_moved.y = HIWORD(lParam);
      push_event(window, event);
      return 0;
    }

    case WM_LBUTTONDOWN:
      push_mouse_button(window, MouseButton::Left, true, lParam);
      break;
    case WM_LBUTTONUP:
      push_mouse_button(window, MouseButton::Left, false, lParam);
      break;
    case WM_RBUTTONDOWN:
      push_mouse_button(window, MouseButton::Right, true, lParam);
      break;
    case WM_RBUTTONUP:
      push_mouse_button(window, MouseButton::Right, false, lParam);
      break;
    case WM_MBUTTONDOWN:
      push_mouse_button(window, MouseButton::Middle, true, lParam);
      break;
    case WM_MBUTTONUP:
      push_mouse_button(window, MouseButton::Middle, false, lParam);
      break;

    case WM_MOUSEWHEEL:
    case WM_MOUSEHWHEEL:
    {
      WindowEvent event = make_event(WindowEventType::MouseWheel);
      f32 delta = static_cast<f32>(GET_WHEEL_DELTA_WPARAM(wParam)) / static_cast<f32>(WHEEL_DELTA);

      if (Msg == WM_MOUSEWHEEL)
        event.mouse_wheel.y = delta;
      else
        event.mouse_wheel.x = delta;

      push_event(window, event);
      return 0;
    }

    default:
      break;
    }

    return DefWindowProc(hwnd, Msg, wParam, lParam);
  }

  static bool class_registered = false;
  bool        register_window_class()
  {
    if (class_registered)
    {
      return true;
    }

    WNDCLASSEXA wc   = {};
    wc.cbSize        = sizeof(wc);
    wc.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc   = WindowProc;
    wc.hInstance     = GetModuleHandle(nullptr);
    wc.hCursor       = LoadCursor(nullptr, IDC_ARROW);
    wc.lpszClassName = WINDOW_CLASS_NAME;

    if (RegisterClassExA(&wc) == 0)
    {
      return false;
    }

    class_registered = true;
    return true;
  }
}

Window *window_create(const WindowDesc &desc)
{
  if (!register_window_class())
  {
    ATR_LOG_ERROR("Failed to register window class");
    return nullptr;
  }

  Window *window = static_cast<Window *>(mem_alloc_zeroed(default_allocator(), sizeof(Window)));
  if (window == nullptr)
  {
    return nullptr;
  }

  window->focused = false;

  DWORD style = WS_OVERLAPPEDWINDOW;
  if (!desc.resizable) style &= ~(WS_THICKFRAME | WS_MAXIMIZEBOX);

  RECT rect = {0, 0, static_cast<LONG>(desc.width), static_cast<LONG>(desc.height)};
  AdjustWindowRectEx(&rect, style, FALSE, 0);

  window->hwnd = CreateWindowExA(
      0,
      WINDOW_CLASS_NAME,
      desc.title,
      style,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      rect.right - rect.left,
      rect.bottom - rect.top,
      nullptr,
      nullptr,
      GetModuleHandle(nullptr),
      nullptr
  );

  if (window->hwnd == nullptr)
  {
    ATR_LOG_ERROR("CreateWindowExA failed (%lu)", GetLastError());
    mem_free(default_allocator(), window, sizeof(Window));
    return nullptr;
  }

  SetWindowLongPtr(window->hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));

  RECT client;
  GetClientRect(window->hwnd, &client);
  window->client_width  = client.right - client.left;
  window->client_height = client.bottom - client.top;

  if (desc.visible)
  {
    ShowWindow(window->hwnd, SW_SHOW);
    window->focused = GetForegroundWindow() == window->hwnd;
  }
  if (desc.fullscreen) window_set_fullscreen(window, true);

  return window;
}

void window_destroy(Window *window)
{
  if (window == nullptr) return;

  SetWindowLongPtr(window->hwnd, GWLP_USERDATA, 0);
  DestroyWindow(window->hwnd);
  mem_free(default_allocator(), window, sizeof(Window));
}

void window_pump_messages()
{
  MSG Msg;

  while (PeekMessageA(&Msg, nullptr, 0, 0, PM_REMOVE))
  {
    TranslateMessage(&Msg);
    DispatchMessageA(&Msg);
  }
}

bool window_poll_event(Window *window, WindowEvent *out_event)
{
  if (window->event_count == 0) return false;

  *out_event         = window->events[window->event_head];
  window->event_head = (window->event_head + 1) % EVENT_CAPACITY;
  window->event_count--;

  return true;
}

void window_get_client_size(const Window *window, u32 *out_width, u32 *out_height)
{
  *out_width  = window->client_width;
  *out_height = window->client_height;
}

void window_set_title(Window *window, const char *title)
{
  SetWindowTextA(window->hwnd, title);
}

void window_set_fullscreen(Window *window, bool fullscreen)
{
  if (fullscreen == window->fullscreen) return;

  if (fullscreen)
  {
    window->window_placement.length = sizeof(WINDOWPLACEMENT);
    GetWindowPlacement(window->hwnd, &window->window_placement);
    window->windowed_style = GetWindowLongPtr(window->hwnd, GWL_STYLE);

    MONITORINFO monitor;
    GetMonitorInfo(MonitorFromWindow(window->hwnd, MONITOR_DEFAULTTONEAREST), &monitor);

    SetWindowLongPtr(window->hwnd, GWL_STYLE, window->windowed_style & ~WS_OVERLAPPEDWINDOW);
    SetWindowPos(
        window->hwnd,
        HWND_TOP,
        monitor.rcMonitor.left,
        monitor.rcMonitor.top,
        monitor.rcMonitor.right - monitor.rcMonitor.left,
        monitor.rcMonitor.bottom - monitor.rcMonitor.top,
        SWP_NOOWNERZORDER | SWP_FRAMECHANGED
    );
  }
  else
  {
    SetWindowLongPtr(window->hwnd, GWL_STYLE, window->windowed_style);
    SetWindowPlacement(window->hwnd, &window->window_placement);
    SetWindowPos(
        window->hwnd,
        nullptr,
        0,
        0,
        0,
        0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED
    );
  }

  window->fullscreen = fullscreen;
}

bool window_is_fullscreen(const Window *window)
{
  return window->fullscreen;
}

void window_set_borderless(Window *window, bool borderless)
{
  if (borderless == window->borderless) return;

  window->window_placement.length = sizeof(WINDOWPLACEMENT);
  GetWindowPlacement(window->hwnd, &window->window_placement);
  window->windowed_style = GetWindowLongPtr(window->hwnd, GWL_STYLE);

  MONITORINFO monitor;
  GetMonitorInfo(MonitorFromWindow(window->hwnd, MONITOR_DEFAULTTONEAREST), &monitor);

  DWORD dwStyle = WS_VISIBLE;
  dwStyle |= borderless ? WS_POPUP : WS_OVERLAPPEDWINDOW;
  SetWindowLongA(window->hwnd, GWL_STYLE, dwStyle);

  if (borderless)
  {
    ShowWindow(window->hwnd, SW_RESTORE);

    SetWindowPos(
        window->hwnd,
        nullptr,
        monitor.rcWork.left,
        monitor.rcWork.top,
        monitor.rcWork.right - monitor.rcWork.left,
        monitor.rcWork.bottom - monitor.rcWork.top,
        SWP_NOZORDER | SWP_FRAMECHANGED
    );
  }
  else
  {
    SetWindowPos(
        window->hwnd, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED
    );

    ShowWindow(window->hwnd, SW_SHOWMAXIMIZED);
  }

  window->borderless = borderless;
}

bool window_is_borderless(const Window *window)
{
  return window->borderless;
}

bool window_is_focused(const Window *window)
{
  return window->focused;
}

bool window_is_closing(const Window *window)
{
  return window->closing;
}

NativeWindowHandle window_get_native_handle(Window *window)
{
  return {.display = GetModuleHandleW(nullptr), .window = reinterpret_cast<u64>(window->hwnd)};
}