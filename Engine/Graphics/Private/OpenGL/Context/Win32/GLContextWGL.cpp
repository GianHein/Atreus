#include "Atreus/Log.h"
#include "Atreus/Memory.h"
#include "Atreus/Platform/Window.h"
#include "Context/GLContext.h"

#include <glad/gl.h>
#include <glad/wgl.h>

#ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>

struct GLContext
{
  HWND  hwnd;
  HDC   hdc;
  HGLRC context;
};

GLContext *GLContextCreate(Window *window, const GLContextDesc &desc)
{
  HWND hwnd = (HWND)WindowGetNativeHandle(window).window;
  HDC  hdc  = GetDC(hwnd);

  PIXELFORMATDESCRIPTOR pfd = {};
  pfd.nSize                 = sizeof(PIXELFORMATDESCRIPTOR);
  pfd.nVersion              = 1;
  pfd.dwFlags               = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
  pfd.iPixelType            = PFD_TYPE_RGBA;
  pfd.cColorBits            = 32;
  pfd.cDepthBits            = 24;
  pfd.cStencilBits          = 8;

  if (!GetPixelFormat(hdc))
  {
    i32 format = ChoosePixelFormat(hdc, &pfd);

    if (!SetPixelFormat(hdc, format, &pfd))
    {
      ATR_LOG_ERROR("Failed to choose or set pixel format for device context: %lu", GetLastError());
      return nullptr;
    }
  }

  HGLRC legacy = wglCreateContext(hdc);
  if (legacy == nullptr)
  {
    ATR_LOG_ERROR("Failed to create context for device context: %lu", GetLastError());
    return nullptr;
  }

  if (!wglMakeCurrent(hdc, legacy))
  {
    ATR_LOG_ERROR("Failed to make context current for device context: %lu", GetLastError());
    wglDeleteContext(legacy);
    return nullptr;
  }

  if (!gladLoaderLoadWGL(hdc) || !GLAD_WGL_ARB_create_context ||
      !GLAD_WGL_ARB_create_context_profile)
  {
    wglMakeCurrent(nullptr, nullptr);
    wglDeleteContext(legacy);
    return nullptr;
  }

  i32 attribs[] = {
      WGL_CONTEXT_MAJOR_VERSION_ARB,
      desc.version_major,
      WGL_CONTEXT_MINOR_VERSION_ARB,
      desc.version_minor,
      WGL_CONTEXT_PROFILE_MASK_ARB,
      WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
      WGL_CONTEXT_FLAGS_ARB,
      desc.debug ? WGL_CONTEXT_DEBUG_BIT_ARB : 0,
      0
  };

  GLContext *context = static_cast<GLContext *>(MemAlloc(DefaultAllocator(), sizeof(GLContext)));
  context->context   = wglCreateContextAttribsARB(hdc, nullptr, attribs);
  context->hdc       = hdc;
  context->hwnd      = hwnd;

  GLContextMakeCurrent(context);

  if (!gladLoaderLoadGL())
  {
    ATR_LOG_ERROR("Failed to load OpenGL functions");
    GLContextDestroy(context);
    return nullptr;
  }

  if (desc.vsync) GLContextSetVSync(context, true);

  ATR_LOG_DEBUG("GLContext created");
  return context;
}

void GLContextDestroy(GLContext *context)
{
  ATR_ASSERT(context != nullptr, "context required");

  wglMakeCurrent(nullptr, nullptr);
  wglDeleteContext(context->context);
  MemFree(DefaultAllocator(), context, sizeof(GLContext));

  ATR_LOG_DEBUG("GLContext destroyed");
}

void GLContextMakeCurrent(GLContext *context)
{
  ATR_ASSERT(context != nullptr, "context required");

  wglMakeCurrent(context->hdc, context->context);
}

void GLContextSwapBuffers(GLContext *context)
{
  ATR_ASSERT(context != nullptr, "context required");

  SwapBuffers(context->hdc);
}

void GLContextSetVSync(GLContext *context, bool enabled)
{
  ATR_ASSERT(context != nullptr, "context required");

  GLContextMakeCurrent(context);
  wglSwapIntervalEXT(enabled ? 1 : 0);
}