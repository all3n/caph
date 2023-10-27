#include "ch_path.h"
#include <stdio.h>
#include <stdlib.h>



int path_callback(const char *path, const char * name, int flags, void *user_data) { 
  printf("%s:%s\n", path, name);
  return 0;
}
int main(int argc, char *argv[]) {
  ch_loop_dir("/usr/local/lib/", path_callback, NULL);
  return EXIT_SUCCESS;
}
