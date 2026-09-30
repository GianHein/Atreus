#pragma once

#include "Atreus/Definitions.h"
#include "Atreus/Maths/Vector2D.h"
#include "Atreus/Platform/Window.h"

namespace Atreus
{
  struct RendererHandle
  {
    u32 index      = UINT32_MAX;
    u32 generation = UINT32_MAX;
  };

  constexpr RendererHandle InvalidRendererHandle = {UINT32_MAX, UINT32_MAX};

  struct RendererConfiguration
  {
    u32 logical_width  = 800;
    u32 logical_height = 600;
  };

  struct Color
  {
    u8 r, g, b, a;
  };

  b8 IsValidRenderer(RendererHandle handle);

  b8 InitializeRenderer2D(
      WindowHandle window, const RendererConfiguration &config, RendererHandle *out_handle
  );
  void DestroyRenderer2D(RendererHandle *handle);

  void Clear(RendererHandle handle, Color color);
  void DrawRectangle(RendererHandle handle, Vector2D top_left, Vector2D size, Color color);
  void DrawCircle(RendererHandle handle, Vector2D center, f32 radius, Color color);
  void Present(RendererHandle handle);
}