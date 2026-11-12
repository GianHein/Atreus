#pragma once

#include "Atreus/Core/Input.h"
#include "Atreus/Graphics/Renderer2D.h"
#include "Atreus/Memory.h"
#include "Atreus/Platform/Window.h"
#include "Atreus/Types.h"

#define ATR_APPLICATION_API_VERSION 0

struct ApplicationContext
{
  Window           *window;
  Renderer2D       *renderer;
  const Allocator  *allocator;
  const InputState *input;

  void *state;

  f32 delta_time;
  f32 alpha;

  bool quit_requested;
};

using ApplicationFn = void (*)(ApplicationContext *);

struct ApplicationAPI
{
  u32   api_version;
  usize state_size;

  void (*init)(ApplicationContext *context);
  void (*shutdown)(ApplicationContext *context);
  void (*update)(ApplicationContext *context);
  void (*render)(ApplicationContext *context);
};

using GetApplicationApiFn = const ApplicationAPI *(*)();
#define ATR_APPLICATION_ENTRY_NAME "atreus_get_application_api"