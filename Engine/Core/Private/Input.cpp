#include "Atreus/Core/Input.h"

#include <cstring>

void input_init(InputState *input)
{
  memset(input, 0, sizeof(InputState));
}

void input_clear_transients(InputState *input)
{
  memset(input->key_pressed, 0, sizeof(input->key_pressed));
  memset(input->key_released, 0, sizeof(input->key_released));
  memset(input->mouse_pressed, 0, sizeof(input->mouse_pressed));
  memset(input->mouse_released, 0, sizeof(input->mouse_released));

  input->mouse_delta = {0, 0};
  input->mouse_wheel = 0.0f;
}

void input_handle_event(InputState *input, const WindowEvent &event)
{
  switch (event.type)
  {
  case WindowEventType::KeyPressed:
  {
    u32 index = (u32)event.key_pressed.key;

    if (!event.key_pressed.repeat) input->key_pressed[index] = true;
    input->key_down[index] = true;
  }
  break;

  case WindowEventType::KeyReleased:
  {
    u32 index = (u32)event.key_released.key;

    input->key_released[index] = true;
    input->key_down[index]     = false;
  }
  break;

  case WindowEventType::MouseMoved:
  {
    Vec2 old = input->mouse_position;
    Vec2 now = {(f32)event.mouse_moved.x, (f32)event.mouse_moved.y};

    input->mouse_position = now;
    input->mouse_delta += now - old;
  }
  break;

  case WindowEventType::MousePressed:
  {
    Vec2 old = input->mouse_position;
    Vec2 now = {(f32)event.mouse_pressed.x, (f32)event.mouse_pressed.y};

    input->mouse_position = now;
    input->mouse_delta += now - old;

    u32 index = (u32)event.mouse_pressed.button;

    if (!input->mouse_down[index]) input->mouse_pressed[index] = true;
    input->mouse_down[index] = true;
  }
  break;

  case WindowEventType::MouseReleased:
  {
    Vec2 old = input->mouse_position;
    Vec2 now = {(f32)event.mouse_released.x, (f32)event.mouse_released.y};

    input->mouse_position = now;
    input->mouse_delta += now - old;

    u32 index = (u32)event.mouse_released.button;

    input->mouse_released[index] = true;
    input->mouse_down[index]     = false;
  }
  break;

  case WindowEventType::MouseWheel:
  {
    input->mouse_wheel +=
        event.mouse_wheel.y; // TODO: Too lazy to make this into a Vec2 now, do later!
  }
  break;

  case WindowEventType::FocusLost:
  {
    memset(input->key_down, 0, sizeof(input->key_down));
    memset(input->mouse_down, 0, sizeof(input->mouse_down));
  }
  break;

  default:
    break;
  }
}