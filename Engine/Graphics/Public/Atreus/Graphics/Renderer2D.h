#pragma once

#include "Atreus/Math.h"
#include "Atreus/Types.h"

struct Window;
struct Renderer2D;

struct Color
{
  f32 r = 1.0f;
  f32 g = 1.0f;
  f32 b = 1.0f;
  f32 a = 1.0f;
};

// Initializes the Renderer2D, creates a context and compiles shaders
ATR_API Renderer2D *create_renderer2d(Window *window);
ATR_API void        destroy_renderer2d(Renderer2D *renderer);

// Must call at the start of each frame; clears command buffers
ATR_API void begin_frame2d(Renderer2D *renderer);

// Must call at the end of each frame; flushes command buffers
ATR_API void end_frame2d(Renderer2D *renderer);

// Records clear command into deferred command buffer, not drawn until end_frame2d is called
ATR_API void clear(Renderer2D *renderer, Color color);

// Records quad command into deferred command buffer, not drawn until end_frame2d is called
// Batched into sets of 1024; batches are broken by calling clear()
ATR_API void draw_quad(Renderer2D *renderer, Vec2 bottom_left, Vec2 size, Color color);
