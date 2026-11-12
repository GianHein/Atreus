#pragma once

#include "Atreus/Math.h"
#include "Atreus/Platform/Window.h"

constexpr u32 INPUT_KEY_COUNT          = (u32)KeyCode::Count;
constexpr u32 INPUT_MOUSE_BUTTON_COUNT = (u32)MouseButton::Count;

struct InputState
{
  bool key_down[INPUT_KEY_COUNT];
  bool key_pressed[INPUT_KEY_COUNT];
  bool key_released[INPUT_KEY_COUNT];

  Vec2 mouse_position;
  Vec2 mouse_delta;
  f32  mouse_wheel;
  bool mouse_down[INPUT_MOUSE_BUTTON_COUNT];
  bool mouse_pressed[INPUT_MOUSE_BUTTON_COUNT];
  bool mouse_released[INPUT_MOUSE_BUTTON_COUNT];
};

ATR_API void input_init(InputState *input);

ATR_API void input_clear_transients(InputState *input);
ATR_API void input_handle_event(InputState *input, const WindowEvent &event);

inline bool input_key_down(const InputState *input, KeyCode key)
{
  return input->key_down[(u32)key];
}

inline bool input_key_pressed(const InputState *input, KeyCode key)
{
  return input->key_pressed[(u32)key];
}

inline bool input_key_released(const InputState *input, KeyCode key)
{
  return input->key_released[(u32)key];
}

inline bool input_mouse_down(const InputState *input, MouseButton button)
{
  return input->mouse_down[(u32)button];
}

inline bool input_mouse_pressed(const InputState *input, MouseButton button)
{
  return input->mouse_pressed[(u32)button];
}

inline bool input_mouse_released(const InputState *input, MouseButton button)
{
  return input->mouse_released[(u32)button];
}