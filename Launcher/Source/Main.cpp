#include <Atreus/Core/Input.h>
#include <Atreus/Graphics/Renderer2D.h>
#include <Atreus/Log.h>
#include <Atreus/Platform/File.h>
#include <Atreus/Platform/System.h>
#include <Atreus/Platform/Window.h>
#include <Atreus/Profiling.h>
#include <Atreus/Runtime/Application.h>
#include <cstdio>
#include <cstring>

void file_log_sink(void *user, LogLevel level, const char *message)
{
  FileHandle *log_file = static_cast<FileHandle *>(user);

  if (log_file->handle != nullptr)
  {
    char     buffer[2048];
    DateTime date_time = time_get_local_date();

    sprintf(
        buffer,
        "[%04u-%02u-%02u %02u:%02u:%02u.%03u][%s]%s\n",
        date_time.year,
        date_time.month,
        date_time.day,
        date_time.hour,
        date_time.minute,
        date_time.second,
        date_time.millisecond,
        log_level_name(level),
        message
    );

    file_write(*log_file, FileSeekOrigin::End, 0, buffer, strlen(buffer), nullptr);
  }
}

constexpr f64 DELTA_TIME = 0.01;

struct Launcher
{
  FileHandle            log_file = INVALID_FILE_HANDLE;
  Library              *app_library;
  const ApplicationAPI *app_api;
  usize                 app_state_size;
  bool                  app_initialized;
  Window               *window;
  Renderer2D           *renderer;
  InputState            input;
  ApplicationContext    app_context;
};

static bool launcher_init(Launcher *launcher, const char *app_name)
{
  // Log file sink

  i64  unix_now = time_unix_milliseconds();
  char executable_directory[512];
  char log_directory[576];
  char log_path[640];

  if (!path_get_executable(executable_directory, sizeof(executable_directory)) ||
      !path_get_directory(executable_directory, executable_directory, sizeof(executable_directory)))
    strcpy(executable_directory, ".");

  snprintf(log_directory, sizeof(log_directory), "%s/Logs", executable_directory);
  snprintf(log_path, sizeof(log_path), "%s/%lld.log", log_directory, unix_now);

  if (directory_create(log_directory) &&
      file_open(log_path, FileAccessMode::Write, FileOpenMode::OpenAlways, &launcher->log_file))
  {
    log_add_sink(file_log_sink, &launcher->log_file);
  }

  // Application

  char app_path[640];
  i32  length = snprintf(
      app_path, sizeof(app_path), "%s/%s%s", executable_directory, app_name, library_extension()
  );

  if (length < 0 || (usize)length >= sizeof(app_path))
  {
    ATR_LOG_FATAL("Application path too long");
    return false;
  }

  launcher->app_library = library_open(app_path);
  if (launcher->app_library == nullptr)
  {
    ATR_LOG_FATAL("Failed to open specified application");
    return false;
  }

  GetApplicationApiFn get_app_api =
      (GetApplicationApiFn)library_symbol(launcher->app_library, ATR_APPLICATION_ENTRY_NAME);

  if (get_app_api == nullptr)
  {
    ATR_LOG_FATAL("Failed to load application entry-point");
    return false;
  }

  launcher->app_api = get_app_api();

  if (launcher->app_api == nullptr)
  {
    ATR_LOG_FATAL("Could not load ApplicationAPI from application");
    return false;
  }

  if (launcher->app_api->api_version != ATR_APPLICATION_API_VERSION)
  {
    ATR_LOG_FATAL(
        "Version mismatch, application was built for incompatible ATR_APPLICATION_API_VERSION: %u, launcher expects: %u",
        launcher->app_api->api_version,
        ATR_APPLICATION_API_VERSION
    );
    return false;
  }

  if (launcher->app_api->update == nullptr || launcher->app_api->render == nullptr)
  {
    ATR_LOG_FATAL(
        "Application must provide Update and Render (Update: %s, Render: %s)",
        launcher->app_api->update ? "ok" : "missing",
        launcher->app_api->render ? "ok" : "missing"
    );
    return false;
  }

  launcher->app_state_size = launcher->app_api->state_size;

  // Window, Renderer, and Input

  WindowDesc window_desc;
  window_desc.title  = "Atreus";
  window_desc.width  = 1920;
  window_desc.height = 1080;

  launcher->window = window_create(window_desc);
  if (!launcher->window) return false;

  launcher->renderer = create_renderer2d(launcher->window);
  if (!launcher->renderer) return false;

  input_init(&launcher->input);

  // Application context

  launcher->app_context.window    = launcher->window;
  launcher->app_context.renderer  = launcher->renderer;
  launcher->app_context.allocator = default_allocator();
  launcher->app_context.input     = &launcher->input;
  launcher->app_context.state     = mem_alloc_zeroed(default_allocator(), launcher->app_state_size);
  launcher->app_context.delta_time     = DELTA_TIME;
  launcher->app_context.alpha          = 0.0f;
  launcher->app_context.quit_requested = false;

  if (!launcher->app_context.state) return false;

  launcher->app_initialized = true;

  if (launcher->app_api->init) launcher->app_api->init(&launcher->app_context);

  return true;
}

static void launcher_run(Launcher *launcher)
{
  bool        quit = false;
  WindowEvent event;

  f64 current_time = time_now_seconds();
  f64 accumulator  = 0.0;

  while (!quit)
  {
    f64 now        = time_now_seconds();
    f64 frame_time = now - current_time;

    if (frame_time >= 0.25) frame_time = 0.25;
    current_time = now;

    accumulator += frame_time;

    window_pump_messages();

    while (window_poll_event(launcher->window, &event))
    {
      input_handle_event(&launcher->input, event);

      if (event.type == WindowEventType::CloseRequested) quit = true;
    }

    while (accumulator >= DELTA_TIME)
    {
      launcher->app_api->update(&launcher->app_context);
      input_clear_transients(&launcher->input);

      accumulator -= DELTA_TIME;
    }

    if (launcher->app_context.quit_requested) quit = true;

    launcher->app_context.alpha = accumulator / DELTA_TIME;
    // render_state = current_state * alpha + previous_state * (1.0 - alpha);

    begin_frame2d(launcher->renderer);

    launcher->app_api->render(&launcher->app_context);

    end_frame2d(launcher->renderer);
  }
}

static void launcher_shutdown(Launcher *launcher)
{
  if (launcher->app_initialized && launcher->app_api->shutdown)
    launcher->app_api->shutdown(&launcher->app_context);

  if (launcher->app_context.state)
    mem_free(default_allocator(), launcher->app_context.state, launcher->app_state_size);

  if (launcher->app_library) library_close(launcher->app_library);
  if (launcher->renderer) destroy_renderer2d(launcher->renderer);
  if (launcher->window) window_destroy(launcher->window);

  if (launcher->log_file != INVALID_FILE_HANDLE)
  {
    log_remove_sink(file_log_sink, &launcher->log_file);
    file_close(&launcher->log_file);
  }
}

int main(int argc, char const *argv[])
{
  printf("Atreus v0.1.0\n");

  if (argc <= 1)
  {
    printf("Usage: %s <application>\n", argv[0]);
    return 1;
  }

  Launcher launcher = {};

  bool initialized_successfully = launcher_init(&launcher, argv[1]);
  if (initialized_successfully) launcher_run(&launcher);
  launcher_shutdown(&launcher);

  return initialized_successfully ? 0 : 1;
}