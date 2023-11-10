#include "ch_hash_map.h"
int main(int argc, char *argv[]) {
  CH_CREATE_HASH_MAP(x, int);
  ch_hash_alloc_debug(&x);
  ch_hash_map_set(&x, "afd", &(int[1]){1});
  ch_hash_map_set(&x, "afd", &(int[1]){2});
  ch_hash_map_alloc_debug_zero();
  return EXIT_SUCCESS;
}
