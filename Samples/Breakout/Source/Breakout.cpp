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

static void Init(ApplicationContext *context)
{
  BreakoutState *state = (BreakoutState *)context->state;

  state->paddle = {100, 100};
}

static void Shutdown(ApplicationContext *context)
{
}

static void Update(ApplicationContext *context)
{
  BreakoutState *state = (BreakoutState *)context->state;

  state->paddle_previous = state->paddle;

  if (InputKeyDown(context->input, KeyCode::W))
    state->paddle.y += PADDLE_SPEED * context->delta_time;

  if (InputKeyDown(context->input, KeyCode::S))
    state->paddle.y -= PADDLE_SPEED * context->delta_time;

  if (InputKeyDown(context->input, KeyCode::A))
    state->paddle.x -= PADDLE_SPEED * context->delta_time;

  if (InputKeyDown(context->input, KeyCode::D))
    state->paddle.x += PADDLE_SPEED * context->delta_time;

  if (InputKeyPressed(context->input, KeyCode::Escape)) context->quit_requested = true;
}

static void Render(ApplicationContext *context)
{
  BreakoutState *state = (BreakoutState *)context->state;

  Clear(context->renderer, {0, 0, 0, 1});
  DrawQuad(
      context->renderer,
      {state->paddle.x * (f32)context->alpha +
           state->paddle_previous.x * (1.0f - (f32)context->alpha),
       state->paddle.y * (f32)context->alpha +
           state->paddle_previous.y * (1.0f - (f32)context->alpha)},
      {100, 50},
      {1, 1, 1, 1}
  );
}

ATR_MODULE_EXPORT const ApplicationAPI *AtreusGetApplicationApi()
{
  static const ApplicationAPI api = {
      ATR_APPLICATION_API_VERSION, sizeof(BreakoutState), Init, Shutdown, Update, Render
  };
  return &api;
}