#ifndef _ch_hash_map_MAP_H
#define _ch_hash_map_MAP_H
#include "ch_macro.h"
#include <stdbool.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif
typedef void *(*ch_malloc_fn)(size_t);
typedef void (*ch_free_fn)(void *);
typedef void *(*ch_realloc_fn)(void *, size_t);
typedef uint64_t (*ch_hash_fn)(const void *, const size_t len, uint32_t seed);

struct ch_hash_map_op {
  ch_hash_fn hash;
  int (*cmp)(const void *, const void *);
  // memmory alloc and free
  ch_malloc_fn malloc;
  ch_realloc_fn realloc;
  ch_free_fn free;
  void (*elfree)(void *item);
};
/*
 hash conflict strategy
 open addressing:
    1. linear probing
    2. quadratic probing
    3. double hashing
 chained hashing:
    1. chained hashing with linked list
*/
enum ch_hash_map_type {
  CH_HASH_MAP_LINEAR_PROBING = 0,
  CH_HASH_MAP_DOUBLE_HASHING = 1,
  CH_HASH_MAP_QUADRATIC_PROBING = 2,
  CH_HASH_MAP_CHAINING = 3
};

struct ch_hash_map_data {
  union {
    void *value;
    int32_t iVal;
    int64_t lVal;
    float fVal;
    double dVal;
  };
};
// chain
struct ch_hash_map_link_node {
  char *key;
  struct ch_hash_map_data data;
  struct ch_hash_map_link_node *next;
};
struct ch_hash_map_bucket {
  uint64_t hash;
  union {
    struct ch_hash_map_link_node *head;
  };
  union {
    int iVal;
    void *value;
  };
};
struct ch_hash_map {
  enum ch_hash_map_type type;
  struct ch_hash_map_op *op;
  size_t elsize;
  size_t capacity;
  uint64_t seed0;
  uint64_t seed1;
  void *udata;
  struct ch_hash_map_bucket *buckets;
  size_t bucketsz;
  size_t nbuckets;

  // size_t count;
  // size_t mask;
  // size_t growat;
  // size_t shrinkat;
  // uint8_t growpower;
  // bool oom;
  // void *buckets;
  // void *spare;
  // void *edata;
};

CH_CAPI_EXPORT void ch_hash_map_init(struct ch_hash_map *hm);
CH_CAPI_EXPORT void ch_hash_map_set(struct ch_hash_map *hm, void *key,
                                    void *value);
CH_CAPI_EXPORT void ch_hash_map_delete(struct ch_hash_map *hm, void *key);
CH_CAPI_EXPORT const void *ch_hash_map_get(struct ch_hash_map *hm, void *key);
CH_CAPI_EXPORT void ch_hash_map_count(struct ch_hash_map *hm);
CH_CAPI_EXPORT void ch_hash_map_free(struct ch_hash_map *hm);
CH_CAPI_EXPORT bool
ch_hash_map_scan(struct ch_hash_map *hm,
                 bool (*iter)(const void *item, void *udata), void *udata);
CH_CAPI_EXPORT bool ch_hash_map_iter(struct ch_hash_map *hm, size_t *i,
                                     void **item);

CH_CAPI_EXPORT void ch_hash_alloc_debug(struct ch_hash_map *hm);
CH_CAPI_EXPORT void ch_hash_map_alloc_debug_zero();

#define CH_CREATE_HASH_MAP(x, T, ...)                                          \
  struct ch_hash_map x = {.elsize = sizeof(T), __VA_ARGS__};                   \
  ch_hash_map_init(&x)

#define CH_NEW_HASH_MAP(x, T, ...)                                             \
  x = malloc(sizeof(struct ch_hash_map));                                      \
  memset(x, 0, sizeof(struct ch_hash_map));                                    \
  x->elsize = sizeof(T);                                                       \
  ch_hash_map_init(x)

#ifdef __cplusplus
}
#endif

#endif
