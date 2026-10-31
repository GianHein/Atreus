#include <Atreus/Core/Input.h>
#include <Atreus/Graphics/Renderer2D.h>
#include <Atreus/Log.h>
#include <Atreus/Platform/File.h>
#include <Atreus/Platform/System.h>
#include <Atreus/Platform/Window.h>
#include <Atreus/Profiling.h>
#include <cstdio>
#include <cstring>

void FileLogSink(void *user, LogLevel level, const char *message)
{
  FileHandle *log_file = static_cast<FileHandle *>(user);

  if (log_file->handle != nullptr)
  {
    char     buffer[2048];
    DateTime date_time = TimeGetLocalDate();

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
        LogLevelName(level),
        message
    );

    FileWrite(*log_file, FileSeekOrigin::End, 0, buffer, strlen(buffer), nullptr);
  }
}

int main(int argc, char const *argv[])

{
  ATR_PROFILE_SCOPE();
  i64        unix_now = TimeUnixMilliseconds();
  char       executable_directory[512];
  char       log_directory[576];
  char       log_path[640];
  FileHandle log_file = INVALID_FILE_HANDLE;

  if (!PathGetExecutable(executable_directory, sizeof(executable_directory)) ||
      !PathGetDirectory(executable_directory, executable_directory, sizeof(executable_directory)))
    strcpy(executable_directory, ".");

  snprintf(log_directory, sizeof(log_directory), "%s/Logs", executable_directory);
  snprintf(log_path, sizeof(log_path), "%s/%lld.log", log_directory, unix_now);

  if (DirectoryCreate(log_directory) &&
      FileOpen(log_path, FileAccessMode::Write, FileOpenMode::OpenAlways, &log_file))
  {
    LogAddSink(FileLogSink, &log_file);
    ATR_LOG_INFO("Logging to %s", log_path);
  }

  WindowDesc window_desc;
  window_desc.title  = "Atreus";
  window_desc.width  = 1920;
  window_desc.height = 1080;

  Window *window = WindowCreate(window_desc);
  if (!window)
  {
    if (log_file.handle) FileClose(&log_file);
    return 1;
  }

  Renderer2D *renderer = CreateRenderer2D(window);
  if (!renderer)
  {
    if (log_file.handle) FileClose(&log_file);
    return 1;
  }

  bool        quit = false;
  WindowEvent event;

  InputState input;
  InputInit(&input);

  f64       total_time = 0.0;
  const f64 delta_time = 0.01;

  f64 current_time = TimeNowSeconds();
  f64 accumulator  = 0.0;

  f32           rect_x     = 100;
  f32           rect_y     = 100;
  constexpr f32 rect_speed = 500.0f;

  while (!quit)
  {
    f64 now        = TimeNowSeconds();
    f64 frame_time = now - current_time;

    if (frame_time >= 0.25) frame_time = 0.25;
    current_time = now;

    accumulator += frame_time;

    WindowPumpMessages();

    while (WindowPollEvent(window, &event))
    {
      InputHandleEvent(&input, event);

      if (event.type == WindowEventType::CloseRequested || InputKeyDown(&input, KeyCode::Escape))
      {
        quit = true;
      }
    }

    f32 prev_rect_x = rect_x;
    f32 prev_rect_y = rect_y;

    while (accumulator >= delta_time)
    {
      // simulate(delta_time);
      // ATR_LOG_TRACE("Simulated 1 frame worth of time");

      if (InputKeyDown(&input, KeyCode::W)) rect_y += rect_speed * delta_time;
      if (InputKeyDown(&input, KeyCode::S)) rect_y -= rect_speed * delta_time;
      if (InputKeyDown(&input, KeyCode::A)) rect_x -= rect_speed * delta_time;
      if (InputKeyDown(&input, KeyCode::D)) rect_x += rect_speed * delta_time;

      total_time += delta_time;
      accumulator -= delta_time;
    }

    const f64 alpha = accumulator / delta_time;
    // render_state = current_state * alpha + previous_state * (1.0 - alpha);

    BeginFrame2D(renderer);

    Clear(renderer, {0, 0, 0, 1});
    DrawQuad(
        renderer,
        {rect_x * (f32)alpha + prev_rect_x * (1.0f - (f32)alpha),
         rect_y * (f32)alpha + prev_rect_y * (1.0f - (f32)alpha)},
        {100, 50},
        {1, 1, 1, 1}
    );

    EndFrame2D(renderer);
  }

  DestroyRenderer2D(renderer);
  WindowDestroy(window);
  LogRemoveSink(FileLogSink, &log_file);
  if (log_file.handle) FileClose(&log_file);
  return 0;
}