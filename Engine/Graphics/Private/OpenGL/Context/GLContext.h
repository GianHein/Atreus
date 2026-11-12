#pragma once

#include "Atreus/Types.h"

struct Window;
struct GLContext;

struct GLContextDesc
{
  i32  version_major = 3;
  i32  version_minor = 3;
  bool debug         = false;
  bool vsync         = true;
};

GLContext *gl_context_create(Window *window, const GLContextDesc &desc);
void       gl_context_destroy(GLContext *context);

void gl_context_make_current(GLContext *context);
void gl_context_swap_buffers(GLContext *context);
void gl_context_set_vsync(GLContext *context, bool enabled);
