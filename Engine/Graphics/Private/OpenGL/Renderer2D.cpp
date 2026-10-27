#include "Atreus/Graphics/Renderer2D.h"

#include "Atreus/Log.h"
#include "Atreus/Memory.h"
#include "Atreus/Platform/Window.h"
#include "GLContext.h"

#include <glad/gl.h>

namespace
{
  enum class DrawCommandType : u8
  {
    None = 0,
    Clear,
  };

  struct ClearCommandData
  {
    Color color;
  };

  struct DrawCommand
  {
    DrawCommandType type;

    union
    {
      ClearCommandData clear;
    };
  };

  constexpr u32 MAX_COMMANDS = 128;
}

struct Renderer2D
{
  Window     *window;
  GLContext  *context;
  DrawCommand command_buffer[MAX_COMMANDS];
  u32         command_buffer_size;
};

static void PushDrawCommand(Renderer2D *renderer, DrawCommand command)
{
  if (renderer->command_buffer_size > MAX_COMMANDS)
  {
    ATR_LOG_ERROR("Failed to push draw command, buffer capacity exhausted");
    return;
  }

  renderer->command_buffer[renderer->command_buffer_size++] = command;
}

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

  u32 width, height;
  WindowGetClientSize(renderer->window, &width, &height);

  glViewport(0, 0, width, height);
}

void EndFrame2D(Renderer2D *renderer)
{
  for (u32 i = 0; i <= renderer->command_buffer_size; ++i)
  {
    DrawCommand command = renderer->command_buffer[i];

    switch (command.type)
    {
    case DrawCommandType::Clear:
    {
      Color color = command.clear.color;

      glClearColor(color.r, color.g, color.b, color.a);
      glClear(GL_COLOR_BUFFER_BIT);
    }
    break;

    default:
      break;
    }
  }

  GLContextSwapBuffers(renderer->context);
}

void Clear(Renderer2D *renderer, Color color)
{
  PushDrawCommand(renderer, {.type = DrawCommandType::Clear, .clear = {.color = color}});
}