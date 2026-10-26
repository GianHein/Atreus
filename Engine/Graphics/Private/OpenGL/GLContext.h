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

GLContext *GLContextCreate(Window *window, const GLContextDesc &desc);
void       GLContextDestroy(GLContext *context);

void GLContextMakeCurrent(GLContext *context);
void GLContextSwapBuffers(GLContext *context);
void GLContextSetVSync(GLContext *context, bool enabled);
