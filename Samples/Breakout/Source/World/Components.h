#pragma once

#include "GameObject.h"

#include <Atreus/Math.h>

class Transform : public Component
{
public:
  Transform(Vec2 position, Vec2 size) : position(position), previous_position(position), size(size)
  {
  }

  void update(const ApplicationContext &) override
  {
    previous_position = position;
  }

  Vec2 interpolated(f32 alpha) const
  {
    return previous_position + (position - previous_position) * alpha;
  }

  Vec2 position; // bottom left
  Vec2 previous_position;
  Vec2 size;
};

class QuadRenderer : public Component
{
public:
  QuadRenderer(Color color) : color(color)
  {
  }

  void start() override
  {
    transform = owner->get_component<Transform>();
    ATR_ASSERT(transform != nullptr, "QuadRenderer requires a Transform");
  }

  void render(Renderer2D &renderer, f32 alpha) override
  {
    draw_quad(&renderer, transform->interpolated(alpha), transform->size, color);
  }

  Color      color;
  Transform *transform = nullptr;
};

class PaddleController : public Component
{
public:
  PaddleController(f32 speed) : speed(speed)
  {
  }

  void start() override
  {
    transform = owner->get_component<Transform>();
    ATR_ASSERT(transform != nullptr, "PaddleController requires a Transform");
  }

  void update(const ApplicationContext &context) override
  {
    f32 direction = 0.0f;

    if (input_key_down(context.input, KeyCode::A)) direction -= 1.0f;
    if (input_key_down(context.input, KeyCode::D)) direction += 1.0f;

    transform->position.x += direction * speed * context.delta_time;

    u32 width, height;
    window_get_client_size(context.window, &width, &height);

    f32 max_x = (f32)width - transform->size.x;
    if (transform->position.x < 0.0f) transform->position.x = 0.0f;
    if (transform->position.x > max_x) transform->position.x = max_x;
  }

  f32        speed;
  Transform *transform = nullptr;
};