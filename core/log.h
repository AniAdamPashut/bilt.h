#ifndef LOG_H
#define LOG_H

#include "base.h"

#define RESET "\x1b[0m"
#define GRAY "\x1b[38;2;192;192;192m"
#define RED "\x1b[0;31m"
#define GREEN "\x1b[0;32m"
#define ORANGE "\x1b[0;33m"

void LogInfo(const char *format, ...) FORMAT_CHECK(1, 2);
void LogWarn(const char *format, ...) FORMAT_CHECK(1, 2);
void LogError(const char *format, ...) FORMAT_CHECK(1, 2);
void LogSuccess(const char *format, ...) FORMAT_CHECK(1, 2);
void LogInit();

void LogInfo(const char *format, ...) {
  printf("%s[INFO]: ", GRAY);
  va_list args;
  va_start(args, format);
  vprintf(format, args);
  va_end(args);
  printf("%s\n", RESET);
}

void LogWarn(const char *format, ...) {
  printf("%s[WARN]: ", ORANGE);
  va_list args;
  va_start(args, format);
  vprintf(format, args);
  va_end(args);
  printf("%s\n", RESET);
}

void LogError(const char *format, ...) {
  printf("%s[ERROR]: ", RED);
  va_list args;
  va_start(args, format);
  vprintf(format, args);
  va_end(args);
  printf("%s\n", RESET);
}

void LogSuccess(const char *format, ...) {
  printf("%s[SUCCESS]: ", GREEN);
  va_list args;
  va_start(args, format);
  vprintf(format, args);
  va_end(args);
  printf("%s\n", RESET);
}

void LogInit() {
#ifdef PLATFORM_WIN
# define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
  HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
  DWORD dwMode = 0;
  GetConsoleMode(hOut, &dwMode);
  dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
  SetConsoleMode(hOut, dwMode);
#endif
}

#endif