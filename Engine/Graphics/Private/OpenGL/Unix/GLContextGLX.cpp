#include "Atreus/Log.h"
#include "Atreus/Memory.h"
#include "Atreus/Platform/Window.h"
#include "GLContext.h"

#define Window XWindow
#define KeyCode XKeyCode
#include <X11/Xlib.h>
#include <glad/gl.h>
#include <glad/glx.h>
#undef Window
#undef KeyCode

struct GLContext
{
  Display   *display;
  XWindow    window;
  GLXContext handle;
};

GLContext *GLContextCreate(Window *window, const GLContextDesc &desc)
{
  ATR_ASSERT(window != nullptr, "window required");

  NativeWindowHandle window_handle = WindowGetNativeHandle(window);
  Display           *display       = reinterpret_cast<Display *>(window_handle.display);
  XWindow            xwindow       = static_cast<XWindow>(window_handle.window);

  i32 screen = DefaultScreen(display);

  i32 glx_version = gladLoaderLoadGLX(display, screen);
  if (!glx_version)
  {
    ATR_LOG_ERROR("Could not load GLX");
    return nullptr;
  }

  XWindowAttributes window_attributes;
  XGetWindowAttributes(display, xwindow, &window_attributes);
  VisualID visual_id = XVisualIDFromVisual(window_attributes.visual);

  GLint visual_attributes[] = {
      GLX_X_RENDERABLE,
      True,
      GLX_DRAWABLE_TYPE,
      GLX_WINDOW_BIT,
      GLX_RENDER_TYPE,
      GLX_RGBA_BIT,
      GLX_DOUBLEBUFFER,
      True,
      None
  };

  i32          num_fbc = 0;
  GLXFBConfig *fbc     = glXChooseFBConfig(display, screen, visual_attributes, &num_fbc);

  GLXFBConfig config = nullptr;
  for (i32 i = 0; i < num_fbc; ++i)
  {
    GLint config_visual_id = 0;
    glXGetFBConfigAttrib(display, fbc[i], GLX_VISUAL_ID, &config_visual_id);
    if (static_cast<VisualID>(config_visual_id) == visual_id)
    {
      config = fbc[i];
      break;
    }
  }
  if (fbc != nullptr) XFree(fbc);

  if (config == nullptr)
  {
    ATR_LOG_ERROR("No GLX framebuffer config matches the window's visual");
    return nullptr;
  }

  GLint context_attributes[] = {
      GLX_CONTEXT_MAJOR_VERSION_ARB,
      desc.version_major,
      GLX_CONTEXT_MINOR_VERSION_ARB,
      desc.version_minor,
      GLX_CONTEXT_PROFILE_MASK_ARB,
      GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
      GLX_CONTEXT_FLAGS_ARB,
      desc.debug ? GLX_CONTEXT_DEBUG_BIT_ARB : 0,
      None
  };

  GLXContext handle =
      glXCreateContextAttribsARB(display, config, nullptr, True, context_attributes);
  if (!handle)
  {
    ATR_LOG_ERROR("Failed to create OpenGL context");
    return nullptr;
  }

  GLContext *context = static_cast<GLContext *>(MemAlloc(DefaultAllocator(), sizeof(GLContext)));
  context->display   = display;
  context->window    = xwindow;
  context->handle    = handle;

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

  glXMakeCurrent(context->display, 0, 0);
  glXDestroyContext(context->display, context->handle);
  MemFree(DefaultAllocator(), context, sizeof(GLContext));

  ATR_LOG_DEBUG("GLContext destroyed");
}

void GLContextMakeCurrent(GLContext *context)
{
  ATR_ASSERT(context != nullptr, "context required");

  glXMakeCurrent(context->display, context->window, context->handle);
}

void GLContextSwapBuffers(GLContext *context)
{
  ATR_ASSERT(context != nullptr, "context required");

  glXSwapBuffers(context->display, context->window);
}

void GLContextSetVSync(GLContext *context, bool enabled)
{
  ATR_ASSERT(context != nullptr, "context required");

  if (GLAD_GLX_EXT_swap_control)
  {
    glXSwapIntervalEXT(context->display, context->window, enabled ? 1 : 0);
  }
}