#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#ifndef BASE_H
# include "base.h"
#endif

#ifdef BASE_IMPLEMENTATION

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
#endif