#include "ch_path.h"
#include "ch_string.h"
#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <dirent.h>
#include <pwd.h>
#include <unistd.h>
#endif

char *ch_get_home_path(){
  char *home_dir = getenv("HOME"); // Linux 和 Mac OS X
  if (home_dir == NULL) {
    home_dir = getenv("USERPROFILE"); // Windows
  }
  if (home_dir == NULL) {
    return NULL;
  }
  return home_dir;
}
char *ch_get_user_path(const char *sub_path) {
  char * home_dir = ch_get_home_path();
  if (home_dir == NULL) {
    return NULL;
  }
  char *path = (char *)malloc(strlen(home_dir) + strlen(sub_path) + 1);
  strcpy(path, home_dir);
  strcat(path, sub_path);
  return path;
}

void ch_convert_dot_to_underscore(char *str) {
  size_t len = strlen(str);
  for (size_t i = 0; i < len; i++) {
    if (isalpha(str[i])) {
      str[i] = toupper(str[i]);
    } else if (str[i] == '.') {
      str[i] = '_';
    } else if (str[i] == '/') {
      str[i] = '_';
    } else if (str[i] == '-') {
      str[i] = '_';
    }
  }
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
char *ch_get_file_name(const char *path) {
  const char *p = strrchr(path, CH_PATH_SEP);
  p = p ? p + 1 : path;
  return strdup(p);
}

ch_str_t ch_path_join(const char *base, ...) {
  va_list ap;
  size_t blen = 0;
  if (base) {
    blen = strlen(base);
  }
  const char *arg;
  // callc  total len  start
  size_t len = blen;
  va_start(ap, base);
  while ((arg = va_arg(ap, const char *)) != NULL) {
    if (len > 0) {
      len += strlen(arg) + 1;
    } else {
      len += strlen(arg);
    }
  }
  va_end(ap);
  // callc total len  end
  ch_str_t out = ch_str_new(len + 1);
  if (base) {
    strcpy(out.str, base);
  }
  out.len = blen;
  // join left
  va_start(ap, base);
  size_t len2;
  while ((arg = va_arg(ap, const char *)) != NULL) {
    // len > 0 && str[len - 1] == CH_PATH_SEP
    if (out.len && out.str[out.len - 1] != CH_PATH_SEP) {
      strcat(out.str + out.len, CH_PATH_SEP_STR);
      out.len += 1;
    }
    len2 = strlen(arg);
    memcpy(out.str + out.len, arg, len2);
    out.len += len2;
  }
  va_end(ap);
  out.len = len;
  out.str[out.len] = '\0';
  return out;
}

#ifdef _WIN32
// TODO not supported
int ch_loop_dir(const char *root, const char *sub_path,
                ch_path_callback callback, void *user_data) {

  return 0;
}
#else
int ch_loop_dir(const char *root, const char *sub_path,
                ch_path_callback callback, void *user_data) {
  ch_str_t abs_path = ch_cstr(root);
  DIR *d = opendir(abs_path.str);
  if (d == NULL) {
    ch_str_free(&abs_path);
    return -1;
  }
  struct dirent *de;
  int ret = 0;
  while ((de = readdir(d)) != NULL) {
    if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0) {
      continue;
    }
    ch_str_t d_str = ch_path_join(abs_path.str, de->d_name, NULL);
    ch_str_t d_path_str = ch_cstr(de->d_name);
    if (sub_path) {
      d_path_str = ch_path_join(sub_path, de->d_name, NULL);
    }
    if (de->d_type == DT_REG) {
      if (callback(d_str.str, d_path_str.str, de->d_name, de->d_type,
                   user_data) != 0) {
        ch_str_free(&d_str);
        ch_str_free(&d_path_str);
        ret = -2;
        break;
      }
    } else if (de->d_type == DT_DIR) {
      printf("dir:%s sub_path:%s\n", d_str.str, d_path_str.str);
      if (ch_loop_dir(d_str.str, d_path_str.str, callback, user_data) != 0) {
        ch_str_free(&d_str);
        ch_str_free(&d_path_str);
        ret = -3;
        break;
      }
    }
    ch_str_free(&d_str);
    ch_str_free(&d_path_str);
  }
  closedir(d);
  ch_str_free(&abs_path);
  printf("flag:%d\n", ret);
  return ret;
}
#endif
