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

#include "vectors.h"

/* --- Platform Specific --- */
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

typedef struct {
  size_t length; // Does not include null terminator
  char *data;
} String;

#define TYPE_INIT(type) (type)

/* --- Errors --- */
typedef i32 errno_t;

enum GeneralError {
  SUCCESS,
  MEMORY_ALLOCATION_FAILED,
};

typedef struct {
  String name;
  char *extension;
  int64_t size;
  int64_t modifyTime;
} File;

typedef struct folder_t {
  String name;

  struct folder_t *folders;
  size_t folderCount;

  File *files;
  size_t fileCount;

  size_t totalCount;
} Folder;

enum FileReadError { 
  FILE_NOT_EXIST = 1, 
  FILE_OPEN_FAILED, 
  FILE_GET_SIZE_FAILED, 
  FILE_READ_FAILED,
  FILE_GET_ATTRIBUTES_FAILED,
  FILE_WRITE_FAILED,
  FILE_DELETE_FAILED,
  FILE_RENAME_FAILED
};

/* --- String and Macros --- */
#define STRING_LENGTH(s) ((sizeof(s) / sizeof((s)[0])) - sizeof((s)[0])) // NOTE: Inspired from clay.h
#define ENSURE_STRING_LITERAL(x) ("" x "")

// NOTE: If an error led you here, it's because `S` can only be used with string literals, i.e. `S("SomeString")` and not `S(yourString)` - for that use `s()`
#define S(string) (TYPE_INIT(String){.length = STRING_LENGTH(ENSURE_STRING_LITERAL(string)), .data = (string)})

#ifdef COMPILER_CLANG
#define FORMAT_CHECK(fmt_pos, args_pos) __attribute__((format(printf, fmt_pos, args_pos))) // NOTE: Printf like warnings on format
#else
#define FORMAT_CHECK(fmt_pos, args_pos)
#endif

String s(char *msg);

String FormatMalloc(const char *format, ...) FORMAT_CHECK(2, 3);

VEC_TYPE(StringVector, String);

#define StringVectorPushMany(vector, ...)                                                                                                                                                                                                      \
  ({                                                                                                                                                                                                                                           \
    char *values[] = {__VA_ARGS__};                                                                                                                                                                                                            \
    size_t count = sizeof(values) / sizeof(values[0]);                                                                                                                                                                                         \
    for (size_t i = 0; i < count; i++) {                                                                                                                                                                                                       \
      VecPush(vector, s(values[i]));                                                                                                                                                                                                           \
    }                                                                                                                                                                                                                                          \
  })

void SetMaxStrSize(size_t size);
String StrNew(char *str);
String StrNewSize(char *str, size_t len); // Without null terminator
void StrCopy(String *destination, String *source);
StringVector StrSplit(String *string, String *delimiter);
bool StrEqual(String string1, String string2);
String StrConcat(String *string1, String *string2);
void StrToUpper(String *string1);
void StrToLower(String *string1);
bool StrIsNull(String *string);
void StrTrim(String *string);
void StrFree(String string);
String StrSlice(String *str, i32 start, i32 end);
String ConvertExe(String path);
String ConvertPath(String path);

String GetCwd();
void SetCwd(String destination);
Folder *GetDirFiles(String initial);
Folder *NewFolder();
void FreeFolder(Folder *folder);
errno_t FileStats(String *path, File *file);
errno_t FileRead(String *path, String *result);
errno_t FileWrite(String *path, String *data);
errno_t FileDelete(String *path);
errno_t FileRename(String *oldPath, String *newPath);
bool Mkdir(String path);

errno_t memcpy_s(void *dest, size_t destSize, const void *src, size_t count);
inline errno_t fopen_s(FILE **streamptr, const char *filename, const char *mode);

/* --- Time and Platforms --- */
i64 TimeNow();
void WaitTime(i64 ms);
String GetCompiler();
String GetPlatform();

/* --- Random --- */
void RandomInit(); // NOTE: Must init before using
u64 RandomGetSeed();
void RandomSetSeed(u64 newSeed);
i32 RandomInteger(i32 min, i32 max);
f32 RandomFloat(f32 min, f32 max);

/* --- Logger --- */
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

#include "strings.h"
#include "fs.h"
#include "log.h"
#include "random.h"
#include "hashset.h"

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

errno_t memcpy_s(void *dest, size_t destSize, const void *src, size_t count) {
  if (dest == NULL) {
    return EINVAL;
  }

  if (src == NULL || destSize < count) {
    memset(dest, 0, destSize);
    return EINVAL;
  }

  memcpy(dest, src, count);
  return 0;
}

inline errno_t fopen_s(FILE **streamptr, const char *filename, const char *mode) {
  if (streamptr == NULL) {
    return EINVAL;
  }

  *streamptr = NULL;
  if (filename == NULL || mode == NULL) {
    return EINVAL;
  }

  if (*filename == '\0') {
    return EINVAL;
  }

  const char *valid_modes = "rwa+btx";
  size_t mode_len = strlen(mode);

  if (mode_len == 0) {
    return EINVAL;
  }

  for (size_t i = 0; i < mode_len; i++) {
    if (strchr(valid_modes, mode[i]) == NULL) {
      return EINVAL;
    }
  }

  *streamptr = fopen(filename, mode);
  if (*streamptr == NULL) {
    return errno;
  }

  return 0;
}
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
