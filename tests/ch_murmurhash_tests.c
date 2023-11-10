#include "ch_murmurhash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc, char *argv[])
{
  uint64_t hash = ch_murmurhash3("admin", strlen("admin"), 123);
  printf("%llu\n", hash);
  return EXIT_SUCCESS;
}
