#include "Atreus/Graphics/Renderer2D.h"

#include "Atreus/Log.h"
#include "Atreus/Memory.h"
#include "Atreus/Platform/Window.h"
#include "Atreus/Profiling.h"
#include "GLContext.h"
#include "Shaders/quad.fragment.glsl.h"
#include "Shaders/quad.vertex.glsl.h"

#include <glad/gl.h>

namespace
{
  u32 CompileShader(GLenum type, const char *source)
  {
    u32 shader = 0;

    shader = glCreateShader(type);

    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    i32  success;
    char info_log[512];

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
      glGetShaderInfoLog(shader, 512, nullptr, info_log);

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
    Quad
  };

  struct ClearCommandData
  {
    Color color;
  };

  struct QuadCommandData
  {
    Vec2  top_left;
    Vec2  size;
    Color color;
  };

  struct DrawCommand
  {
    DrawCommandType type;

    union
    {
      ClearCommandData clear;
      QuadCommandData  quad;
    };
  };

  constexpr u32 MAX_COMMANDS = 1024;

  struct QuadVertex
  {
    Vec2  position;
    Color color;
  };

  constexpr u32 MAX_QUADS    = 8192;
  constexpr u32 MAX_VERTICES = MAX_QUADS * 4;
  constexpr u32 MAX_INDICES  = MAX_QUADS * 6;
}

struct Renderer2D
{
  Window     *window;
  GLContext  *context;
  uVec2       resolution;
  DrawCommand command_buffer[MAX_COMMANDS];
  u32         command_buffer_size;

  u32 quad_program;

  u32 vbo;
  u32 vao;
  u32 ebo;
  i32 u_resolution_loc;

  QuadVertex *vertices;
  u32         quad_count;
};

static void PushDrawCommand(Renderer2D *renderer, DrawCommand command)
{
  if (renderer->command_buffer_size >= MAX_COMMANDS)
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

  u32 quad_vertex_shader   = CompileShader(GL_VERTEX_SHADER, quad_vertex_glsl);
  u32 quad_fragment_shader = CompileShader(GL_FRAGMENT_SHADER, quad_fragment_glsl);

  renderer->quad_program = LinkProgram(quad_vertex_shader, quad_fragment_shader);

  if (renderer->quad_program == 0)
  {
    ATR_LOG_ERROR("Failed to link shader program for Renderer2D");
    DestroyRenderer2D(renderer);
    return nullptr;
  }

  renderer->u_resolution_loc = glGetUniformLocation(renderer->quad_program, "u_resolution");
  if (renderer->u_resolution_loc == -1)
    ATR_LOG_ERROR("u_resolution does not exist in quad program");

  renderer->vertices =
      (QuadVertex *)MemAlloc(DefaultAllocator(), MAX_VERTICES * sizeof(QuadVertex));

  glGenVertexArrays(1, &renderer->vao);
  glBindVertexArray(renderer->vao);

  glGenBuffers(1, &renderer->vbo);
  glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo);
  glBufferData(GL_ARRAY_BUFFER, MAX_VERTICES * sizeof(QuadVertex), nullptr, GL_DYNAMIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(QuadVertex), (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(QuadVertex), (void *)(2 * sizeof(f32)));
  glEnableVertexAttribArray(1);

  u32 *indices = (u32 *)MemAlloc(DefaultAllocator(), MAX_INDICES * sizeof(u32));

  for (u32 i = 0; i < MAX_QUADS; ++i)
  {
    u32 base           = i * 4;
    indices[i * 6 + 0] = base + 0;
    indices[i * 6 + 1] = base + 1;
    indices[i * 6 + 2] = base + 2;
    indices[i * 6 + 3] = base + 2;
    indices[i * 6 + 4] = base + 3;
    indices[i * 6 + 5] = base + 0;
  }

  glGenBuffers(1, &renderer->ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer->ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, MAX_INDICES * sizeof(u32), indices, GL_STATIC_DRAW);
  MemFree(DefaultAllocator(), indices, MAX_INDICES * sizeof(u32));

  ATR_LOG_INFO("Renderer2D created");
  return renderer;
}

void DestroyRenderer2D(Renderer2D *renderer)
{
  ATR_ASSERT(renderer != nullptr, "renderer required");

  GLContextMakeCurrent(renderer->context);
  glDeleteProgram(renderer->quad_program);

  glDeleteVertexArrays(1, &renderer->vao);
  glDeleteBuffers(1, &renderer->vbo);
  glDeleteBuffers(1, &renderer->ebo);

  GLContextDestroy(renderer->context);

  if (renderer->vertices)
    MemFree(DefaultAllocator(), renderer->vertices, MAX_VERTICES * sizeof(QuadVertex));

  MemFree(DefaultAllocator(), renderer, sizeof(Renderer2D));

  ATR_LOG_INFO("Renderer2D destroyed");
}

void BeginFrame2D(Renderer2D *renderer)
{
  ATR_PROFILE_SCOPE();
  GLContextMakeCurrent(renderer->context);

  u32 width, height;
  WindowGetClientSize(renderer->window, &width, &height);

  renderer->resolution = {.x = width, .y = height};
  glViewport(0, 0, width, height);
}

static void FlushQuads(Renderer2D *renderer)
{
  if (renderer->quad_count == 0) return;

  glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo);
  glBufferData(GL_ARRAY_BUFFER, MAX_VERTICES * sizeof(QuadVertex), nullptr, GL_DYNAMIC_DRAW);
  glBufferSubData(
      GL_ARRAY_BUFFER, 0, renderer->quad_count * 4 * sizeof(QuadVertex), renderer->vertices
  );

  glUseProgram(renderer->quad_program);
  glBindVertexArray(renderer->vao);
  glDrawElements(GL_TRIANGLES, renderer->quad_count * 6, GL_UNSIGNED_INT, nullptr);

  renderer->quad_count = 0;
}

static void PushQuad(Renderer2D *renderer, Vec2 position, Vec2 size, Color color)
{
  if (renderer->quad_count == MAX_QUADS) FlushQuads(renderer);

  QuadVertex *vertex = &renderer->vertices[renderer->quad_count * 4];

  vertex[0] = {{position.x, position.y}, color};
  vertex[1] = {{position.x + size.x, position.y}, color};
  vertex[2] = {{position.x + size.x, position.y + size.y}, color};
  vertex[3] = {{position.x, position.y + size.y}, color};

  renderer->quad_count++;
}

void EndFrame2D(Renderer2D *renderer)
{
  ATR_PROFILE_SCOPE();

  glUseProgram(renderer->quad_program);
  glUniform2f(renderer->u_resolution_loc, (f32)renderer->resolution.x, (f32)renderer->resolution.y);

  for (u32 i = 0; i < renderer->command_buffer_size; ++i)
  {
    DrawCommand command = renderer->command_buffer[i];

    switch (command.type)
    {
    case DrawCommandType::Clear:
    {
      Color color = command.clear.color;

      FlushQuads(renderer);
      glClearColor(color.r, color.g, color.b, color.a);
      glClear(GL_COLOR_BUFFER_BIT);
    }
    break;

    case DrawCommandType::Quad:
    {
      PushQuad(renderer, command.quad.top_left, command.quad.size, command.quad.color);
    }
    break;

    default:
      break;
    }
  }

  FlushQuads(renderer);
  renderer->command_buffer_size = 0;

  GLContextSwapBuffers(renderer->context);
  ATR_PROFILE_FRAME();
}

void Clear(Renderer2D *renderer, Color color)
{
  PushDrawCommand(renderer, {.type = DrawCommandType::Clear, .clear = {.color = color}});
}

void DrawQuad(Renderer2D *renderer, Vec2 top_left, Vec2 size, Color color)
{
  PushDrawCommand(
      renderer,
      {.type = DrawCommandType::Quad, .quad = {.top_left = top_left, .size = size, .color = color}}
  );
}
