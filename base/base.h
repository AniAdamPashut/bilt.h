/* MIT License

  base.h - Better cross-platform std
  Version - 2025-04-15 (0.1.4):
  https://github.com/TomasBorquez/base.h

  Usage:
    #define BASE_IMPLEMENTATION
    #include "base.h"

  More on the the `README.md`
*/

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

/* --- Platform Specific --- */
#ifdef PLATFORM_WIN
/* Process functions */
#define popen _popen
#define pclose _pclose

/* File I/O functions */
#define fileno _fileno
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

// Maximum values for integer types
#define U8_MAX UINT8_MAX
#define U16_MAX UINT16_MAX
#define U32_MAX UINT32_MAX
#define U64_MAX UINT64_MAX

#define S8_MAX INT8_MAX
#define S8_MIN INT8_MIN
#define S16_MAX INT16_MAX
#define S16_MIN INT16_MIN
#define S32_MAX INT32_MAX
#define S32_MIN INT32_MIN
#define S64_MAX INT64_MAX
#define S64_MIN INT64_MIN

#define I8_MAX INT8_MAX
#define I8_MIN INT8_MIN
#define I16_MAX INT16_MAX
#define I16_MIN INT16_MIN
#define I32_MAX INT32_MAX
#define I32_MIN INT32_MIN
#define I64_MAX INT64_MAX
#define I64_MIN INT64_MIN

#define TYPE_INIT(type) (type)

/* --- Vector Macros --- */
// TODO: Add MSVC like vector macros
#define VEC_TYPE(typeName, valueType)                                                                                                                                                                                                          \
  typedef struct {                                                                                                                                                                                                                             \
    valueType *data;                                                                                                                                                                                                                           \
    i32 length;                                                                                                                                                                                                                                \
    i32 capacity;                                                                                                                                                                                                                              \
  } typeName;

#define VecPush(vector, value)                                                                                                                                                                                                                 \
  ({                                                                                                                                                                                                                                           \
    if (vector.length >= vector.capacity) {                                                                                                                                                                                                    \
      if (vector.capacity == 0) vector.capacity = 128;                                                                                                                                                                                         \
      else vector.capacity *= 2;                                                                                                                                                                                                               \
      vector.data = realloc(vector.data, vector.capacity * sizeof(*vector.data));                                                                                                                                                              \
    }                                                                                                                                                                                                                                          \
    vector.data[vector.length++] = value;                                                                                                                                                                                                      \
    &vector.data[vector.length - 1];                                                                                                                                                                                                           \
  })

#define VecPop(vector)                                                                                                                                                                                                                         \
  ({                                                                                                                                                                                                                                           \
    assert(vector.length > 0 && "Cannot pop from empty vector");                                                                                                                                                                               \
    typeof(vector.data[0]) value = vector.data[vector.length - 1];                                                                                                                                                                             \
    vector.length--;                                                                                                                                                                                                                           \
    &value;                                                                                                                                                                                                                                    \
  })

#define VecShift(vector)                                                                                                                                                                                                                       \
  ({                                                                                                                                                                                                                                           \
    assert(vector.length != 0 && "Length should at least be >= 1");                                                                                                                                                                            \
    typeof(vector.data[0]) value = vector.data[0];                                                                                                                                                                                             \
    memmove(&vector.data[0], &vector.data[1], (vector.length - 1) * sizeof(*vector.data));                                                                                                                                                     \
    vector.length--;                                                                                                                                                                                                                           \
    &value;                                                                                                                                                                                                                                    \
  })

#define VecUnshift(vector, value)                                                                                                                                                                                                              \
  ({                                                                                                                                                                                                                                           \
    if (vector.length >= vector.capacity) {                                                                                                                                                                                                    \
      if (vector.capacity == 0) vector.capacity = 2;                                                                                                                                                                                           \
      else vector.capacity *= 2;                                                                                                                                                                                                               \
      vector.data = realloc(vector.data, vector.capacity * sizeof(*vector.data));                                                                                                                                                              \
    }                                                                                                                                                                                                                                          \
                                                                                                                                                                                                                                               \
    if (vector.length > 0) {                                                                                                                                                                                                                   \
      memmove(&vector.data[1], &vector.data[0], vector.length * sizeof(*vector.data));                                                                                                                                                         \
    }                                                                                                                                                                                                                                          \
                                                                                                                                                                                                                                               \
    vector.data[0] = value;                                                                                                                                                                                                                    \
    vector.length++;                                                                                                                                                                                                                           \
    &value;                                                                                                                                                                                                                                    \
  })

#define VecInsert(vector, value, index)                                                                                                                                                                                                        \
  ({                                                                                                                                                                                                                                           \
    assert(index <= vector.length && "Index out of bounds for insertion");                                                                                                                                                                     \
    if (vector.length >= vector.capacity) {                                                                                                                                                                                                    \
      if (vector.capacity == 0) vector.capacity = 2;                                                                                                                                                                                           \
      else vector.capacity *= 2;                                                                                                                                                                                                               \
      vector.data = realloc(vector.data, vector.capacity * sizeof(*vector.data));                                                                                                                                                              \
    }                                                                                                                                                                                                                                          \
    memmove(&vector.data[index + 1], &vector.data[index], (vector.length - index) * sizeof(*vector.data));                                                                                                                                     \
    vector.data[index] = value;                                                                                                                                                                                                                \
    vector.length++;                                                                                                                                                                                                                           \
    &value;                                                                                                                                                                                                                                    \
  })

#define VecAt(vector, index)                                                                                                                                                                                                                   \
  ({                                                                                                                                                                                                                                           \
    assert(index >= 0 && index < vector.length && "Index out of bounds");                                                                                                                                                                      \
    &vector.data[index];                                                                                                                                                                                                                       \
  })

#define VecFree(vector)                                                                                                                                                                                                                        \
  ({                                                                                                                                                                                                                                           \
    assert(vector.data != NULL && "Vector data should never be NULL");                                                                                                                                                                         \
    free(vector.data);                                                                                                                                                                                                                         \
    vector.data = NULL;                                                                                                                                                                                                                        \
  })

/* --- Time and Platforms --- */
i64 TimeNow();
void WaitTime(i64 ms);
String GetCompiler();
String GetPlatform();

/* --- Errors --- */
typedef i32 errno_t;

enum GeneralError {
  SUCCESS,
  MEMORY_ALLOCATION_FAILED,
};

/* --- Arena --- */
typedef struct {
  int8_t *buffer;
  size_t bufferLength;
  size_t prevOffset;
  size_t currOffset;
} Arena;

// This makes sure right alignment on 86/64 bits
#define DEFAULT_ALIGNMENT (2 * sizeof(void *))

Arena ArenaInit(size_t size);
void *ArenaAlloc(Arena *arena, size_t size);
void ArenaFree(Arena *arena);
void ArenaReset(Arena *arena);

/* --- String and Macros --- */
#define STRING_LENGTH(s) ((sizeof(s) / sizeof((s)[0])) - sizeof((s)[0])) // NOTE: Inspired from clay.h
#define ENSURE_STRING_LITERAL(x) ("" x "")

// NOTE: If an error led you here, it's because `S` can only be used with string literals, i.e. `S("SomeString")` and not `S(yourString)` - for that use `s()`
#define S(string) (TYPE_INIT(String){.length = STRING_LENGTH(ENSURE_STRING_LITERAL(string)), .data = (string)})
String s(char *msg);

#ifdef COMPILER_CLANG
#define FORMAT_CHECK(fmt_pos, args_pos) __attribute__((format(printf, fmt_pos, args_pos))) // NOTE: Printf like warnings on format
#else
#define FORMAT_CHECK(fmt_pos, args_pos)
#endif

String F(Arena *arena, const char *format, ...) FORMAT_CHECK(2, 3);

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
String StrNew(Arena *arena, char *str);
String StrNewSize(Arena *arena, char *str, size_t len); // Without null terminator
void StrCopy(String *destination, String *source);
StringVector StrSplit(Arena *arena, String *string, String *delimiter);
bool StrEqual(String *string1, String *string2);
String StrConcat(Arena *arena, String *string1, String *string2);
void StrToUpper(String *string1);
void StrToLower(String *string1);
bool StrIsNull(String *string);
void StrTrim(String *string);
String StrSlice(Arena *arena, String *str, i32 start, i32 end);
String ConvertExe(Arena *arena, String path);
String ConvertPath(Arena *arena, String path);

/* --- Random --- */
void RandomInit(); // NOTE: Must init before using
u64 RandomGetSeed();
void RandomSetSeed(u64 newSeed);
i32 RandomInteger(i32 min, i32 max);
f32 RandomFloat(f32 min, f32 max);

/* --- File --- */
#define MAX_FILES 200

typedef struct {
  char *name;
  char *extension;
  int64_t size;
  int64_t createTime;
  int64_t modifyTime;
} File;

typedef struct {
  char *name;
} Folder;

typedef struct {
  Folder *folders;
  size_t folderCount;

  File *files;
  size_t fileCount;

  size_t totalCount;
} FileData;

#ifndef MAX_PATH
#define MAX_PATH 260
#endif
extern char currentPath[MAX_PATH];

char *GetCwd();
void SetCwd(char *destination);
FileData *GetDirFiles();
FileData *NewFileData();

enum FileStatsError { FILE_GET_ATTRIBUTES_FAILED = 1 };
errno_t FileStats(String *path, File *file);

enum FileReadError { FILE_NOT_EXIST = 1, FILE_OPEN_FAILED, FILE_GET_SIZE_FAILED, FILE_READ_FAILED };
errno_t FileRead(Arena *arena, String *path, String *result);

// TODO: enum FileWriteError {};
errno_t FileWrite(String *path, String *data);

// TODO: enum FileDeleteError {};
errno_t FileDelete(String *path);

// TODO: enum FileRenameError {};
errno_t FileRename(String *oldPath, String *newPath);

bool Mkdir(String path); // NOTE: Mkdir if not exist

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
  auto void F(int *);                                                                                                                                                                                                                          \
  [[gnu::cleanup(F)]] int V;                                                                                                                                                                                                                   \
  auto void F(int *)

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

/*
  Implementation of base.h
  Version - 2025-04-12 (0.1.1):
  https://github.com/TomasBorquez/base.h
*/
#ifdef BASE_IMPLEMENTATION

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

#ifdef PLATFORM_WIN
# include "windows/times.h"
#endif
#ifdef PLATFORM_LINUX
# include "linux/times.h"
#endif

/* Arena Implemenation */
// https://www.gingerbill.org/article/2019/02/08/memory-allocation-strategies-002/
static intptr_t alignForward(const intptr_t ptr) {
  intptr_t p, a, modulo;

  p = ptr;
  a = (intptr_t)DEFAULT_ALIGNMENT;
  // Same as (p % a) but faster as 'a' is a power of two
  modulo = p & (a - 1);

  if (modulo != 0) {
    // If 'p' address is not aligned, push the address to the
    // next value which is aligned
    p += a - modulo;
  }
  return p;
}

void *ArenaAlloc(Arena *arena, const size_t size) {
  // Align 'currPtr' forward to the specified alignment
  intptr_t currPtr = (intptr_t)arena->buffer + (intptr_t)arena->currOffset;
  intptr_t offset = alignForward(currPtr);
  offset -= (intptr_t)arena->buffer; // Change to relative offset

  if (offset + size > arena->bufferLength) {
    LogError("Arena ran out of space left, bufferLength: %llu", arena->bufferLength);
    return NULL;
  }

  void *ptr = &arena->buffer[offset];
  arena->prevOffset = offset;
  arena->currOffset = offset + size;

  memset(ptr, 0, size);
  return ptr;
}

void ArenaFree(Arena *arena) {
  free(arena->buffer);
}

void ArenaReset(Arena *arena) {
  arena->currOffset = 0;
}

Arena ArenaInit(size_t size) {
  return (Arena){
      .buffer = (i8 *)malloc(size),
      .bufferLength = size,
      .prevOffset = 0,
      .currOffset = 0,
  };
}

/* String Implementation */
static size_t maxStringSize = 10000;

static size_t strLength(char *str, size_t maxSize) {
  if (str == NULL) {
    return 0;
  }

  size_t len = 0;
  while (len < maxSize && str[len] != '\0') {
    len++;
  }

  return len;
}

static void addNullTerminator(char *str, size_t len) {
  str[len] = '\0';
}

bool StrIsNull(String *str) {
  return str == NULL || str->data == NULL;
}

void SetMaxStrSize(size_t size) {
  maxStringSize = size;
}

String StrNewSize(Arena *arena, char *str, size_t len) {
  const size_t memorySize = sizeof(char) * len + 1; // NOTE: Includes null terminator
  char *allocatedString = ArenaAlloc(arena, memorySize);

  memcpy(allocatedString, str, memorySize);
  addNullTerminator(allocatedString, len);
  return (String){len, allocatedString};
}

String StrNew(Arena *arena, char *str) {
  const size_t len = strLength(str, maxStringSize);
  if (len == 0) {
    return (String){0, NULL};
  }
  const size_t memorySize = sizeof(char) * len + 1; // NOTE: Includes null terminator
  char *allocatedString = ArenaAlloc(arena, memorySize);

  memcpy(allocatedString, str, memorySize);
  addNullTerminator(allocatedString, len);
  return (String){len, allocatedString};
}

String s(char *msg) {
  return (String){
      .length = strlen(msg),
      .data = msg,
  };
}

String StrConcat(Arena *arena, String *string1, String *string2) {
  assert(!StrIsNull(string1) && "string1 should never be NULL");
  assert(!StrIsNull(string2) && "string2 should never be NULL");

  const size_t len = string1->length + string2->length;
  const size_t memorySize = sizeof(char) * len + 1; // NOTE: Includes null terminator
  char *allocatedString = ArenaAlloc(arena, memorySize);

  memcpy_s(allocatedString, memorySize, string1->data, string1->length);
  memcpy_s(allocatedString + string1->length, memorySize, string2->data, string2->length);
  addNullTerminator(allocatedString, len);
  return (String){len, allocatedString};
};

void StrCopy(String *destination, String *source) {
  assert(!StrIsNull(destination) && "destination should never be NULL");
  assert(!StrIsNull(source) && "source should never be NULL");
  assert(destination->length >= source->length && "destination length should never smaller than source length");

  const errno_t result = memcpy_s(destination->data, destination->length, source->data, source->length);

  assert(result == 0 && "result should never be anything but 0");
  destination->length = source->length;
  addNullTerminator(destination->data, destination->length);
}

bool StrEqual(String *string1, String *string2) {
  if (string1->length != string2->length) {
    return false;
  }

  if (memcmp(string1->data, string2->data, string1->length) != 0) {
    return false;
  }
  return true;
}

StringVector StrSplit(Arena *arena, String *str, String *delimiter) {
  assert(!StrIsNull(str) && "str should never be NULL");
  assert(!StrIsNull(delimiter) && "delimiter should never be NULL");

  char *start = str->data;
  const char *end = str->data + str->length;
  char *curr = start;
  StringVector result = {0};
  if (delimiter->length == 0) {
    for (size_t i = 0; i < str->length; i++) {
      String currString = StrNewSize(arena, str->data + i, 1);
      VecPush(result, currString);
    }
    return result;
  }

  while (curr < end) {
    char *match = NULL;
    for (char *search = curr; search <= end - delimiter->length; search++) {
      if (memcmp(search, delimiter->data, delimiter->length) == 0) {
        match = search;
        break;
      }
    }

    if (!match) {
      String currString = StrNewSize(arena, curr, end - curr);
      VecPush(result, currString);
      break;
    }

    size_t len = match - curr;
    String currString = StrNewSize(arena, curr, len);
    VecPush(result, currString);

    curr = match + delimiter->length;
  }

  return result;
}

void StringToUpper(String *str) {
  for (int i = 0; i < str->length; ++i) {
    char currChar = str->data[i];
    str->data[i] = toupper(currChar);
  }
}

void StrToLower(String *str) {
  for (int i = 0; i < str->length; ++i) {
    char currChar = str->data[i];
    str->data[i] = tolower(currChar);
  }
}

bool isSpace(char character) {
  return character == ' ' || character == '\n' || character == '\t' || character == '\r';
}

void StrTrim(String *str) {
  char *firstChar = NULL;
  char *lastChar = NULL;
  if (str->length == 0) {
    return;
  }

  if (str->length == 1) {
    if (isSpace(str->data[0])) {
      str->data[0] = '\0';
      str->length = 0;
    }
    return;
  }

  for (int i = 0; i < str->length; ++i) {
    char *currChar = &str->data[i];
    if (isSpace(*currChar)) {
      continue;
    }

    if (firstChar == NULL) {
      firstChar = currChar;
    }
    lastChar = currChar;
  }

  if (firstChar == NULL || lastChar == NULL) {
    str->data[0] = '\0';
    str->length = 0;
    addNullTerminator(str->data, 0);
    return;
  }

  size_t len = (lastChar - firstChar) + 1;
  memcpy_s(str->data, str->length, firstChar, len);
  str->length = len;
  addNullTerminator(str->data, len);
}

String StrSlice(Arena *arena, String *str, i32 start, i32 end) {
  assert(start >= 0 && "start index must be non-negative");
  assert(start <= str->length && "start index out of bounds");

  if (end < 0) {
    end = str->length + end;
  }

  assert(end >= start && "end must be greater than or equal to start");
  assert(end <= str->length && "end index out of bounds");

  size_t len = end - start;
  return StrNewSize(arena, str->data + start, len);
}

String F(Arena *arena, const char *format, ...) {
  va_list args;
  va_start(args, format);
  size_t size = vsnprintf(NULL, 0, format, args) + 1; // +1 for null terminator
  va_end(args);

  char *buffer = (char *)ArenaAlloc(arena, size);
  va_start(args, format);
  vsnprintf(buffer, size, format, args);
  va_end(args);

  return (String){.length = size - 1, .data = buffer};
}

String ConvertPath(Arena *arena, String path) {
  String platform = GetPlatform();
  String result;

  if (path.length >= 2 && path.data[0] == '.' && (path.data[1] == '/' || path.data[1] == '\\')) {
    result = StrNewSize(arena, path.data + 2, path.length - 2);
    memcpy(result.data, path.data + 2, path.length - 2);
  } else {
    result = StrNewSize(arena, path.data, path.length);
  }

  if (StrEqual(&platform, &S("linux")) || StrEqual(&platform, &S("macos"))) {
    return result;
  }

  for (size_t i = 0; i < result.length; i++) {
    if (result.data[i] == '/') {
      result.data[i] = '\\';
    }
  }

  return result;
}

String ParsePath(Arena *arena, String path) {
  String result;

  if (path.length >= 2 && path.data[0] == '.' && (path.data[1] == '/' || path.data[1] == '\\')) {
    result = StrNewSize(arena, path.data + 2, path.length - 2);
    memcpy(result.data, path.data + 2, path.length - 2);
  } else {
    result = StrNewSize(arena, path.data, path.length);
  }

  return result;
}

String ConvertExe(Arena *arena, String path) {
  String platform = GetPlatform();
  String exeExtension = S(".exe");

  bool hasExe = false;
  if (path.length >= exeExtension.length) {
    String pathEnd = StrSlice(arena, &path, path.length - exeExtension.length, path.length);
    if (StrEqual(&pathEnd, &exeExtension)) {
      hasExe = true;
    }
  }

  if (StrEqual(&platform, &S("windows"))) {
    if (hasExe) {
      return path;
    }
    return StrConcat(arena, &path, &exeExtension);
  }

  if (StrEqual(&platform, &S("linux")) || StrEqual(&platform, &S("macos"))) {
    if (hasExe) {
      return StrSlice(arena, &path, 0, path.length - exeExtension.length);
    }
    return path;
  }

  return path;
}

/* Random Implemenation */
static u64 seed = 0;
void RandomInit() {
  seed = TimeNow();
  srand(seed);
}

u64 RandomGetSeed() {
  return seed;
}

void RandomSetSeed(u64 newSeed) {
  seed = newSeed;
}

i32 RandomInteger(i32 min, i32 max) {
  assert(min <= max && "min should always be less than or equal to max");
  assert(max - min <= INT32_MAX - 1 && "range too large");

  i32 range = max - min + 1;

  // Calculate scaling factor to avoid modulo bias
  u32 buckets = RAND_MAX / range;
  u32 limit = buckets * range;

  // Reject numbers that would create bias
  u32 r;
  do {
    r = rand();
  } while (r >= limit);

  return min + (r / buckets);
}

f32 RandomFloat(f32 min, f32 max) {
  assert(min <= max && "min must be less than or equal to max");
  f32 normalized = (f32)rand() / RAND_MAX;
  return min + normalized * (max - min);
}

/* File Implementation */
FileData *NewFileData() {
  FileData *fileData = (FileData *)malloc(sizeof(FileData));
  fileData->files = (File *)malloc(MAX_FILES * sizeof(File));
  fileData->fileCount = 0;
  fileData->folders = (Folder *)malloc(MAX_FILES * sizeof(Folder));
  fileData->folderCount = 0;
  fileData->totalCount = 0;
  return fileData;
};

#ifdef PLATFORM_WIN
# include "windows/files.h"
#endif
#ifdef PLATFORM_LINUX
# include "linux/files.h"
#endif

/* Logger Implemenation */
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
  HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
  DWORD dwMode = 0;
  GetConsoleMode(hOut, &dwMode);
  dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
  SetConsoleMode(hOut, dwMode);
#endif
}
#endif

#endif