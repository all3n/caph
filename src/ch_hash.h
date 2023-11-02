#ifndef _CH_HASH_H
#define _CH_HASH_H

#include "hashmap.h"
#ifdef __cplusplus
extern "C" {
#endif
#define HASH_STR(TYPE, field)                                                  \
  static uint64_t hash_str_##field(const void *item, uint64_t seed0,           \
                                   uint64_t seed1) {                           \
    TYPE *p = (TYPE *)item;                                                    \
    return hashmap_xxhash3(p->field, strlen(p->field), seed0, seed1);          \
  }

#define HASH_COMPARE_STR(TYPE, field)                                               \
  static int hash_compare_str_##field(const void *a, const void *b, void *udata) {  \
    TYPE *item_a = (TYPE *)a;                                                  \
    TYPE *item_b = (TYPE *)b;                                                  \
    return strcmp(item_a->field, item_b->field);                               \
  }

#define CH_HASH(TYPE, field, seed)                                                   \
  hashmap_new(sizeof(TYPE), 0, seed, seed, hash_str_##field,                   \
              hash_compare_str_##field, NULL, NULL)


#ifdef __cplusplus
}
#endif

#endif
