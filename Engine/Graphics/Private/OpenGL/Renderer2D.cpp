#include "Atreus/Graphics/Renderer2D.h"

#include "Atreus/Log.h"
#include "Atreus/Memory.h"
#include "Atreus/Platform/Window.h"
#include "GLContext.h"

#include <glad/gl.h>

namespace
{
  u32 CompileShader(GLenum type, const char *source)
  {
    u32 shader;
    shader = glCreateShader(type);

    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    i32  success;
    char info_log[512];

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
      glGetShaderInfoLog(shader, 512, nullptr, info_log);
      ATR_LOG_ERROR("Failed to compile shader: %s", info_log);

      glDeleteShader(shader);
      return 0;
    }

    ATR_LOG_DEBUG(
        "Compiled %s shader",
        type == GL_VERTEX_SHADER     ? "vertex"
        : type == GL_FRAGMENT_SHADER ? "fragment"
                                     : "unknown"
    );
    return shader;
  }

  u32 LinkProgram(u32 vertex_shader, u32 fragment_shader)
  {
    if (vertex_shader == 0 || fragment_shader == 0)
    {
      glDeleteShader(vertex_shader);
      glDeleteShader(fragment_shader);

      ATR_LOG_ERROR(
          "Failed to link program due to at least one of the passed shaders being invalid"
      );
      return 0;
    }

    u32 shader_program;
    shader_program = glCreateProgram();

    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    i32  success;
    char info_log[512];

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success)
    {
      glGetProgramInfoLog(shader_program, 512, nullptr, info_log);
      ATR_LOG_ERROR("Failed to link shader program: %s", info_log);

      glDeleteProgram(shader_program);
      return 0;
    }

    return shader_program;
  }

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
  u32         quad_program;
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

  const char *vertex_source = "#version 330 core\n"
                              "layout (location = 0) in vec3 aPos;\n"
                              "void main()\n"
                              "{\n"
                              " gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                              "}";

  const char *fragment_source = "#version 330 core\n"
                                "out vec4 FragColor;\n"
                                "void main()\n"
                                "{\n"
                                " FragColor = vec4(1.0, 0.5, 0.2, 1.0);\n"
                                "}";

  u32 quad_vertex_shader   = CompileShader(GL_VERTEX_SHADER, vertex_source);
  u32 quad_fragment_shader = CompileShader(GL_FRAGMENT_SHADER, fragment_source);
  renderer->quad_program   = LinkProgram(quad_vertex_shader, quad_fragment_shader);

  if (renderer->quad_program == 0)
  {
    ATR_LOG_ERROR("Failed to link shader program for Renderer2D");
    DestroyRenderer2D(renderer);
    return nullptr;
  }

  return renderer;
}

void DestroyRenderer2D(Renderer2D *renderer)
{
  ATR_ASSERT(renderer != nullptr, "renderer required");

  GLContextMakeCurrent(renderer->context);
  glDeleteProgram(renderer->quad_program);

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