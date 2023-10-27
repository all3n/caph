#include "ch_path.h"
#include <dirent.h>
#include <stdlib.h>
#include <string.h>

#ifdef __unix__
#include <pwd.h>
#endif
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

char *ch_get_user_path(const char *sub_path) {
  char *home_dir = getenv("HOME"); // Linux 和 Mac OS X
  if (home_dir == NULL) {
    home_dir = getenv("USERPROFILE"); // Windows
  }
  if (home_dir == NULL) {
    return NULL;
  }
  char *path = (char *)malloc(strlen(home_dir) + strlen(sub_path) + 1);
  strcpy(path, home_dir);
  strcat(path, sub_path);
  return path;
}

int ch_is_dir(const char *path) {
  if (path == NULL) {
    return 0;
  }
  struct stat path_stat;
  if (stat(path, &path_stat) == 0) {
    if (S_ISDIR(path_stat.st_mode)) {
      return 1;
    } else {
      return 0;
    }
  }
  return 0;
}

char *ch_expanduser(const char *path) {
  if (path == NULL)
    return NULL;

  if (path[0] == '~') {
    const char *home = getenv("HOME");

#ifdef __unix__
    if (home == NULL) {
      struct passwd *pw = getpwuid(getuid());
      if (pw != NULL)
        home = pw->pw_dir;
    }
#endif
    if (home != NULL) {
      size_t len = strlen(home) + strlen(path) - 1;
      char *expanded_path = (char *)malloc(len + 1);
      sprintf(expanded_path, "%s%s", home, path + 1);
      return expanded_path;
    }
  }
  return strdup(path);
}

char *ch_get_file_no_ext(const char *path) {
  const char *p = strrchr(path, CH_PATH_SEP);
  p = p ? p + 1 : path;
  char *dp = strrchr(p, '.');
  if (dp) {
    return strndup(p, dp - p);
  } else {
    return strdup(p);
  }
}

int ch_loop_dir(const char *dir, ch_path_callback callback, void *user_data) {
  DIR *d = opendir(dir);
  if (d == NULL) {
    return -1;
  }
  struct dirent *de;
  size_t dir_len = strlen(dir);
  int need_append_slash = dir[dir_len - 1] != CH_PATH_SEP ? 1 : 0;
  char sep[2] = {CH_PATH_SEP};
  while ((de = readdir(d)) != NULL) {
    if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0) {
      continue;
    }
    size_t len = dir_len + strlen(de->d_name) + (need_append_slash ? 2 : 1);
    char *sub_path = (char *)malloc(len);
    strcpy(sub_path, dir);
    if (need_append_slash) {
      strcat(sub_path, sep);
    }
    strcat(sub_path, de->d_name);
    sub_path[len - 1] = '\0';
    if (de->d_type == DT_REG) {
      printf("name:%s type:%d\n", de->d_name, de->d_type);
      if (callback(sub_path, de->d_name, de->d_type, user_data) != 0) {
        free(sub_path);
        break;
      }
    } else if (de->d_type == DT_DIR) {
      if (ch_loop_dir(sub_path, callback, user_data) != 0) {
        free(sub_path);
        break;
      }
    }
    free(sub_path);
  }
  closedir(d);
  return 0;
}
