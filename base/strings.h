#ifndef STRINGS_H
#define STRINGS_H

#ifndef BASE_H
# include "base.h"
#endif

#ifdef BASE_IMPLEMENTATION

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

String StrNewSize(char *str, size_t len) {
  const size_t memorySize = sizeof(char) * len + 1; // NOTE: Includes null terminator
  char *allocatedString = (char *)malloc(memorySize);

  memcpy(allocatedString, str, memorySize);
  addNullTerminator(allocatedString, len);
  return (String){len, allocatedString};
}

String StrNew(char *str) {
  const size_t len = strLength(str, maxStringSize);
  if (len == 0) {
    return (String){0, NULL};
  }
  const size_t memorySize = sizeof(char) * len + 1; // NOTE: Includes null terminator
  char *allocatedString = (char *)malloc(memorySize);

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

String StrConcat(String *string1, String *string2) {
  assert(!StrIsNull(string1) && "string1 should never be NULL");
  assert(!StrIsNull(string2) && "string2 should never be NULL");

  const size_t len = string1->length + string2->length;
  const size_t memorySize = sizeof(char) * len + 1; // NOTE: Includes null terminator
  char *allocatedString = (char *)malloc(memorySize);

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

bool StrEqual(String string1, String string2) {
  if (string1.length != string2.length) {
    return false;
  }

  if (memcmp(string1.data, string2.data, string1.length) != 0) {
    return false;
  }
  return true;
}

StringVector StrSplit(String *str, String *delimiter) {
  assert(!StrIsNull(str) && "str should never be NULL");
  assert(!StrIsNull(delimiter) && "delimiter should never be NULL");

  char *start = str->data;
  const char *end = str->data + str->length;
  char *curr = start;
  StringVector result = {0};
  if (delimiter->length == 0) {
    for (size_t i = 0; i < str->length; i++) {
      String currString = StrNewSize(str->data + i, 1);
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
      String currString = StrNewSize(curr, end - curr);
      VecPush(result, currString);
      break;
    }

    size_t len = match - curr;
    String currString = StrNewSize(curr, len);
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

String StrSlice(String *str, i32 start, i32 end) {
  assert(start >= 0 && "start index must be non-negative");
  assert(start <= str->length && "start index out of bounds");

  if (end < 0) {
    end = str->length + end;
  }

  assert(end >= start && "end must be greater than or equal to start");
  assert(end <= str->length && "end index out of bounds");

  size_t len = end - start;
  return StrNewSize(str->data + start, len);
}

void StrFree(String string) {
  free(string.data);
}

String FormatMalloc(const char *format, ...) {
  va_list args;
  va_start(args, format);
  size_t size = vsnprintf(NULL, 0, format, args) + 1; // +1 for null terminator
  va_end(args);

  char *buffer = (char *)malloc(size);
  va_start(args, format);
  vsnprintf(buffer, size, format, args);
  va_end(args);

  return (String){.length = size - 1, .data = buffer};
}

String ConvertPath(String path) {
  String platform = GetPlatform();
  String result;

  if (path.length >= 2 && path.data[0] == '.' && (path.data[1] == '/' || path.data[1] == '\\')) {
    result = StrNewSize(path.data + 2, path.length - 2);
    memcpy(result.data, path.data + 2, path.length - 2);
  } else {
    result = StrNewSize(path.data, path.length);
  }

  if (StrEqual(platform, S("linux")) || StrEqual(platform, S("macos"))) {
    return result;
  }

  for (size_t i = 0; i < result.length; i++) {
    if (result.data[i] == '/') {
      result.data[i] = '\\';
    }
  }

  return result;
}

String ParsePath(String path) {
  String result;

  if (path.length >= 2 && path.data[0] == '.' && (path.data[1] == '/' || path.data[1] == '\\')) {
    result = StrNewSize(path.data + 2, path.length - 2);
    memcpy(result.data, path.data + 2, path.length - 2);
  } else {
    result = StrNewSize(path.data, path.length);
  }

  return result;
}

String ConvertExe(String path) {
  String platform = GetPlatform();
  String exeExtension = S(".exe");

  bool hasExe = false;
  if (path.length >= exeExtension.length) {
    String pathEnd = StrSlice(&path, path.length - exeExtension.length, path.length);
    if (StrEqual(pathEnd, exeExtension)) {
      hasExe = true;
    }
  }

  if (StrEqual(platform, S("windows"))) {
    if (hasExe) {
      return path;
    }
    return StrConcat(&path, &exeExtension);
  }

  if (StrEqual(platform, S("linux")) || StrEqual(platform, S("macos"))) {
    if (hasExe) {
      return StrSlice(&path, 0, path.length - exeExtension.length);
    }
    return path;
  }

  return path;
}

#endif

#endif