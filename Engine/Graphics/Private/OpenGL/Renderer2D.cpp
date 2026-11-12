#include "Atreus/Graphics/Renderer2D.h"

#include "Atreus/Log.h"
#include "Atreus/Memory.h"
#include "Atreus/Platform/Window.h"
#include "Atreus/Profiling.h"
#include "Context/GLContext.h"
#include "Shaders/quad.fragment.glsl.h"
#include "Shaders/quad.vertex.glsl.h"

#include <glad/gl.h>

namespace
{
  u32 compile_shader(GLenum type, const char *source)
  {
    u32 shader = 0;

    shader = glCreateShader(type);

    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    i32 success;

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
      i32 info_log_size;
      glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_log_size);

      char *info_log =
          info_log_size > 0 ? (char *)mem_alloc(default_allocator(), info_log_size) : nullptr;
      glGetShaderInfoLog(shader, info_log_size, nullptr, info_log);

      ATR_LOG_ERROR(
          "Failed to compile %s shader: %s",
          type == GL_VERTEX_SHADER     ? "vertex"
          : type == GL_FRAGMENT_SHADER ? "fragment"
                                       : "unknown",
          info_log ? info_log : "(no info log)"
      );

      mem_free(default_allocator(), info_log, info_log_size);
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

  u32 link_program(u32 vertex_shader, u32 fragment_shader)
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

    i32 success;

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success)
    {
      i32 info_log_size;
      glGetProgramiv(shader_program, GL_INFO_LOG_LENGTH, &info_log_size);

      char *info_log =
          info_log_size > 0 ? (char *)mem_alloc(default_allocator(), info_log_size) : nullptr;
      glGetProgramInfoLog(shader_program, info_log_size, nullptr, info_log);

      ATR_LOG_ERROR("Failed to link shader program: %s", info_log ? info_log : "(no info log)");

      mem_free(default_allocator(), info_log, info_log_size);
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
    Vec2  bottom_left;
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
    Vec2  bottom_left;
    Color color;
  };

  constexpr u32 MAX_QUADS    = 1024;
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
  u32         frame_quad_count;
  u32         frame_draw_calls;

  u32 dropped_draw_commands;
};

static void push_draw_command(Renderer2D *renderer, DrawCommand command)
{
  if (renderer->command_buffer_size >= MAX_COMMANDS)
  {
    renderer->dropped_draw_commands++;
    return;
  }

  renderer->command_buffer[renderer->command_buffer_size++] = command;
}

Renderer2D *create_renderer2d(Window *window)
{
  ATR_ASSERT(window != nullptr, "window required");

  GLContextDesc context_desc;
  GLContext    *context = gl_context_create(window, context_desc);
  if (context == nullptr)
  {
    ATR_LOG_ERROR("Failed to create OpenGL context");
    return nullptr;
  }

  Renderer2D *renderer =
      static_cast<Renderer2D *>(mem_alloc_zeroed(default_allocator(), sizeof(Renderer2D)));

  renderer->window  = window;
  renderer->context = context;

  u32 quad_vertex_shader   = compile_shader(GL_VERTEX_SHADER, quad_vertex_glsl);
  u32 quad_fragment_shader = compile_shader(GL_FRAGMENT_SHADER, quad_fragment_glsl);

  renderer->quad_program = link_program(quad_vertex_shader, quad_fragment_shader);

  if (renderer->quad_program == 0)
  {
    ATR_LOG_ERROR("Failed to link shader program for Renderer2D");
    destroy_renderer2d(renderer);
    return nullptr;
  }

  renderer->u_resolution_loc = glGetUniformLocation(renderer->quad_program, "u_resolution");
  if (renderer->u_resolution_loc == -1)
    ATR_LOG_ERROR("u_resolution does not exist in quad program");

  renderer->vertices =
      (QuadVertex *)mem_alloc(default_allocator(), MAX_VERTICES * sizeof(QuadVertex));

  glGenVertexArrays(1, &renderer->vao);
  glBindVertexArray(renderer->vao);

  glGenBuffers(1, &renderer->vbo);
  glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo);
  glBufferData(GL_ARRAY_BUFFER, MAX_VERTICES * sizeof(QuadVertex), nullptr, GL_DYNAMIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(QuadVertex), (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(QuadVertex), (void *)(2 * sizeof(f32)));
  glEnableVertexAttribArray(1);

  u32 *indices = (u32 *)mem_alloc(default_allocator(), MAX_INDICES * sizeof(u32));

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
  mem_free(default_allocator(), indices, MAX_INDICES * sizeof(u32));

  ATR_LOG_DEBUG(
      "Quad batch: %u quads, vertex buffer %zu KB, index buffer %zu KB",
      MAX_QUADS,
      MAX_VERTICES * sizeof(QuadVertex) / 1024,
      MAX_INDICES * sizeof(u32) / 1024
  );

  ATR_LOG_INFO("Renderer2D created");
  return renderer;
}

void destroy_renderer2d(Renderer2D *renderer)
{
  ATR_ASSERT(renderer != nullptr, "renderer required");

  gl_context_make_current(renderer->context);
  glDeleteProgram(renderer->quad_program);

  glDeleteVertexArrays(1, &renderer->vao);
  glDeleteBuffers(1, &renderer->vbo);
  glDeleteBuffers(1, &renderer->ebo);

  gl_context_destroy(renderer->context);

  if (renderer->vertices)
    mem_free(default_allocator(), renderer->vertices, MAX_VERTICES * sizeof(QuadVertex));

  mem_free(default_allocator(), renderer, sizeof(Renderer2D));

  ATR_LOG_INFO("Renderer2D destroyed");
}

void begin_frame2d(Renderer2D *renderer)
{
  ATR_PROFILE_SCOPE();
  gl_context_make_current(renderer->context);

  u32 width, height;
  window_get_client_size(renderer->window, &width, &height);

  renderer->resolution = {.x = width, .y = height};
  glViewport(0, 0, width, height);
}

static void flush_quads(Renderer2D *renderer)
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

  renderer->frame_quad_count += renderer->quad_count;
  renderer->frame_draw_calls++;

  renderer->quad_count = 0;
}

static void push_quad(Renderer2D *renderer, Vec2 bottom_left, Vec2 size, Color color)
{
  if (renderer->quad_count == MAX_QUADS) flush_quads(renderer);

  QuadVertex *vertex = &renderer->vertices[renderer->quad_count * 4];

  vertex[0] = {{bottom_left.x, bottom_left.y}, color};
  vertex[1] = {{bottom_left.x + size.x, bottom_left.y}, color};
  vertex[2] = {{bottom_left.x + size.x, bottom_left.y + size.y}, color};
  vertex[3] = {{bottom_left.x, bottom_left.y + size.y}, color};

  renderer->quad_count++;
}

void end_frame2d(Renderer2D *renderer)
{
  ATR_PROFILE_SCOPE();

  if (renderer->dropped_draw_commands > 0)
  {
    ATR_LOG_ERROR(
        "Draw command buffer exhausted, dropped %u commands", renderer->dropped_draw_commands
    );
    renderer->dropped_draw_commands = 0;
  }

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

      flush_quads(renderer);
      glClearColor(color.r, color.g, color.b, color.a);
      glClear(GL_COLOR_BUFFER_BIT);
    }
    break;

    case DrawCommandType::Quad:
    {
      push_quad(renderer, command.quad.bottom_left, command.quad.size, command.quad.color);
    }
    break;

    default:
      break;
    }
  }

  flush_quads(renderer);
  renderer->command_buffer_size = 0;

  ATR_PROFILE_PLOT("Renderer2D quads", static_cast<i64>(renderer->frame_quad_count));
  ATR_PROFILE_PLOT("Renderer2D draw calls", static_cast<i64>(renderer->frame_draw_calls));
  renderer->frame_quad_count = 0;
  renderer->frame_draw_calls = 0;

  gl_context_swap_buffers(renderer->context);
  ATR_PROFILE_FRAME();
}

void clear(Renderer2D *renderer, Color color)
{
  push_draw_command(renderer, {.type = DrawCommandType::Clear, .clear = {.color = color}});
}

void draw_quad(Renderer2D *renderer, Vec2 bottom_left, Vec2 size, Color color)
{
  push_draw_command(
      renderer,
      {.type = DrawCommandType::Quad,
       .quad = {.bottom_left = bottom_left, .size = size, .color = color}}
  );
}
