#ifndef LOG_H
#define LOG_H

#ifndef BASE_H
# include "base.h"
#endif

#ifdef BASE_IMPLEMENTATION

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

#endif