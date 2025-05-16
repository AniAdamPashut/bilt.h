#ifndef BASE_H
#define BASE_H

#if defined(__clang__)
#define COMPILER_CLANG
#elif defined(_MSC_VER)
#define COMPILER_MSVC
#elif defined(__GNUC__)
#define COMPILER_GCC
#else
#error "The codebase only supports Clang, MSVC and GCC, TCC soon"
#endif

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#define PLATFORM_WIN
#elif defined(__linux__) || defined(__gnu_linux__)
#define PLATFORM_LINUX
#else
#error "The codebase only supports windows and linux, macos soon"
#endif

#ifdef COMPILER_CLANG
#define FILE_NAME __FILE_NAME__
#else
#define FILE_NAME __FILE__
#endif

#ifdef COMPILER_CLANG
#define FORMAT_CHECK(fmt_pos, args_pos) __attribute__((format(printf, fmt_pos, args_pos))) // NOTE: Printf like warnings on format
#else
#define FORMAT_CHECK(fmt_pos, args_pos)
#endif

#if defined(__STDC_VERSION__)
#if (__STDC_VERSION__ >= 202311L)
#define C_STANDARD_C23
#define C_STANDARD "C23"
#elif (__STDC_VERSION__ >= 201710L)
#define C_STANDARD_C17
#define C_STANDARD "C17"
#elif (__STDC_VERSION__ >= 201112L)
#define C_STANDARD_C11
#define C_STANDARD "C11"
#elif (__STDC_VERSION__ >= 199901L)
#define C_STANDARD_C99
#define C_STANDARD "C99"
#else
#error "Current C standard is unsupported" // ???
#endif
#endif

#if defined(COMPILER_MSVC)
#if _MSVC_LANG >= 202000L
#define C_STANDARD_C23
#define C_STANDARD "C23"
#elif _MSVC_LANG >= 201704L
#define C_STANDARD_C17
#define C_STANDARD "C17"
#elif _MSVC_LANG >= 201103L
#define C_STANDARD_C11
#define C_STANDARD "C11"
#else
#error "Current C standard is unsupported" // ???
#endif
#endif

#ifdef PLATFORM_WIN
/* Process functions */
#define popen _popen
#define pclose _pclose

/* File I/O functions */
#define fdopen _fdopen
#define access _access
#define unlink _unlink
#define isatty _isatty
#define dup _dup
#define dup2 _dup2
#define ftruncate _chsize
#define fsync _commit

/* Directory functions */
#define mkdir(path, mode) _mkdir(path)
#define rmdir _rmdir
#define getcwd _getcwd
#define chdir _chdir

/* Process/Threading */
#define getpid _getpid
#define execvp _execvp
#define execve _execve
#define sleep(x) Sleep((x) * 1000)
#define usleep(x) Sleep((x) / 1000)

/* String functions */
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#define strdup _strdup

/* File modes */
#define R_OK 4
#define W_OK 2
#define X_OK 0 /* Windows doesn't have explicit X_OK */
#define F_OK 0

/* File descriptors */
#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

/* Some functions need complete replacements */
#ifdef COMPILER_MSVC
#define snprintf _snprintf
#define vsnprintf _vsnprintf
#endif

#endif

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

/* --- Types and MACRO types --- */
// Unsigned int types
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

// Signed int types
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

// Regular int types
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

// Floating point types
typedef float f32;
typedef double f64;

/* --- Errors --- */
typedef i32 errno_t;

enum GeneralError {
  SUCCESS,
  MEMORY_ALLOCATION_FAILED,
};

/* --- Time and Platforms --- */
i64 TimeNow();
void WaitTime(i64 ms);


/* --- Defer Macros --- */
#ifdef DEFER_MACRO // NOTE: Optional since not all compilers support it and not all C versions do either

/* - GCC implementation -
  NOTE: Must use C23 (depending on the platform)
*/
#ifdef COMPILER_GCC
#define defer __DEFER(__COUNTER__)
#define __DEFER(N) __DEFER_(N)
#define __DEFER_(N) __DEFER__(__DEFER_FUNCTION_##N, __DEFER_VARIABLE_##N)
#define __DEFER__(F, V)                                                                                                                                                                                                                        \
  auto void FormatMalloc(int *);                                                                                                                                                                                                                          \
  [[gnu::cleanup(FormatMalloc)]] int V;                                                                                                                                                                                                                   \
  auto void FormatMalloc(int *)

/* - Clang implementation -
  NOTE: Must compile with flag `-fblocks`
*/
#elif defined(COMPILER_CLANG)
typedef void (^const __df_t)(void);

[[maybe_unused]]
static inline void __df_cb(__df_t *__fp) {
  (*__fp)();
}

#define defer __DEFER(__COUNTER__)
#define __DEFER(N) __DEFER_(N)
#define __DEFER_(N) __DEFER__(__DEFER_VARIABLE_##N)
#define __DEFER__(V) [[gnu::cleanup(__df_cb)]] __df_t V = ^void(void)

/* -- MSVC implementation --
  NOTE: Not available yet in MSVC, use `_try/_finally`
*/
#elif defined(COMPILER_MSVC)
#error "Not available yet in MSVC, use `_try/_finally`"
#endif

#endif

#ifdef BASE_IMPLEMENTATION

#include "fs.h"
#include "log.h"
#include "random.h"
#include "strings.h"
#include "vectors.h"

#ifdef PLATFORM_WIN
# include "windows/times.h"
#endif
#ifdef PLATFORM_LINUX
# include "linux/times.h"
#endif

// --- Platform specific functions ---
#if !defined(PLATFORM_WIN) && !defined(C_STANDARD_C11)
#ifndef EINVAL
#define EINVAL 22 // NOTE: Invalid argument
#endif
#ifndef ERANGE
#define ERANGE 34 // NOTE: Result too large
#endif

#endif

String GetCompiler() {
#if defined(COMPILER_CLANG)
  return S("clang");
#elif defined(COMPILER_GCC)
  return S("gcc");
#elif defined(COMPILER_MSVC)
  return S("MSVC");
#else
  return S("unknown");
#endif
}

String GetPlatform() {
#if defined(PLATFORM_WIN)
  return S("windows");
#elif defined(PLATFORM_LINUX)
  return S("linux");
#else
  return S("unknown");
#endif
}

#endif

#endif
