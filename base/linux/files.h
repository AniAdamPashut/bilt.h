#ifndef BASE_H
# include "../base.h"
#endif


#ifdef PLATFORM_LINUX

#include <unistd.h>
#include <fcntl.h>
#include <errno.h> 
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <linux/limits.h>

String GetCwd() {
  char *cwd = malloc(PATH_MAX);
  if (getcwd(cwd, MAX_PATH) == NULL) {
    LogError("Wasn't able to call getcwd, %d", errno);
    abort();
  }
  return s(cwd);
}

void SetCwd(String destination) {
  chdir(destination.data);
}

Folder *GetDirFiles(String initial) {
  struct dirent *entry;
  DIR *dp = opendir(initial.data);

  Folder *folder = NewFolder();

  if (dp == NULL) {
    LogError("Couldn't open dir %s", initial.data);
    abort();
  }

  while ((entry = readdir(dp))) {
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        continue;

    char fullpath[4096];
    snprintf(fullpath, sizeof(fullpath), "%s/%s", initial.data, entry->d_name); 
    
    struct stat sb;
    if (stat(fullpath, &sb) == -1) {
      LogWarn("Couldn't stat file %s", fullpath);
      continue;
    }
    
    File *currFile = &folder->files[folder->fileCount];
    Folder *currFolder = &folder->folders[folder->folderCount];

    if (S_ISDIR(sb.st_mode)) {
      currFolder->name = s(strdup(entry->d_name));
      folder->folderCount++;
    } else if (S_ISREG(sb.st_mode)) {
      char *dot = strrchr(entry->d_name, '.');
      const char *ext = (dot && dot != entry->d_name) ? dot + 1 : "";
      
      currFile->name = strdup(entry->d_name);
      currFile->extension = strdup(ext);
      currFile->size = sb.st_size;
      currFile->modifyTime = sb.st_mtime;

      folder->fileCount++;
    }

    folder->totalCount++;
  }

  closedir(dp);

  return folder;
}

errno_t FileStats(String *path, File *file) {
  struct stat sb;
  
  char* pathCstr = strdup(path->data);
  
  if (stat(pathCstr, &sb) == -1) {
    LogError("Couldn't stat file %s", pathCstr);
    return FILE_GET_ATTRIBUTES_FAILED;
  }
  
  if (!S_ISREG(sb.st_mode)) {
    LogError("%s is not a regular file", pathCstr);
    return 1;
  }

  char *dot = strrchr(pathCstr, '.');
  const char *ext = (dot && dot != pathCstr) ? dot + 1 : "";
  
  file->name = strdup(pathCstr);
  file->extension = strdup(ext);
  file->size = sb.st_size;
  file->modifyTime = sb.st_mtime;

  return SUCCESS;
}

errno_t FileRead(Arena *arena, String *path, String *result) {
  FILE *file;
  char buffer[1024];
  
  file = fopen(path->data, "r");
  if (file == NULL) {
    if (errno == ENOENT)
      return FILE_NOT_EXIST;
    LogError("Failed to open file: %s", path->data);
    return FILE_OPEN_FAILED;
  }
  ssize_t charsRead = fread(buffer, sizeof(buffer), sizeof(char), file);
  *result = StrNew(arena, buffer);

  while (fread(buffer, sizeof(buffer), sizeof(char), file) == 1024) {
    String stringedBuffer = s(buffer);
    *result = StrConcat(arena, result, &stringedBuffer);
  }

  fclose(file);
  return SUCCESS;
}


errno_t FileWrite(String *path, String *data) {
  // using open and write instead of fopen and fwrite cause it broke ninja.
  // if you are smarter than me (or me in the future) and have a solve please suggest i
  int fd = open(path->data, O_WRONLY | O_CREAT | O_TRUNC, 0644);

  if (fd == -1) {
    LogError("Couldn't open file %s", path->data);
    return FILE_OPEN_FAILED;
  }

  if (write(fd, data->data, data->length) != data->length)
    return 1;  
  return SUCCESS;
}

errno_t FileDelete(String *path) {
  if (remove(path->data) == 0) {
    LogWarn("Deleting file %s", path->data);
    return SUCCESS;
  }
  LogError("Couldn't delete file %s", path->data);
  return 1;
}

errno_t FileRename(String *oldPath, String *newPath) {
  if (rename(oldPath->data, newPath->data) == 0) {
    LogWarn("Renamed file %s to %s", oldPath->data, newPath->data);
    return SUCCESS;
  }
  LogError("Couldn't rename file %s to %s", oldPath->data, newPath->data);
  return 1;
}

bool Mkdir(String path) {
  struct stat st = {0};

  if (stat(path.data, &st) == -1) {
    if (mkdir(path.data, 0755) != 0) {
      LogError("Error creating directory %s", path.data);
      return false;
    }
    LogInfo("Created directory %s", path.data);
    return true;
  }

  return true;
}

#endif