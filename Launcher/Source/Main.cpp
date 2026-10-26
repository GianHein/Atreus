#include <Atreus/Graphics/Renderer2D.h>
#include <Atreus/Log.h>
#include <Atreus/Platform/File.h>
#include <Atreus/Platform/System.h>
#include <Atreus/Platform/Window.h>
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
  i64        unix_now = TimeUnixMilliseconds();
  char       log_file_name[32];
  FileHandle log_file;

  sprintf(log_file_name, "%lld.log", unix_now);
  FileOpen(log_file_name, FileAccessMode::Write, FileOpenMode::OpenAlways, &log_file);
  LogAddSink(FileLogSink, &log_file);

  WindowDesc window_desc;
  window_desc.title  = "Atreus";
  window_desc.width  = 1920;
  window_desc.height = 1080;

  Window     *window   = WindowCreate(window_desc);
  Renderer2D *renderer = CreateRenderer2D(window);

  bool        quit = false;
  WindowEvent event;

  while (!quit)
  {
    WindowPumpMessages();

    while (WindowPollEvent(window, &event))
    {
      if (event.type == WindowEventType::CloseRequested ||
          (event.type == WindowEventType::KeyPressed && event.key_pressed.key == KeyCode::Escape))
      {
        quit = true;
      }

      if (event.type == WindowEventType::KeyPressed && event.key_pressed.key == KeyCode::P)
      {
        ATR_LOG_INFO("Logging");
      }
    }

    BeginFrame2D(renderer);

    Clear(renderer, {1.0f, 0.0f, 1.0f, 1.0f});

    EndFrame2D(renderer);
  }

  DestroyRenderer2D(renderer);
  WindowDestroy(window);
  LogRemoveSink(FileLogSink, &log_file);
  FileClose(&log_file);
  return 0;
}