#include "Atreus/Graphics/Renderer2D.h"

#include "Atreus/Log.h"
#include "Atreus/Memory.h"
#include "Atreus/Platform/File.h"
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
    Rectangle
  };

  struct ClearCommandData
  {
    Color color;
  };

  struct RectangleCommandData
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
      ClearCommandData     clear;
      RectangleCommandData rectangle;
    };
  };

  constexpr u32 MAX_COMMANDS = 128;
}

struct Renderer2D
{
  Window     *window;
  GLContext  *context;
  uVec2       resolution;
  DrawCommand command_buffer[MAX_COMMANDS];
  u32         command_buffer_size;
  u32         rect_program;
  u32         vbo;
  u32         vao;
  u32         ebo;
  i32         u_color_loc;
  i32         u_resolution_loc;
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

  ATR_LOG_INFO("Renderer2D created");

  FileHandle vertex_file;
  FileHandle fragment_file;

  FileOpen(
      "../../Engine/Graphics/Private/OpenGL/Shaders/rect.vertex.glsl",
      FileAccessMode::Read,
      FileOpenMode::OpenExisting,
      &vertex_file
  );

  u64 vertex_size;
  FileGetSize(vertex_file, &vertex_size);

  char vertex_source[vertex_size + 1];
  u64  read;
  FileRead(vertex_file, FileSeekOrigin::Begin, 0, vertex_source, vertex_size, &read);
  vertex_source[read] = '\0';

  u32 rect_vertex_shader = CompileShader(GL_VERTEX_SHADER, vertex_source);
  FileClose(&vertex_file);

  FileOpen(
      "../../Engine/Graphics/Private/OpenGL/Shaders/rect.fragment.glsl",
      FileAccessMode::Read,
      FileOpenMode::OpenExisting,
      &fragment_file
  );

  u64 fragment_size;
  FileGetSize(fragment_file, &fragment_size);

  char fragment_source[fragment_size + 1];
  FileRead(fragment_file, FileSeekOrigin::Begin, 0, fragment_source, fragment_size, &read);
  fragment_source[read] = '\0';

  u32 rect_fragment_shader = CompileShader(GL_FRAGMENT_SHADER, fragment_source);
  FileClose(&fragment_file);

  renderer->rect_program = LinkProgram(rect_vertex_shader, rect_fragment_shader);

  renderer->u_color_loc = glGetUniformLocation(renderer->rect_program, "u_color");
  if (renderer->u_color_loc == -1) ATR_LOG_ERROR("u_color does not exist in rect program");

  renderer->u_resolution_loc = glGetUniformLocation(renderer->rect_program, "u_resolution");
  if (renderer->u_resolution_loc == -1)
    ATR_LOG_ERROR("u_resolution does not exist in rect program");

  if (renderer->rect_program == 0)
  {
    ATR_LOG_ERROR("Failed to link shader program for Renderer2D");
    DestroyRenderer2D(renderer);
    return nullptr;
  }

  glGenVertexArrays(1, &renderer->vao);
  glBindVertexArray(renderer->vao);

  glGenBuffers(1, &renderer->vbo);
  glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo);
  glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(f32), nullptr, GL_DYNAMIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(f32), (void *)0);
  glEnableVertexAttribArray(0);

  return renderer;
}

void DestroyRenderer2D(Renderer2D *renderer)
{
  ATR_ASSERT(renderer != nullptr, "renderer required");

  GLContextMakeCurrent(renderer->context);
  glDeleteProgram(renderer->rect_program);

  GLContextDestroy(renderer->context);
  MemFree(DefaultAllocator(), renderer, sizeof(Renderer2D));

  ATR_LOG_INFO("Renderer2D destroyed");
}

void BeginFrame2D(Renderer2D *renderer)
{
  GLContextMakeCurrent(renderer->context);

  u32 width, height;
  WindowGetClientSize(renderer->window, &width, &height);

  renderer->resolution = {.x = width, .y = height};
  glViewport(0, 0, width, height);
}

void EndFrame2D(Renderer2D *renderer)
{
  for (u32 i = 0; i < renderer->command_buffer_size; ++i)
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

    case DrawCommandType::Rectangle:
    {
      Vec2  top_left   = command.rectangle.top_left;
      Vec2  size       = command.rectangle.size;
      Color color      = command.rectangle.color;
      uVec2 resolution = renderer->resolution;

      float vertices[] = {
          top_left.x + size.x,
          top_left.y,
          top_left.x + size.x,
          top_left.y + size.y,
          top_left.x,
          top_left.y,
          top_left.x,
          top_left.y + size.y
      };

      glUseProgram(renderer->rect_program);
      glUniform2f(renderer->u_resolution_loc, (f32)resolution.x, (f32)resolution.y);
      glUniform4f(renderer->u_color_loc, color.r, color.b, color.g, color.a);

      glBindVertexArray(renderer->vao);
      glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo);
      glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
      glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
    break;

    default:
      break;
    }
  }

  renderer->command_buffer_size = 0;

  GLContextSwapBuffers(renderer->context);
}

void Clear(Renderer2D *renderer, Color color)
{
  PushDrawCommand(renderer, {.type = DrawCommandType::Clear, .clear = {.color = color}});
}

void DrawRect(Renderer2D *renderer, Vec2 top_left, Vec2 size, Color color)
{
  PushDrawCommand(
      renderer,
      {.type      = DrawCommandType::Rectangle,
       .rectangle = {.top_left = top_left, .size = size, .color = color}}
  );
}
