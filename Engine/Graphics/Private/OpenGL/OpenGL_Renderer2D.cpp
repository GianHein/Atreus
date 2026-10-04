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
    u32          quad_program   = 0;
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
    if (renderer->context.device != nullptr && renderer->context.context != nullptr)
    {
      MakeContextCurrent(renderer->context);
      glDeleteProgram(renderer->quad_program);
    }

    DestroyGLContext(&renderer->context, renderer->window);
    renderer->window = InvalidWindowHandle;
    FreeOpenGLRenderer(renderer);
  }

  static u32 CompileShader(GLenum type, const char *source)
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

    return shader;
  }

  static u32 LinkProgram(u32 vertex_shader, u32 fragment_shader)
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

    u32 vertex_shader   = CompileShader(GL_VERTEX_SHADER, vertex_source);
    u32 fragment_shader = CompileShader(GL_FRAGMENT_SHADER, fragment_source);
    u32 quad_program    = LinkProgram(vertex_shader, fragment_shader);

    if (quad_program == 0)
    {
      ATR_LOG_ERROR(
          "Failed to link quad program for Renderer2D for slot %u for window on slot %u",
          renderer->index,
          window.index
      );

      DeleteAndFreeOpenGLRenderer(renderer);
      return false;
    }

    renderer->quad_program = quad_program;

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

  enum class CommandType : u8
  {
    None = 0,
    Clear,
    Rectangle
  };

  struct ClearCommand
  {
    Color color;
  };

  struct RectangleCommand
  {
    Vector2D top_left;
    Vector2D size;
    Color    color;
  };

  struct DrawCommand
  {
    CommandType    type;
    RendererHandle renderer;

    union
    {
      ClearCommand     clear;
      RectangleCommand rectangle;
    };
  };

  static constexpr u32 MAX_COMMANDS = 128;
  static DrawCommand   command_buffer[MAX_COMMANDS];
  static u32           command_buffer_size;

  static void PushDrawCommand(DrawCommand command)
  {
    if (command_buffer_size == MAX_COMMANDS)
      return;
    else
      command_buffer[command_buffer_size++] = command;
  }

  void Clear(RendererHandle handle, Color color)
  {
    // OpenGL_Renderer *renderer = ResolveRenderer(handle);
    // ATR_ASSERT(renderer != nullptr, "Tried to clear for an invalid renderer");

    // MakeContextCurrent(renderer->context);

    // glClearColor(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f);
    // glClear(GL_COLOR_BUFFER_BIT);

    PushDrawCommand({.type = CommandType::Clear, .renderer = handle, .clear = {.color = color}});
  }

  void DrawRectangle(RendererHandle handle, Vector2D top_left, Vector2D size, Color color)
  {
    // OpenGL_Renderer *renderer = ResolveRenderer(handle);
    // ATR_ASSERT(renderer != nullptr, "Tried to draw rect for an invalid renderer");

    // MakeContextCurrent(renderer->context);

    PushDrawCommand(
        {.type      = CommandType::Rectangle,
         .renderer  = handle,
         .rectangle = {.top_left = top_left, .size = size, .color = color}}
    );
  }

  static void FlushDrawCommands()
  {
    for (u32 i = 0; i < MAX_COMMANDS; ++i)
    {
      DrawCommand command = command_buffer[i];

      switch (command.type)
      {
      case CommandType::Clear:
      {
        OpenGL_Renderer *renderer = ResolveRenderer(command.renderer);
        ATR_ASSERT(renderer != nullptr, "Tried to clear for an invalid renderer");

        MakeContextCurrent(renderer->context);

        glClearColor(
            command.clear.color.r / 255.0f,
            command.clear.color.g / 255.0f,
            command.clear.color.b / 255.0f,
            command.clear.color.a / 255.0f
        );

        glClear(GL_COLOR_BUFFER_BIT);

        break;
      }

      case CommandType::Rectangle:
      {
        break;
      }

      default:
        break;
      }
    }
  }

  void Present(RendererHandle handle)
  {
    OpenGL_Renderer *renderer = ResolveRenderer(handle);
    ATR_ASSERT(renderer != nullptr, "Tried to present for an invalid renderer");

    SwapContextBuffers(renderer->context);
  }
}