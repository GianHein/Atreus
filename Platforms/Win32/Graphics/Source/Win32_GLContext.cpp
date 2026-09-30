#include "Atreus/Log.h"
#include "OpenGL/GLContext.h"

#include <glad/gl.h>
#include <glad/wgl.h>
#include <windows.h>

namespace Atreus
{
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

  b8 CreateGLContext(WindowHandle window, GLContext *out_context)
  {
    out_context->device = GetDC((HWND)GetNativeWindowHandle(window));
    if (out_context->device == nullptr)
    {
      ATR_LOG_ERROR("Failed to retrieve device context from window on slot %u", window.index);
      return false;
    }

    PIXELFORMATDESCRIPTOR pfd = {};
    pfd.nSize                 = sizeof(pfd);
    pfd.nVersion              = 1;
    pfd.dwFlags               = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType            = PFD_TYPE_RGBA;
    pfd.cColorBits            = 32;
    pfd.cDepthBits            = 24;
    pfd.cStencilBits          = 8;

    if (GetPixelFormat((HDC)out_context->device) == 0)
    {
      i32 format = ChoosePixelFormat((HDC)out_context->device, &pfd);
      if (!SetPixelFormat((HDC)out_context->device, format, &pfd))
      {
        ATR_LOG_ERROR(
            "Failed to choose or set pixel format for device context of window on slot %u: %lu",
            window.index,
            GetLastError()
        );

        return false;
      }
    }

    HGLRC legacy = wglCreateContext((HDC)out_context->device);
    if (legacy == nullptr)
    {
      ATR_LOG_ERROR(
          "Failed to create context for device context of window on slot %u: %lu",
          window.index,
          GetLastError()
      );

      return false;
    }

    if (!wglMakeCurrent((HDC)out_context->device, legacy))
    {
      ATR_LOG_ERROR(
          "Failed to make context current for device context of window on slot %u: %lu",
          window.index,
          GetLastError()
      );

      wglDeleteContext(legacy);
      return false;
    }

    gladLoadWGL((HDC)out_context->device, (GLADloadfunc)wglGetProcAddress);

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

      wglMakeCurrent(nullptr, nullptr);
      wglDeleteContext(legacy);
      return false;
    }

    out_context->context = wglCreateContextAttribsARB((HDC)out_context->device, nullptr, attribs);
    if (out_context->context == nullptr)
    {
      ATR_LOG_ERROR(
          "wglCreateContextAttribsARB failed to create a 3.3 core context: %lu", GetLastError()
      );

      wglMakeCurrent(nullptr, nullptr);
      wglDeleteContext(legacy);
      return false;
    }

    if (!wglMakeCurrent((HDC)out_context->device, (HGLRC)out_context->context))
    {
      ATR_LOG_ERROR(
          "Failed to make context current for device context of window on slot %u: %lu",
          window.index,
          GetLastError()
      );

      wglMakeCurrent(nullptr, nullptr);
      wglDeleteContext(legacy);
      wglDeleteContext((HGLRC)out_context->context);
      out_context->context = nullptr;
      return false;
    }

    wglDeleteContext(legacy);

    if (!gladLoadGL(LoadGLFunction))
    {
      ATR_LOG_ERROR("gladLoadGL failed to load OpenGL functions");
      return false;
    }

    wglSwapIntervalEXT(1);

    return true;
  }

  void DestroyGLContext(GLContext *context, WindowHandle window)
  {
    if (wglGetCurrentContext() == context->context) wglMakeCurrent(nullptr, nullptr);

    if (context->context)
    {
      wglDeleteContext((HGLRC)context->context);
      context->context = nullptr;
    }

    if (context->device && IsValidWindow(window))
    {
      ReleaseDC((HWND)GetNativeWindowHandle(window), (HDC)context->device);
      context->device = nullptr;
    }
  }

  void MakeContextCurrent(const GLContext &context)
  {
    HGLRC current = wglGetCurrentContext();

    if (context.context == current)
    {
      return;
    }

    if (!wglMakeCurrent((HDC)context.device, (HGLRC)context.context))
    {
      ATR_LOG_ERROR("Failed to make context current: %lu", GetLastError());
    }
  }

  void SwapContextBuffers(const GLContext &context)
  {
    SwapBuffers((HDC)context.device);
  }
}