#include "World/Components.h"
#include "World/GameObject.h"

#include <Atreus/Log.h>
#include <Atreus/Math.h>
#include <Atreus/Platform/File.h>
#include <Atreus/Platform/System.h>
#include <Atreus/Runtime/Application.h>

constexpr u32 ROWS         = 2;
constexpr u32 COLS         = 8;
constexpr f32 PADDLE_SPEED = 900.0f;

struct BreakoutState
{
  GameObject paddle;
  Vec2       ball;
  Vec2       ball_velocity;
  bool       bricks[ROWS][COLS];
};

static void init(ApplicationContext *context)
{
  BreakoutState *state = (BreakoutState *)context->state;

  state->paddle.init();
  state->paddle.add_component<Transform>({Vec2{590, 40}, Vec2{150, 25}});
  state->paddle.add_component<QuadRenderer>({Color{1, 1, 1, 1}});
  state->paddle.add_component<PaddleController>({PADDLE_SPEED});
  state->paddle.start();

  state->ball = {100, 200};

  ATR_LOG_INFO("Successfully initialized Breakout");
}

static void shutdown(ApplicationContext *context)
{
  BreakoutState *state = (BreakoutState *)context->state;

  state->paddle.shutdown();

  ATR_LOG_INFO("Successfully shut down Breakout");
}

static void update(ApplicationContext *context)
{
  BreakoutState *state = (BreakoutState *)context->state;

  state->paddle.update(*context);

  if (input_key_pressed(context->input, KeyCode::Escape)) context->quit_requested = true;
}

static void render(ApplicationContext *context)
{
  BreakoutState *state    = (BreakoutState *)context->state;
  Renderer2D    *renderer = context->renderer;

  clear(renderer, {0, 0, 0, 1});

  state->paddle.render(*renderer, context->alpha);
}

ATR_MODULE_EXPORT const ApplicationAPI *atreus_get_application_api()
{
  static const ApplicationAPI api = {
      ATR_APPLICATION_API_VERSION, sizeof(BreakoutState), init, shutdown, update, render
  };
  return &api;
}