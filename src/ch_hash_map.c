#include "ch_hash_map.h"
#include "ch_murmurhash.h"
#include <assert.h>
#include <string.h>

#define CH_HASH_MAP_DEFAULT_CAPACITY 10
static uint64_t hash_map_murmur(const void * p, const size_t size, uint32_t seed){
  uint64_t out[2];
  MurmurHash3_x64_128(p, size, seed, out);
  return out[0];
}

static struct ch_hash_map_op ch_hash_map_default_ops = {.malloc = malloc,
                                                        .free = free,
                                                        .realloc = realloc,
                                                        .elfree = NULL,
                                                        .cmp = NULL,
                                                        .hash = hash_map_murmur};

static int64_t g_hash_map_alloc;
static void *x_malloc(size_t size) {
  void *ret = malloc(size);
  g_hash_map_alloc += 1;
  return ret;
}
static void x_free(void *ptr) {
  free(ptr);
  g_hash_map_alloc--;
}
void ch_hash_alloc_debug(struct ch_hash_map *hm) {
  hm->op->malloc = x_malloc;
  hm->op->free = x_free;
}
void ch_hash_map_alloc_debug_zero(){
  assert(g_hash_map_alloc == 0);
}

void ch_hash_map_init(struct ch_hash_map *map) {
  ch_malloc_fn malloc_fn = ch_hash_map_default_ops.malloc;
  if (map->op && map->op->malloc) {
    malloc_fn = map->op->malloc;
  }
  if (map->op == NULL) {
    map->op = malloc_fn(sizeof(struct ch_hash_map_op));
    memcpy(map->op, &ch_hash_map_default_ops, sizeof(struct ch_hash_map_op));
  }
  if (map->capacity == 0) {
    map->capacity = CH_HASH_MAP_DEFAULT_CAPACITY;
  }
  map->elsize = map->elsize ? map->elsize : sizeof(void *);
  map->bucketsz = sizeof(struct ch_hash_map_bucket);
  map->nbuckets = 0;
  map->buckets = malloc_fn(map->bucketsz * map->capacity);
  memset(map->buckets, 0, map->bucketsz * map->capacity);
}
void ch_hash_map_set(struct ch_hash_map *hm, void *key, void *value) {
  uint64_t hash = hm->op->hash(key, hm->elsize, hm->seed0);



}
void ch_hash_map_delete(struct ch_hash_map *hm, void *key) {}
const void *ch_hash_map_get(struct ch_hash_map *hm, void *key) { return NULL; }
void ch_hash_map_count(struct ch_hash_map *hm) {}
void ch_hash_map_free(struct ch_hash_map *hm) {
  ch_free_fn free_fn = ch_hash_map_default_ops.free;
  if (hm->op) {
    if (hm->op->free) {
      free_fn = hm->op->free;
    }
    if (hm->op->elfree) {
      for (int i = 0; i < hm->capacity; i++) {
        if (hm->type == CH_HASH_MAP_CHAINING) {
          struct ch_hash_map_link_node *head = hm->buckets[i].head;
          while (head != NULL) {
            hm->op->elfree(head->data.value);
            head = head->next;
          }
        } else {
          hm->op->elfree(&hm->buckets[i].value);
        }
      }
    }
    free_fn(hm->buckets);
  }
  free_fn(hm->op);
}
bool ch_hash_map_scan(struct ch_hash_map *hm,
                      bool (*iter)(const void *item, void *udata),
                      void *udata) {
  return false;
}
bool ch_hash_map_iter(struct ch_hash_map *hm, size_t *i, void **item) {
  return false;
}
