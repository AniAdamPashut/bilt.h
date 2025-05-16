#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "str.h"

typedef struct {
  String name;
  char *extension;
  i64 size;
  i64 modifyTime;
} File;

typedef struct folder_t {
  String name;

  struct folder_t *folders;
  size_t folderCount;

  File *files;
  size_t fileCount;

  size_t totalCount;
} Folder;

enum FileError { 
  FILE_NOT_EXIST = 1, 
  FILE_OPEN_FAILED, 
  FILE_GET_SIZE_FAILED, 
  FILE_READ_FAILED,
  FILE_GET_ATTRIBUTES_FAILED,
  FILE_WRITE_FAILED,
  FILE_DELETE_FAILED,
  FILE_RENAME_FAILED
};

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

#define MAX_FILES 200

/* File Implementation */
Folder *NewFolder() {
  Folder *fileData = (Folder *)malloc(sizeof(Folder));
  fileData->files = (File *)malloc(MAX_FILES * sizeof(File));
  fileData->fileCount = 0;
  fileData->folders = (Folder *)malloc(MAX_FILES * sizeof(Folder));
  fileData->folderCount = 0;
  fileData->totalCount = 0;
  fileData->name = S("");
  return fileData;
};

void _freeFolderRecursiveImpl(Folder *folder){ 
  free(folder->files);
  folder->totalCount -= folder->fileCount;

  for (size_t i = 0; i < folder->folderCount; i++) {
    Folder *subfolder = folder->folders + i;
    _freeFolderRecursiveImpl(subfolder);
    folder->totalCount--;
  }
  free(folder->folders);
  
  assert(folder->totalCount == 0 && "Should free every item in folder");
}


void FreeFolder(Folder *folder) {
  _freeFolderRecursiveImpl(folder);
  free(folder);
}


#ifdef PLATFORM_WIN
# include "windows/files.h"
#endif
#ifdef PLATFORM_LINUX
# include "linux/files.h"
#endif

#endif