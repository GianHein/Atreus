#include "Atreus/Log.h"
#include "Atreus/Math.h"
#include "Atreus/Runtime/Application.h"

constexpr u32 ROWS         = 2;
constexpr u32 COLS         = 8;
constexpr f32 PADDLE_SPEED = 600.0f;

struct BreakoutState
{
  Vec2 paddle;
  Vec2 paddle_previous;
  Vec2 ball;
  Vec2 ball_velocity;
  bool bricks[ROWS][COLS];
};

static void init(ApplicationContext *context)
{
  BreakoutState *state = (BreakoutState *)context->state;

  state->paddle = {100, 100};
}

static void shutdown(ApplicationContext *context)
{
}

static void update(ApplicationContext *context)
{
  BreakoutState *state = (BreakoutState *)context->state;

  state->paddle_previous = state->paddle;

  if (input_key_down(context->input, KeyCode::W))
    state->paddle.y += PADDLE_SPEED * context->delta_time;

  if (input_key_down(context->input, KeyCode::S))
    state->paddle.y -= PADDLE_SPEED * context->delta_time;

  if (input_key_down(context->input, KeyCode::A))
    state->paddle.x -= PADDLE_SPEED * context->delta_time;

  if (input_key_down(context->input, KeyCode::D))
    state->paddle.x += PADDLE_SPEED * context->delta_time;

  if (input_key_pressed(context->input, KeyCode::Escape)) context->quit_requested = true;
}

static void render(ApplicationContext *context)
{
  BreakoutState *state = (BreakoutState *)context->state;

  clear(context->renderer, {0, 0, 0, 1});
  draw_quad(
      context->renderer,
      {state->paddle.x * (f32)context->alpha +
           state->paddle_previous.x * (1.0f - (f32)context->alpha),
       state->paddle.y * (f32)context->alpha +
           state->paddle_previous.y * (1.0f - (f32)context->alpha)},
      {100, 50},
      {1, 1, 1, 1}
  );
}

ATR_MODULE_EXPORT const ApplicationAPI *atreus_get_application_api()
{
  static const ApplicationAPI api = {
      ATR_APPLICATION_API_VERSION, sizeof(BreakoutState), init, shutdown, update, render
  };
  return &api;
}