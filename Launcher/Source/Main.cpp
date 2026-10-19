#include <Atreus/Platform/File.h>
#include <Atreus/Platform/System.h>
#include <cstdio>
#include <cstring>

int main(int argc, char const *argv[])
{
  i64        unix_now = TimeUnixMilliseconds();
  char       file_name[32];
  FileHandle file;

  sprintf(file_name, "%lld.log", unix_now);
  FileOpen(file_name, FileAccessMode::Write, FileOpenMode::OpenAlways, &file);

  const char *buffer = "Hello World!";
  FileWrite(file, FileSeekOrigin::End, 0, (void *)buffer, strlen(buffer), nullptr);

  FileClose(&file);
  return 0;
}