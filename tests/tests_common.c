#include "tests_common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#define TEST_PATH_SEP '\\'
#define TEST_PATH_SEP_STR "\\"
#else
#define TEST_PATH_SEP '/'
#define TEST_PATH_SEP_STR "/"
#endif

void test() { printf("Hello World\n"); }

char *test_data(const char *bin, const char *data_path) {
  char *slash = strrchr(bin, TEST_PATH_SEP);
  if (slash == NULL) {
    return NULL;
  }
  size_t dir_len = slash - bin;
  size_t path_len = dir_len + strlen(data_path) + 2;
  char *path = malloc(path_len);
  memset(path, 0, path_len);
  strncpy(path, bin, dir_len);
  printf("%s\n", path);
  memcpy(path+dir_len, TEST_PATH_SEP_STR, 2);
  printf("%s\n", path);
  strcat(path, data_path);
  printf("%s\n", path);
  path[path_len - 1] = '\0';
  return path;
}
