#include "Atreus/Platform/Window.h"

#include "Atreus/Log.h"
#include "Atreus/Memory.h"

// Xlib declares types named `Window` and `KeyCode`, which collide with the engine's own types.
#define Window XWindow
#define KeyCode XKeyCode
#include <X11/XKBlib.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#undef Window
#undef KeyCode

namespace
{
  constexpr usize EVENT_CAPACITY = 2048;

  // Extra pointer buttons beyond the ones Xlib names.
  constexpr u32 BUTTON_WHEEL_LEFT  = 6;
  constexpr u32 BUTTON_WHEEL_RIGHT = 7;
  constexpr u32 BUTTON_BACK        = 8;
  constexpr u32 BUTTON_FORWARD     = 9;

  // Shared by every window, since WindowPumpMessages() drains a single connection.
  struct X11State
  {
    Display *display;
    XContext context; // Maps an XWindow to its Window.
    u32      window_count;

    Atom wm_protocols;
    Atom wm_delete_window;
    Atom net_wm_name;
    Atom utf8_string;
    Atom net_wm_state;
    Atom net_wm_state_fullscreen;
    Atom net_wm_state_hidden;
    Atom net_wm_state_maximized_vert;
    Atom net_wm_state_maximized_horz;
    Atom motif_wm_hints;
  };

  X11State x11;
}

struct Window
{
  XWindow     handle;
  Cursor      hidden_cursor;
  bool        mapped;
  bool        closing;
  bool        focused;
  bool        fullscreen;
  bool        borderless;
  bool        resizable;
  bool        minimized;
  bool        maximized;
  bool        cursor_visible;
  i32         x;
  i32         y;
  u32         client_width;
  u32         client_height;
  u32         fixed_width; // Size enforced through WM hints when not resizable.
  u32         fixed_height;
  bool        key_down[static_cast<u32>(KeyCode::Count)];
  WindowEvent events[EVENT_CAPACITY];
  u32         event_head;
  u32         event_count;
};

namespace
{
  bool X11Acquire()
  {
    if (x11.display == nullptr)
    {
      x11.display = XOpenDisplay(nullptr);
      if (x11.display == nullptr)
      {
        ATR_LOG_ERROR("Failed to open the X11 display");
        return false;
      }

      x11.context = XUniqueContext();

      // Report held keys as repeated KeyPress events without the synthetic KeyRelease in between.
      Bool supported = False;
      XkbSetDetectableAutoRepeat(x11.display, True, &supported);

      Display *display                = x11.display;
      x11.wm_protocols                = XInternAtom(display, "WM_PROTOCOLS", False);
      x11.wm_delete_window            = XInternAtom(display, "WM_DELETE_WINDOW", False);
      x11.net_wm_name                 = XInternAtom(display, "_NET_WM_NAME", False);
      x11.utf8_string                 = XInternAtom(display, "UTF8_STRING", False);
      x11.net_wm_state                = XInternAtom(display, "_NET_WM_STATE", False);
      x11.net_wm_state_fullscreen     = XInternAtom(display, "_NET_WM_STATE_FULLSCREEN", False);
      x11.net_wm_state_hidden         = XInternAtom(display, "_NET_WM_STATE_HIDDEN", False);
      x11.net_wm_state_maximized_vert = XInternAtom(display, "_NET_WM_STATE_MAXIMIZED_VERT", False);
      x11.net_wm_state_maximized_horz = XInternAtom(display, "_NET_WM_STATE_MAXIMIZED_HORZ", False);
      x11.motif_wm_hints              = XInternAtom(display, "_MOTIF_WM_HINTS", False);
    }

    x11.window_count += 1;
    return true;
  }

  void X11Release()
  {
    x11.window_count -= 1;
    if (x11.window_count == 0)
    {
      XCloseDisplay(x11.display);
      x11.display = nullptr;
    }
  }

  KeyCode TranslateKey(unsigned int keycode)
  {
    // Level 0 of the first group, so the result ignores Shift and matches the Win32 virtual keys.
    KeySym sym = XkbKeycodeToKeysym(x11.display, static_cast<XKeyCode>(keycode), 0, 0);

    if (sym >= XK_a && sym <= XK_z)
    {
      return static_cast<KeyCode>(static_cast<u8>(KeyCode::A) + (sym - XK_a));
    }
    if (sym >= XK_A && sym <= XK_Z)
    {
      return static_cast<KeyCode>(static_cast<u8>(KeyCode::A) + (sym - XK_A));
    }
    if (sym >= XK_0 && sym <= XK_9)
    {
      return static_cast<KeyCode>(static_cast<u8>(KeyCode::Num0) + (sym - XK_0));
    }
    if (sym >= XK_F1 && sym <= XK_F12)
    {
      return static_cast<KeyCode>(static_cast<u8>(KeyCode::F1) + (sym - XK_F1));
    }
    if (sym >= XK_KP_0 && sym <= XK_KP_9)
    {
      return static_cast<KeyCode>(static_cast<u8>(KeyCode::Numpad0) + (sym - XK_KP_0));
    }

    switch (sym)
    {
    case XK_Left:
      return KeyCode::Left;
    case XK_Right:
      return KeyCode::Right;
    case XK_Up:
      return KeyCode::Up;
    case XK_Down:
      return KeyCode::Down;
    case XK_space:
      return KeyCode::Space;
    case XK_Return:
      return KeyCode::Enter;
    case XK_Escape:
      return KeyCode::Escape;
    case XK_Tab:
    case XK_ISO_Left_Tab:
      return KeyCode::Tab;
    case XK_BackSpace:
      return KeyCode::Backspace;
    case XK_Delete:
      return KeyCode::Delete;
    case XK_Insert:
      return KeyCode::Insert;
    case XK_Home:
      return KeyCode::Home;
    case XK_End:
      return KeyCode::End;
    case XK_Prior:
      return KeyCode::PageUp;
    case XK_Next:
      return KeyCode::PageDown;

    case XK_Shift_L:
      return KeyCode::LeftShift;
    case XK_Shift_R:
      return KeyCode::RightShift;
    case XK_Control_L:
      return KeyCode::LeftCtrl;
    case XK_Control_R:
      return KeyCode::RightCtrl;
    case XK_Alt_L:
    case XK_Meta_L:
      return KeyCode::LeftAlt;
    case XK_Alt_R:
    case XK_Meta_R:
    case XK_ISO_Level3_Shift: // AltGr on most non-US layouts.
      return KeyCode::RightAlt;
    case XK_Super_L:
      return KeyCode::LeftSuper;
    case XK_Super_R:
      return KeyCode::RightSuper;

    case XK_comma:
      return KeyCode::Comma;
    case XK_period:
      return KeyCode::Period;
    case XK_minus:
      return KeyCode::Minus;
    case XK_equal:
      return KeyCode::Equals;
    case XK_slash:
      return KeyCode::Slash;
    case XK_backslash:
      return KeyCode::Backslash;
    case XK_semicolon:
      return KeyCode::Semicolon;
    case XK_apostrophe:
      return KeyCode::Apostrophe;
    case XK_bracketleft:
      return KeyCode::LeftBracket;
    case XK_bracketright:
      return KeyCode::RightBracket;
    case XK_grave:
      return KeyCode::GraveAccent;

    // With Num Lock off, level 0 of the keypad holds the navigation keysyms.
    case XK_KP_Insert:
      return KeyCode::Numpad0;
    case XK_KP_End:
      return KeyCode::Numpad1;
    case XK_KP_Down:
      return KeyCode::Numpad2;
    case XK_KP_Next:
      return KeyCode::Numpad3;
    case XK_KP_Left:
      return KeyCode::Numpad4;
    case XK_KP_Begin:
      return KeyCode::Numpad5;
    case XK_KP_Right:
      return KeyCode::Numpad6;
    case XK_KP_Home:
      return KeyCode::Numpad7;
    case XK_KP_Up:
      return KeyCode::Numpad8;
    case XK_KP_Prior:
      return KeyCode::Numpad9;
    case XK_KP_Delete:
    case XK_KP_Decimal:
    case XK_KP_Separator:
      return KeyCode::NumpadDecimal;
    case XK_KP_Add:
      return KeyCode::NumpadAdd;
    case XK_KP_Subtract:
      return KeyCode::NumpadSub;
    case XK_KP_Multiply:
      return KeyCode::NumpadMul;
    case XK_KP_Divide:
      return KeyCode::NumpadDiv;
    case XK_KP_Enter:
      return KeyCode::NumpadEnter;

    case XK_Caps_Lock:
      return KeyCode::CapsLock;
    case XK_Num_Lock:
      return KeyCode::NumLock;
    case XK_Scroll_Lock:
      return KeyCode::ScrollLock;
    case XK_Print:
      return KeyCode::PrintScreen;
    case XK_Pause:
      return KeyCode::Pause;

    default:
      return KeyCode::Unknown;
    }
  }

  bool TranslateButton(u32 button, MouseButton *out_button)
  {
    switch (button)
    {
    case Button1:
      *out_button = MouseButton::Left;
      return true;
    case Button2:
      *out_button = MouseButton::Middle;
      return true;
    case Button3:
      *out_button = MouseButton::Right;
      return true;
    case BUTTON_BACK:
      *out_button = MouseButton::X1;
      return true;
    case BUTTON_FORWARD:
      *out_button = MouseButton::X2;
      return true;
    default:
      return false;
    }
  }

  // X11 reports pointer positions from the top-left; the engine uses a bottom-left origin, Y-up.
  i32 FlipY(const Window *window, i32 y)
  {
    return static_cast<i32>(window->client_height) - 1 - y;
  }

  void PushEvent(Window *window, const WindowEvent &event)
  {
    if (window->event_count == EVENT_CAPACITY)
    {
      // Drop the oldest event rather than the newest so that state changes are never lost.
      window->event_head = (window->event_head + 1) % EVENT_CAPACITY;
      window->event_count -= 1;
    }

    window->events[(window->event_head + window->event_count) % EVENT_CAPACITY] = event;
    window->event_count += 1;
  }

  WindowEvent MakeEvent(WindowEventType type)
  {
    WindowEvent event = {};
    event.type        = type;
    return event;
  }

  void UpdateSizeHints(Window *window, bool fixed)
  {
    XSizeHints *hints = XAllocSizeHints();
    if (hints == nullptr) return;

    if (fixed)
    {
      hints->flags      = PMinSize | PMaxSize;
      hints->min_width  = static_cast<int>(window->fixed_width);
      hints->min_height = static_cast<int>(window->fixed_height);
      hints->max_width  = static_cast<int>(window->fixed_width);
      hints->max_height = static_cast<int>(window->fixed_height);
    }

    XSetWMNormalHints(x11.display, window->handle, hints);
    XFree(hints);
  }

  void UpdateDecorations(Window *window)
  {
    // Layout of the Motif hints property: flags, functions, decorations, input mode, status.
    constexpr unsigned long MWM_HINTS_DECORATIONS = 1ul << 1;

    unsigned long hints[5] = {};
    hints[0]               = MWM_HINTS_DECORATIONS;
    hints[2]               = window->borderless ? 0 : 1;

    XChangeProperty(
        x11.display,
        window->handle,
        x11.motif_wm_hints,
        x11.motif_wm_hints,
        32,
        PropModeReplace,
        reinterpret_cast<unsigned char *>(hints),
        ATR_ARRAY_COUNT(hints)
    );
  }

  // Detects the KeyRelease half of a classic auto-repeat pair, for servers without detectable
  // auto-repeat. The KeyPress that follows is then reported as a repeat through key_down.
  bool IsAutoRepeatRelease(const XKeyEvent &release)
  {
    if (XEventsQueued(x11.display, QueuedAfterReading) == 0) return false;

    XEvent next;
    XPeekEvent(x11.display, &next);

    return next.type == KeyPress && next.xkey.window == release.window &&
           next.xkey.keycode == release.keycode && next.xkey.time == release.time;
  }

  // The window manager owns minimized, maximized and fullscreen state, and reports it here.
  void HandleWmStateChange(Window *window)
  {
    Atom           type;
    int            format;
    unsigned long  count;
    unsigned long  bytes_after;
    unsigned char *data = nullptr;

    int result = XGetWindowProperty(
        x11.display,
        window->handle,
        x11.net_wm_state,
        0,
        1024,
        False,
        XA_ATOM,
        &type,
        &format,
        &count,
        &bytes_after,
        &data
    );
    if (result != Success) return;

    bool minimized  = false;
    bool fullscreen = false;
    bool max_vert   = false;
    bool max_horz   = false;

    Atom *states = reinterpret_cast<Atom *>(data);
    for (unsigned long i = 0; i < count; ++i)
    {
      if (states[i] == x11.net_wm_state_hidden) minimized = true;
      if (states[i] == x11.net_wm_state_fullscreen) fullscreen = true;
      if (states[i] == x11.net_wm_state_maximized_vert) max_vert = true;
      if (states[i] == x11.net_wm_state_maximized_horz) max_horz = true;
    }
    if (data != nullptr) XFree(data);

    bool maximized = max_vert && max_horz;

    if (minimized && !window->minimized)
    {
      PushEvent(window, MakeEvent(WindowEventType::Minimized));
    }
    else if (!minimized && maximized && (window->minimized || !window->maximized))
    {
      PushEvent(window, MakeEvent(WindowEventType::Maximized));
    }
    else if (!minimized && !maximized && (window->minimized || window->maximized))
    {
      PushEvent(window, MakeEvent(WindowEventType::Restored));
    }
    window->minimized = minimized;
    window->maximized = maximized;

    if (fullscreen != window->fullscreen)
    {
      window->fullscreen = fullscreen;
      if (!fullscreen) UpdateSizeHints(window, !window->resizable);

      PushEvent(
          window,
          MakeEvent(
              fullscreen ? WindowEventType::FullscreenEntered : WindowEventType::FullscreenLeft
          )
      );
    }
  }

  void HandleEvent(Window *window, XEvent *event)
  {
    switch (event->type)
    {
    case ClientMessage:
    {
      if (event->xclient.message_type == x11.wm_protocols &&
          static_cast<Atom>(event->xclient.data.l[0]) == x11.wm_delete_window)
      {
        window->closing = true;
        PushEvent(window, MakeEvent(WindowEventType::CloseRequested));
      }
      break;
    }

    case MapNotify:
      PushEvent(window, MakeEvent(WindowEventType::Shown));
      break;

    case UnmapNotify:
      PushEvent(window, MakeEvent(WindowEventType::Hidden));
      break;

    case ConfigureNotify:
    {
      u32 width  = static_cast<u32>(event->xconfigure.width);
      u32 height = static_cast<u32>(event->xconfigure.height);
      if (width != window->client_width || height != window->client_height)
      {
        window->client_width  = width;
        window->client_height = height;

        WindowEvent resized           = MakeEvent(WindowEventType::Resized);
        resized.window_resized.width  = width;
        resized.window_resized.height = height;
        PushEvent(window, resized);
      }

      // Reparenting window managers report coordinates relative to the frame, so ask for the
      // position on the root window instead.
      int     x;
      int     y;
      XWindow child;
      XTranslateCoordinates(
          x11.display, window->handle, DefaultRootWindow(x11.display), 0, 0, &x, &y, &child
      );
      if (x != window->x || y != window->y)
      {
        window->x = x;
        window->y = y;
        PushEvent(window, MakeEvent(WindowEventType::Moved));
      }
      break;
    }

    case PropertyNotify:
      if (event->xproperty.atom == x11.net_wm_state) HandleWmStateChange(window);
      break;

    case FocusIn:
    case FocusOut:
    {
      // Ignore transient focus changes caused by keyboard grabs (e.g. the WM's Alt+Tab).
      if (event->xfocus.mode == NotifyGrab || event->xfocus.mode == NotifyUngrab) break;
      if (event->xfocus.detail == NotifyPointer) break;

      bool focused = event->type == FocusIn;
      if (focused == window->focused) break;

      window->focused = focused;
      if (!focused)
      {
        // Releases that happen while unfocused are never delivered.
        memset(window->key_down, 0, sizeof(window->key_down));
      }
      PushEvent(
          window, MakeEvent(focused ? WindowEventType::FocusGained : WindowEventType::FocusLost)
      );
      break;
    }

    case EnterNotify:
    case LeaveNotify:
    {
      if (event->xcrossing.mode != NotifyNormal) break;

      PushEvent(
          window,
          MakeEvent(
              event->type == EnterNotify ? WindowEventType::MouseEntered
                                         : WindowEventType::MouseLeft
          )
      );
      break;
    }

    case KeyPress:
    {
      KeyCode key = TranslateKey(event->xkey.keycode);
      if (key == KeyCode::Unknown) break;

      bool &down = window->key_down[static_cast<u32>(key)];

      WindowEvent pressed        = MakeEvent(WindowEventType::KeyPressed);
      pressed.key_pressed.key    = key;
      pressed.key_pressed.repeat = down;
      PushEvent(window, pressed);

      down = true;
      break;
    }

    case KeyRelease:
    {
      if (IsAutoRepeatRelease(event->xkey)) break;

      KeyCode key = TranslateKey(event->xkey.keycode);
      if (key == KeyCode::Unknown) break;

      window->key_down[static_cast<u32>(key)] = false;

      WindowEvent released      = MakeEvent(WindowEventType::KeyReleased);
      released.key_released.key = key;
      PushEvent(window, released);
      break;
    }

    case MotionNotify:
    {
      WindowEvent moved   = MakeEvent(WindowEventType::MouseMoved);
      moved.mouse_moved.x = event->xmotion.x;
      moved.mouse_moved.y = FlipY(window, event->xmotion.y);
      PushEvent(window, moved);
      break;
    }

    case ButtonPress:
    case ButtonRelease:
    {
      bool pressed = event->type == ButtonPress;
      u32  button  = event->xbutton.button;

      // X11 reports each wheel notch as a press/release pair of buttons 4-7.
      if (button >= Button4 && button <= BUTTON_WHEEL_RIGHT)
      {
        if (!pressed) break;

        WindowEvent wheel = MakeEvent(WindowEventType::MouseWheel);
        switch (button)
        {
        case Button4:
          wheel.mouse_wheel.y = 1.0f;
          break;
        case Button5:
          wheel.mouse_wheel.y = -1.0f;
          break;
        case BUTTON_WHEEL_LEFT:
          wheel.mouse_wheel.x = -1.0f;
          break;
        case BUTTON_WHEEL_RIGHT:
          wheel.mouse_wheel.x = 1.0f;
          break;
        }
        PushEvent(window, wheel);
        break;
      }

      MouseButton mouse_button;
      if (!TranslateButton(button, &mouse_button)) break;

      // While a button is held, X11 grabs the pointer for the window implicitly, so no capture
      // call is needed to keep receiving events outside of it.
      if (pressed)
      {
        WindowEvent down          = MakeEvent(WindowEventType::MousePressed);
        down.mouse_pressed.button = mouse_button;
        down.mouse_pressed.x      = event->xbutton.x;
        down.mouse_pressed.y      = FlipY(window, event->xbutton.y);
        PushEvent(window, down);
      }
      else
      {
        WindowEvent up           = MakeEvent(WindowEventType::MouseReleased);
        up.mouse_released.button = mouse_button;
        up.mouse_released.x      = event->xbutton.x;
        up.mouse_released.y      = FlipY(window, event->xbutton.y);
        PushEvent(window, up);
      }
      break;
    }

    default:
      break;
    }
  }
}

Window *WindowCreate(const WindowDesc &desc)
{
  if (!X11Acquire()) return nullptr;

  Window *window = static_cast<Window *>(MemAllocZeroed(DefaultAllocator(), sizeof(Window)));
  if (window == nullptr)
  {
    X11Release();
    return nullptr;
  }

  window->cursor_visible = true;
  window->resizable      = desc.resizable;
  window->borderless     = desc.borderless;
  window->client_width   = desc.width;
  window->client_height  = desc.height;
  window->fixed_width    = desc.width;
  window->fixed_height   = desc.height;

  Display *display = x11.display;

  XSetWindowAttributes attributes = {};
  // A solid background makes the server paint the window when it is mapped. Under XWayland a
  // window that is never drawn to never gets a buffer, so the compositor never shows it.
  attributes.background_pixel = BlackPixel(display, DefaultScreen(display));
  attributes.event_mask = KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask |
                          PointerMotionMask | EnterWindowMask | LeaveWindowMask | FocusChangeMask |
                          StructureNotifyMask | PropertyChangeMask;

  window->handle = XCreateWindow(
      display,
      DefaultRootWindow(display),
      0,
      0,
      desc.width,
      desc.height,
      0,
      CopyFromParent,
      InputOutput,
      CopyFromParent,
      CWBackPixel | CWEventMask,
      &attributes
  );
  if (window->handle == 0)
  {
    ATR_LOG_ERROR("XCreateWindow failed");
    MemFree(DefaultAllocator(), window, sizeof(Window));
    X11Release();
    return nullptr;
  }

  XSaveContext(display, window->handle, x11.context, reinterpret_cast<XPointer>(window));

  // Ask the WM to send a message instead of killing the connection when the user closes us.
  XSetWMProtocols(display, window->handle, &x11.wm_delete_window, 1);

  XClassHint class_hint = {};
  class_hint.res_name   = const_cast<char *>("atreus");
  class_hint.res_class  = const_cast<char *>("Atreus");
  XSetClassHint(display, window->handle, &class_hint);

  char   empty_bits     = 0;
  Pixmap empty          = XCreateBitmapFromData(display, window->handle, &empty_bits, 1, 1);
  XColor black          = {};
  window->hidden_cursor = XCreatePixmapCursor(display, empty, empty, &black, &black, 0, 0);
  XFreePixmap(display, empty);

  WindowSetTitle(window, desc.title);
  UpdateSizeHints(window, !desc.resizable);
  if (desc.borderless) UpdateDecorations(window);
  if (desc.fullscreen) WindowSetFullscreen(window, true);

  if (desc.visible)
  {
    XMapWindow(display, window->handle);
    window->mapped = true;
  }

  XFlush(display);
  return window;
}

void WindowDestroy(Window *window)
{
  if (window == nullptr) return;

  Display *display = x11.display;

  XDeleteContext(display, window->handle, x11.context);
  XFreeCursor(display, window->hidden_cursor);
  XDestroyWindow(display, window->handle);
  XFlush(display);

  MemFree(DefaultAllocator(), window, sizeof(Window));
  X11Release();
}

void WindowPumpMessages()
{
  if (x11.display == nullptr) return;

  while (XPending(x11.display) > 0)
  {
    XEvent event;
    XNextEvent(x11.display, &event);

    // Events for windows that were already destroyed are simply dropped.
    XPointer data = nullptr;
    if (XFindContext(x11.display, event.xany.window, x11.context, &data) != 0) continue;

    HandleEvent(reinterpret_cast<Window *>(data), &event);
  }
}

bool WindowPollEvent(Window *window, WindowEvent *out_event)
{
  if (window->event_count == 0) return false;

  *out_event         = window->events[window->event_head];
  window->event_head = (window->event_head + 1) % EVENT_CAPACITY;
  window->event_count -= 1;
  return true;
}

void WindowGetClientSize(const Window *window, u32 *out_width, u32 *out_height)
{
  *out_width  = window->client_width;
  *out_height = window->client_height;
}

void WindowSetTitle(Window *window, const char *title)
{
  // WM_NAME for legacy window managers, _NET_WM_NAME for UTF-8 aware ones.
  XStoreName(x11.display, window->handle, title);
  XChangeProperty(
      x11.display,
      window->handle,
      x11.net_wm_name,
      x11.utf8_string,
      8,
      PropModeReplace,
      reinterpret_cast<const unsigned char *>(title),
      static_cast<int>(strlen(title))
  );
  XFlush(x11.display);
}

void WindowSetFullscreen(Window *window, bool fullscreen)
{
  // Some window managers refuse to fullscreen a window whose min and max size are pinned.
  if (fullscreen) UpdateSizeHints(window, false);

  if (!window->mapped)
  {
    // The WM only reads the initial state from the property when it maps the window.
    if (fullscreen)
    {
      XChangeProperty(
          x11.display,
          window->handle,
          x11.net_wm_state,
          XA_ATOM,
          32,
          PropModeReplace,
          reinterpret_cast<unsigned char *>(&x11.net_wm_state_fullscreen),
          1
      );
    }
    else
    {
      XDeleteProperty(x11.display, window->handle, x11.net_wm_state);
    }
  }
  else
  {
    constexpr long NET_WM_STATE_REMOVE = 0;
    constexpr long NET_WM_STATE_ADD    = 1;

    XEvent event               = {};
    event.xclient.type         = ClientMessage;
    event.xclient.window       = window->handle;
    event.xclient.message_type = x11.net_wm_state;
    event.xclient.format       = 32;
    event.xclient.data.l[0]    = fullscreen ? NET_WM_STATE_ADD : NET_WM_STATE_REMOVE;
    event.xclient.data.l[1]    = static_cast<long>(x11.net_wm_state_fullscreen);
    event.xclient.data.l[2]    = 0;
    event.xclient.data.l[3]    = 1; // Request comes from a normal application.

    XSendEvent(
        x11.display,
        DefaultRootWindow(x11.display),
        False,
        SubstructureRedirectMask | SubstructureNotifyMask,
        &event
    );
  }

  // window->fullscreen is updated once the WM confirms the change through _NET_WM_STATE.
  XFlush(x11.display);
}

bool WindowIsFullscreen(const Window *window)
{
  return window->fullscreen;
}

void WindowSetBorderless(Window *window, bool borderless)
{
  if (borderless == window->borderless) return;

  window->borderless = borderless;
  UpdateDecorations(window);
  XFlush(x11.display);
}

bool WindowIsBorderless(const Window *window)
{
  return window->borderless;
}

bool WindowIsFocused(const Window *window)
{
  return window->focused;
}

bool WindowIsClosing(const Window *window)
{
  return window->closing;
}

void WindowSetCursorVisible(Window *window, bool visible)
{
  window->cursor_visible = visible;

  if (visible)
  {
    XUndefineCursor(x11.display, window->handle);
  }
  else
  {
    XDefineCursor(x11.display, window->handle, window->hidden_cursor);
  }
  XFlush(x11.display);
}

NativeWindowHandle WindowGetNativeHandle(Window *window)
{
  return {
      reinterpret_cast<void *>(x11.display),
      reinterpret_cast<u64>(static_cast<uintptr_t>(window->handle))
  };
}

// Gamepads

void GamepadPoll(u32 index, GamepadState *out_state)
{
  // TODO: Gamepads aren't part of X11; implement this on top of evdev (/dev/input/event*).
  ATR_UNUSED(index);
  *out_state = {};
}
