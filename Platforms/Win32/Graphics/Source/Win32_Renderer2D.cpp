#include "Atreus/Graphics/Renderer2D.h"
#include "Atreus/Log.h"

#include <glad/gl.h>
#include <glad/wgl.h>
#include <windows.h>

namespace Atreus
{
  static HDC   hdc  = nullptr;
  static HGLRC core = nullptr;

  static GLADapiproc LoadGLFunction(const char *name)
  {
    GLADapiproc proc = (GLADapiproc)wglGetProcAddress(name);
    if (proc == nullptr)
    {
      static HMODULE opengl32 = LoadLibraryA("opengl32.dll");
      proc                    = (GLADapiproc)GetProcAddress(opengl32, name);
    }
    return proc;
  }

  void InitializeGraphics(WindowHandle window)
  {
    ATR_ASSERT(
        hdc == nullptr && core == nullptr, "Cannot initialize graphics twice for the same window"
    );

    if (!IsValidWindow(window))
    {
      ATR_LOG_ERROR("Cannot initialize graphics using invalid window");
      return;
    }

    hdc = GetDC((HWND)GetNativeWindowHandle(window));

    PIXELFORMATDESCRIPTOR pfd = {};
    pfd.nSize                 = sizeof(pfd);
    pfd.nVersion              = 1;
    pfd.dwFlags               = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType            = PFD_TYPE_RGBA;
    pfd.cColorBits            = 32;
    pfd.cDepthBits            = 24;
    pfd.cStencilBits          = 8;

    i32 format = ChoosePixelFormat(hdc, &pfd);
    if (!SetPixelFormat(hdc, format, &pfd))
    {
      ATR_LOG_ERROR(
          "Failed to choose of set pixel format for device context of window on slot %u: %lu",
          window.index,
          GetLastError()
      );
      return;
    }

    HGLRC legacy = wglCreateContext(hdc);
    if (legacy == nullptr)
    {
      ATR_LOG_ERROR(
          "Failed to create context for device context of window on slot %u: %lu",
          window.index,
          GetLastError()
      );
      return;
    }

    if (!wglMakeCurrent(hdc, legacy))
    {
      ATR_LOG_ERROR(
          "Failed to make context current for device context of window on slot %u: %lu",
          window.index,
          GetLastError()
      );
      return;
    }

    ATR_LOG_DEBUG(
        "Created and made legacy OpenGL context current for window on slot %u", window.index
    );

    gladLoadWGL(hdc, (GLADloadfunc)wglGetProcAddress);

    i32 attribs[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB,
        3,
        WGL_CONTEXT_MINOR_VERSION_ARB,
        3,
        WGL_CONTEXT_PROFILE_MASK_ARB,
        WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0
    };

    if (wglCreateContextAttribsARB == nullptr)
    {
      ATR_LOG_ERROR(
          "wglCreateContextAttribsARB not available (WGL_ARB_create_context unsupported)"
      );
      return;
    }

    core = wglCreateContextAttribsARB(hdc, nullptr, attribs);
    if (core == nullptr)
    {
      ATR_LOG_ERROR(
          "wglCreateContextAttribsARB failed to create a 3.3 core context: %lu", GetLastError()
      );
      return;
    }

    if (!wglMakeCurrent(hdc, core))
    {
      ATR_LOG_ERROR(
          "Failed to make context current for device context of window on slot %u: %lu",
          window.index,
          GetLastError()
      );

      wglMakeCurrent(hdc, nullptr);
      wglDeleteContext(core);
      ReleaseDC((HWND)GetNativeWindowHandle(window), hdc);
      return;
    }

    ATR_LOG_DEBUG(
        "Created and made 3.3 Core OpenGL context current for window on slot %u", window.index
    );

    wglDeleteContext(legacy);

    if (!gladLoadGL(LoadGLFunction))
    {
      ATR_LOG_ERROR("gladLoadGL failed to load OpenGL functions");
      return;
    }
  }

  void ClearBackground(WindowHandle window, f32 r, f32 g, f32 b, f32 a)
  {
    ATR_ASSERT(
        hdc != nullptr && core != nullptr,
        "Cannot clear background before initializing graphics for window"
    );
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT);
  }

  void Present(WindowHandle window)
  {
    ATR_ASSERT(
        hdc != nullptr && core != nullptr, "Cannot present before initializing graphics for window"
    );
    SwapBuffers(hdc);
  }

  void ShutdownGraphics(WindowHandle window)
  {
    ATR_ASSERT(
        hdc != nullptr && core != nullptr, "Cannot shut down uninitialized graphics for window"
    );

    if (!IsValidWindow(window))
    {
      ATR_LOG_ERROR("Cannot shut down graphics using invalid window");
      return;
    }

    wglMakeCurrent(hdc, nullptr);
    wglDeleteContext(core);
    ATR_LOG_DEBUG("Freed current context and deleted 3.3 Core OpenGL context");
    ReleaseDC((HWND)GetNativeWindowHandle(window), hdc);

    hdc  = nullptr;
    core = nullptr;
  }
}