#include "Atreus/Graphics/Renderer2D.h"
#include "Atreus/Log.h"
#include "OpenGL/GLContext.h"

#include <glad/gl.h>

namespace Atreus
{
  struct OpenGL_Renderer
  {
    u32          index          = UINT32_MAX;
    WindowHandle window         = InvalidWindowHandle;
    GLContext    context        = {nullptr, nullptr};
    u32          logical_width  = 0;
    u32          logical_height = 0;
  };

  static OpenGL_Renderer renderer_pool[MAX_WINDOWS];
  static b8              renderer_pool_used[MAX_WINDOWS];
  static u32             renderer_generations[MAX_WINDOWS];

  static OpenGL_Renderer *AllocateOpenGLRenderer()
  {
    for (u32 i = 0; i < MAX_WINDOWS; ++i)
    {
      if (!renderer_pool_used[i])
      {
        renderer_pool_used[i]  = true;
        renderer_pool[i].index = i;

        ATR_LOG_DEBUG("Renderer allocated on slot %u", i);
        return &renderer_pool[i];
      }
    }

    ATR_LOG_ERROR("Renderer pool exhausted, %u renderers reached", MAX_WINDOWS);
    return nullptr;
  }

  static void FreeOpenGLRenderer(OpenGL_Renderer *renderer)
  {
    u32 index = renderer->index;
    ATR_ASSERT(index < MAX_WINDOWS, "Tried to free a renderer with out-of-bounds index");
    ATR_ASSERT(&renderer_pool[index] == renderer, "Tried to free an invalid OpenGL_Renderer");

    *renderer                 = {};
    renderer_pool_used[index] = false;
    renderer_generations[index]++;

    ATR_LOG_DEBUG("Freed renderer on slot %u", index);
  }

  static OpenGL_Renderer *ResolveRenderer(RendererHandle handle)
  {
    if (handle.index >= MAX_WINDOWS) return nullptr;
    if (!renderer_pool_used[handle.index]) return nullptr;
    if (renderer_generations[handle.index] != handle.generation) return nullptr;

    return &renderer_pool[handle.index];
  }

  b8 IsValidRenderer(RendererHandle handle)
  {
    return ResolveRenderer(handle) != nullptr;
  }

  static void DeleteAndFreeOpenGLRenderer(OpenGL_Renderer *renderer)
  {
    DestroyGLContext(&renderer->context, renderer->window);
    renderer->window = InvalidWindowHandle;
    FreeOpenGLRenderer(renderer);
  }

  b8 InitializeRenderer2D(
      WindowHandle window, const RendererConfiguration &config, RendererHandle *out_handle
  )
  {
    ATR_ASSERT(out_handle != nullptr, "out_handle required");
    ATR_ASSERT(config.logical_width != 0, "config.logical_width must be non-zero");
    ATR_ASSERT(config.logical_height != 0, "config.logical_height must be non-zero");

    if (!IsValidWindow(window))
    {
      ATR_LOG_ERROR("Cannot initialize Renderer2D using invalid window");
      return false;
    }

    OpenGL_Renderer *renderer = AllocateOpenGLRenderer();
    if (!renderer) return false;

    renderer->window         = window;
    renderer->logical_width  = config.logical_width;
    renderer->logical_height = config.logical_height;

    if (!CreateGLContext(window, &renderer->context))
    {
      ATR_LOG_ERROR(
          "Failed to create context while initializing Renderer2D for window on slot %u",
          renderer->index
      );

      DeleteAndFreeOpenGLRenderer(renderer);
      return false;
    }

    ATR_LOG_INFO(
        "Initialized Renderer2D for slot %u with %s for window on slot %u",
        renderer->index,
        (const char *)glGetString(GL_VERSION),
        window.index
    );

    out_handle->index      = renderer->index;
    out_handle->generation = renderer_generations[renderer->index];
    return true;
  }

  void DestroyRenderer2D(RendererHandle *handle)
  {
    ATR_ASSERT(handle != nullptr, "Cannot destroy renderer for empty handle");

    OpenGL_Renderer *renderer = ResolveRenderer(*handle);
    if (renderer == nullptr)
    {
      ATR_LOG_WARN("Failed to resolve renderer from handle for slot %u", handle->index);
      return;
    }

    DeleteAndFreeOpenGLRenderer(renderer);

    ATR_LOG_INFO("Destroyed renderer for slot %u", handle->index);
    *handle = InvalidRendererHandle;
  }

  void Clear(RendererHandle handle, Color color)
  {
    OpenGL_Renderer *renderer = ResolveRenderer(handle);
    ATR_ASSERT(renderer != nullptr, "Tried to clear for an invalid renderer");

    MakeContextCurrent(renderer->context);

    glClearColor(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f);
    glClear(GL_COLOR_BUFFER_BIT);
  }

  void Present(RendererHandle handle)
  {
    OpenGL_Renderer *renderer = ResolveRenderer(handle);
    ATR_ASSERT(renderer != nullptr, "Tried to present for an invalid renderer");

    SwapContextBuffers(renderer->context);
  }
}