#pragma once

#include "Atreus/Definitions.h"
#include "Atreus/Platform/Window.h"

#include <glad/gl.h>

namespace Atreus
{
  struct GLContext
  {
    void *device;
    void *context;
  };

  b8   CreateGLContext(WindowHandle window, GLContext *out_context);
  void DestroyGLContext(GLContext *context, WindowHandle window);
  void MakeContextCurrent(const GLContext &context);
  void SwapContextBuffers(const GLContext &context);
}