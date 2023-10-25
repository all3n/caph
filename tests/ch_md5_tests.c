#include "ch_md5.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc, char *argv[]) {
  if (argc != 2) {
    exit(-1);
  }
  const char *buffer = argv[1];
  char out[65] = {0};
  ChHashMD5(buffer, strlen(buffer), out);
  ChHashMD5(buffer, strlen(buffer), out + 32);
  printf("%s\n", out);

  ch_str_t md5 = ChHashMd5CStr(buffer, strlen(buffer));
  printf_cstr(md5);
  ch_str_free(&md5);
  return EXIT_SUCCESS;
}
