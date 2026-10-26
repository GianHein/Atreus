#include "Atreus/Graphics/Renderer2D.h"

#include "Atreus/Log.h"
#include "Atreus/Memory.h"
#include "GLContext.h"

#include <glad/gl.h>

struct Renderer2D
{
  Window    *window;
  GLContext *context;
};

Renderer2D *CreateRenderer2D(Window *window)
{
  ATR_ASSERT(window != nullptr, "window required");

  GLContextDesc context_desc;
  GLContext    *context = GLContextCreate(window, context_desc);
  if (context == nullptr)
  {
    ATR_LOG_ERROR("Failed to create OpenGL context");
    return nullptr;
  }

  Renderer2D *renderer =
      static_cast<Renderer2D *>(MemAllocZeroed(DefaultAllocator(), sizeof(Renderer2D)));

  renderer->window  = window;
  renderer->context = context;

  ATR_LOG_INFO("Renderer2D created");
  return renderer;
}

void DestroyRenderer2D(Renderer2D *renderer)
{
  ATR_ASSERT(renderer != nullptr, "renderer required");

  GLContextDestroy(renderer->context);
  MemFree(DefaultAllocator(), renderer, sizeof(Renderer2D));

  ATR_LOG_INFO("Renderer2D destroyed");
}

void BeginFrame2D(Renderer2D *renderer)
{
  GLContextMakeCurrent(renderer->context);
}

void EndFrame2D(Renderer2D *renderer)
{
  GLContextSwapBuffers(renderer->context);
}

void Clear(Renderer2D *renderer, Color color)
{
  glClearColor(color.r, color.g, color.b, color.a);
  glClear(GL_COLOR_BUFFER_BIT);
}
