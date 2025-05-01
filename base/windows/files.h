#ifndef BASE_H
# include "../base.h"
#endif


#ifdef PLATFORM_WIN

#include <windows.h>

char currentPath[MAX_PATH];
char *GetCwd() {
  DWORD length = GetCurrentDirectory(MAX_PATH, currentPath);
  if (length == 0) {
    printf("Error getting current directory: %lu\n", GetLastError());
    return "";
  }
  return currentPath;
}
void SetCwd(char *destination) {
  bool result = SetCurrentDirectory(destination);
  if (!result) {
    printf("Error setting cwd: %lu\n", GetLastError());
  }
  GetCwd();
}

FileData *GetDirFiles() {
  WIN32_FIND_DATA findData;
  HANDLE hFind;
  char searchPath[MAX_PATH];
  FileData *fileData = NewFileData();
  i32 result = snprintf(searchPath, MAX_PATH - 2, "%s\\*", currentPath);
  assert(result >= 0 && "sprint should not return error");

  hFind = FindFirstFile(searchPath, &findData);
  if (hFind == INVALID_HANDLE_VALUE) {
    printf("Error finding files: %lu\n", GetLastError());
    return NULL;
  }

  do {
    if (strcmp(findData.cFileName, ".") == 0 || strcmp(findData.cFileName, "..") == 0) {
      continue;
    }

    if (fileData->totalCount >= MAX_FILES) {
      printf("Warning: Maximum file count reached (%d). Some files might be skipped.\n", MAX_FILES);
      break;
    }

    bool isDirectory = findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
    File *currFile = &fileData->files[fileData->fileCount];
    Folder *currFolder = &fileData->folders[fileData->folderCount];

    if (isDirectory) {
      currFolder->name = strdup(findData.cFileName);
      fileData->folderCount++;
    }

    if (!isDirectory) {
      char *dot = strrchr(findData.cFileName, '.');
      if (dot != NULL) {
        currFile->extension = strdup(dot + 1);

        size_t baseNameLength = dot - findData.cFileName;
        char *baseName = (char *)malloc(baseNameLength + 1);
        memcpy(baseName, findData.cFileName, baseNameLength);
        baseName[baseNameLength] = '\0';
        currFile->name = baseName;
      }

      if (dot == NULL) {
        currFile->extension = strdup("");
        currFile->name = strdup(findData.cFileName);
      }

      LARGE_INTEGER createTime, modifyTime;
      createTime.LowPart = findData.ftCreationTime.dwLowDateTime;
      createTime.HighPart = findData.ftCreationTime.dwHighDateTime;
      modifyTime.LowPart = findData.ftLastWriteTime.dwLowDateTime;
      modifyTime.HighPart = findData.ftLastWriteTime.dwHighDateTime;

      const i64 WINDOWS_TICK = 10000000;
      const i64 SEC_TO_UNIX_EPOCH = 11644473600LL;

      currFile->createTime = createTime.QuadPart / WINDOWS_TICK - SEC_TO_UNIX_EPOCH;
      currFile->modifyTime = modifyTime.QuadPart / WINDOWS_TICK - SEC_TO_UNIX_EPOCH;
      currFile->size = (((i64)findData.nFileSizeHigh) << 32) | findData.nFileSizeLow;
      fileData->fileCount++;
    }

    fileData->totalCount++;
  } while (FindNextFile(hFind, &findData) != 0);

  DWORD dwError = GetLastError();
  if (dwError != ERROR_NO_MORE_FILES) {
    printf("Error searching for files: %lu\n", dwError);
  }

  FindClose(hFind);
  return fileData;
}

void FreeFileData(FileData *fileData) {
  if (fileData->files == NULL && fileData->folders == NULL) return;

  for (size_t i = 0; i < fileData->fileCount; i++) {
    File currentFile = fileData->files[i];
    free(currentFile.name);
    free(currentFile.extension);
  }

  free(fileData->files);

  for (size_t i = 0; i < fileData->folderCount; i++) {
    Folder currentFolder = fileData->folders[i];
    free(currentFolder.name);
  }

  free(fileData->folders);
  free(fileData);
}

errno_t FileStats(String *path, File *result) {
  WIN32_FILE_ATTRIBUTE_DATA fileAttr = {0};

  char *pathStr = malloc(path->length + 1);
  if (!pathStr) {
    LogError("Memory allocation failed");
    return MEMORY_ALLOCATION_FAILED;
  }

  memcpy(pathStr, path->data, path->length);
  pathStr[path->length] = '\0';

  if (!GetFileAttributesExA(pathStr, GetFileExInfoStandard, &fileAttr)) {
    LogError("Failed to get file attributes: %lu", GetLastError());
    free(pathStr);
    return FILE_GET_ATTRIBUTES_FAILED;
  }

  char *nameStart = strrchr(pathStr, '\\');
  if (!nameStart) {
    nameStart = strrchr(pathStr, '/');
  }

  if (nameStart) {
    nameStart++;
  } else {
    nameStart = pathStr;
  }

  result->name = strdup(nameStart);

  char *extStart = strrchr(nameStart, '.');
  if (extStart) {
    result->extension = strdup(extStart + 1);
  } else {
    result->extension = strdup("");
  }

  LARGE_INTEGER fileSize;
  fileSize.HighPart = fileAttr.nFileSizeHigh;
  fileSize.LowPart = fileAttr.nFileSizeLow;
  result->size = fileSize.QuadPart;

  LARGE_INTEGER createTime, modifyTime;
  createTime.LowPart = fileAttr.ftCreationTime.dwLowDateTime;
  createTime.HighPart = fileAttr.ftCreationTime.dwHighDateTime;
  modifyTime.LowPart = fileAttr.ftLastWriteTime.dwLowDateTime;
  modifyTime.HighPart = fileAttr.ftLastWriteTime.dwHighDateTime;

  const i64 WINDOWS_TICK = 10000000;
  const i64 SEC_TO_UNIX_EPOCH = 11644473600LL;

  result->createTime = createTime.QuadPart / WINDOWS_TICK - SEC_TO_UNIX_EPOCH;
  result->modifyTime = modifyTime.QuadPart / WINDOWS_TICK - SEC_TO_UNIX_EPOCH;

  free(pathStr);
  return SUCCESS;
}

errno_t FileRead(Arena *arena, String *path, String *result) {
  HANDLE hFile = INVALID_HANDLE_VALUE;

  char *pathStr = malloc(path->length + 1);
  if (!pathStr) {
    LogError("Memory allocation failed");
    return MEMORY_ALLOCATION_FAILED;
  }

  memcpy(pathStr, path->data, path->length);
  pathStr[path->length] = '\0';
  hFile = CreateFileA(pathStr, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

  if (hFile == INVALID_HANDLE_VALUE) {
    DWORD error = GetLastError();
    free(pathStr);

    if (error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND) {
      return FILE_NOT_EXIST;
    }

    LogError("File open failed, err: %lu", error);
    return FILE_OPEN_FAILED;
  }

  LARGE_INTEGER fileSize;
  if (!GetFileSizeEx(hFile, &fileSize)) {
    LogError("Failed to get file size: %lu\n", GetLastError());
    CloseHandle(hFile);
    free(pathStr);
    return FILE_GET_SIZE_FAILED;
  }

  char *buffer = (char *)ArenaAlloc(arena, fileSize.QuadPart);
  if (!buffer) {
    LogError("Memory allocation failed");
    CloseHandle(hFile);
    free(pathStr);
    return MEMORY_ALLOCATION_FAILED;
  }

  DWORD bytesRead;
  if (!ReadFile(hFile, buffer, (DWORD)fileSize.QuadPart, &bytesRead, NULL) || bytesRead != fileSize.QuadPart) {
    LogError("Failed to read file: %lu", GetLastError());
    CloseHandle(hFile);
    free(pathStr);
    return FILE_READ_FAILED;
  }

  *result = StrNewSize(arena, buffer, (size_t)bytesRead);

  CloseHandle(hFile);
  free(pathStr);
  return SUCCESS;
}

errno_t FileWrite(String *path, String *result) {
  HANDLE hFile = INVALID_HANDLE_VALUE;

  char *pathStr = malloc(path->length + 1);
  if (!pathStr) {
    LogError("Memory allocation failed");
    return ENOMEM;
  }

  memcpy(pathStr, path->data, path->length);
  pathStr[path->length] = '\0';

  hFile = CreateFileA(pathStr, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

  if (hFile == INVALID_HANDLE_VALUE) {
    DWORD error = GetLastError();
    free(pathStr);

    switch (error) {
    case ERROR_ACCESS_DENIED:
      return EACCES;
    case ERROR_NOT_ENOUGH_MEMORY:
    case ERROR_OUTOFMEMORY:
      return ENOMEM;
    case ERROR_FILE_NOT_FOUND:
      return ENOENT;
    default:
      return EIO;
    }
  }

  DWORD bytesWritten;
  if (!WriteFile(hFile, result->data, (DWORD)result->length, &bytesWritten, NULL) || bytesWritten != result->length) {
    DWORD error = GetLastError();
    CloseHandle(hFile);
    free(pathStr);

    switch (error) {
    case ERROR_DISK_FULL:
      return ENOSPC;
    case ERROR_NOT_ENOUGH_MEMORY:
    case ERROR_OUTOFMEMORY:
      return ENOMEM;
    default:
      return EIO;
    }
  }

  CloseHandle(hFile);
  free(pathStr);

  return SUCCESS;
}

errno_t FileDelete(String *path) {
  char *pathStr = malloc(path->length + 1);
  if (!pathStr) {
    LogError("Memory allocation failed");
    return ENOMEM;
  }

  memcpy(pathStr, path->data, path->length);
  pathStr[path->length] = '\0';

  if (!DeleteFileA(pathStr)) {
    DWORD error = GetLastError();
    free(pathStr);
    switch (error) {
    case ERROR_ACCESS_DENIED:
      return EACCES;
    case ERROR_NOT_ENOUGH_MEMORY:
    case ERROR_OUTOFMEMORY:
      return ENOMEM;
    case ERROR_FILE_NOT_FOUND:
      return ENOENT;
    default:
      return EIO;
    }
  }

  free(pathStr);
  return SUCCESS;
}

errno_t FileRename(String *oldPath, String *newPath) {
  if (!MoveFileEx(oldPath->data, newPath->data, MOVEFILE_REPLACE_EXISTING)) {
    return 1;
  }

  return SUCCESS;
}

bool Mkdir(String path) {
  bool result = CreateDirectory(path.data, NULL);
  if (result != false) {
    return true;
  }

  u64 error = GetLastError();
  if (error == ERROR_ALREADY_EXISTS) {
    return true;
  }

  LogError("Error meanwhile Mkdir() %llu", error);
  return false;
}
#endif