#include <stdio.h>
#include <stdlib.h>
#ifndef T
#define T int
#endif

// #ifndef _CH_ENV_T_H
// #define _CH_ENV_T_H
#include "ch_macro.h"
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
// T Base
#define TB CH_JOIN(CH_JOIN(CH_API_PREFIX, list), T)
// Node
#define TN CH_JOIN(TB, node)
// iterator
#define TI CH_JOIN(TB, it)

typedef struct TN {
  T value;
  struct TN *next;
  struct TN *prev;
} TN;

typedef struct TB {
  void (*free_item)(T *);
  T (*copy_item)(T *);
  struct TN *head;
  struct TN *tail;
  size_t size;
  void (*print_item)(T *);
  void (*push_back)(struct TB *self, T value);
  void (*free)(struct TB *self);
  void (*print)(struct TB *self);
} TB;

typedef struct TI {
  void (*step)(struct TI *);
  T *ref;
  TN *begin;
  TN *node;
  TN *next;
  TN *end;
  int done;
} TI;

static void CH_JOIN(TB, push_back)(struct TB *self, T value);
static void CH_JOIN(TB, free)(TB *self);
static void CH_JOIN(TB, print)(TB *self);

static inline TB CH_JOIN(TB, init)(void) {
  static TB zero = {.push_back = CH_JOIN(TB, push_back),
                    .free = CH_JOIN(TB, free),
                    .print = CH_JOIN(TB, print)};
  TB self = zero;
  // self.copy_item = CH_JOIN(TB, copy);
  return self;
}

static inline struct TN *CH_JOIN(TN, init)(T value) {
  struct TN *node = (struct TN *)malloc(sizeof(struct TN));
  node->value = value;
  node->next = NULL;
  node->prev = NULL;
  return node;
}

// ch_list_T_front
static inline T *CH_JOIN(TB, front)(TB *self) { return &self->head->value; }
// ch_list_T_back
static inline T *CH_JOIN(TB, back)(TB *self) { return &self->tail->value; }
// ch_list_T_begin
static inline TN *CH_JOIN(TB, begin)(TB *self) { return self->head; }
// ch_list_T_end
static inline TN *CH_JOIN(TB, end)(TB *self) {
  (void)self;
  return NULL;
}

// ch_list_T_it_step
static inline void CH_JOIN(TI, step)(TI *self) {
  if (self->next == self->end)
    self->done = 1;
  else {
    self->node = self->next;
    self->ref = &self->node->value;
    self->next = self->node->next;
  }
}
static inline int CH_JOIN(TB, empty)(TB *self) { return self->size == 0; }

static inline void CH_JOIN(TB, swap)(TB *self, TB *other) {
  TB temp = *self;
  *self = *other;
  *other = temp;
}

static inline void CH_JOIN(TB, connect)(TB *self, TN *position, TN *node,
                                        int before) {
  if (CH_JOIN(TB, empty)(self)) {
    self->head = self->tail = node;
  } else {
    if (before) {
      node->next = position;
      node->prev = position->prev;
      if (position->prev)
        position->prev->next = node;
      position->prev = node;
      if (position == self->head)
        self->head = node;
    } else {
      node->prev = position;
      node->next = position->next;
      if (position->next)
        position->next->prev = node;
      position->next = node;
      if (position == self->tail)
        self->tail = node;
    }
  }
  self->size += 1;
}

static inline void CH_JOIN(TB, push_back)(TB *self, T value) {
  TN *node = CH_JOIN(TN, init)(value);
  // insert after
  CH_JOIN(TB, connect)(self, self->tail, node, 0);
}

static inline void CH_JOIN(TB, push_front)(TB *self, T value) {
  TN *node = CH_JOIN(TN, init)(value);
  // insert before
  CH_JOIN(TB, connect)(self, self->head, node, 1);
}
static inline void CH_JOIN(TB, disconnect)(TB *self, TN *node) {
  if (node == self->tail)
    self->tail = self->tail->prev;
  if (node == self->head)
    self->head = self->head->next;
  if (node->prev)
    node->prev->next = node->next;
  if (node->next)
    node->next->prev = node->prev;
  node->prev = node->next = NULL;
  self->size -= 1;
}

static inline void CH_JOIN(TB, erase)(TB *self, TN *node) {
  CH_JOIN(TB, disconnect)(self, node);
  if (self->free_item)
    self->free_item(&node->value);
  free(node);
}

static inline void CH_JOIN(TB, pop_back)(TB *self) {
  CH_JOIN(TB, erase)(self, self->tail);
}

static inline void CH_JOIN(TB, pop_front)(TB *self) {
  CH_JOIN(TB, erase)(self, self->head);
}

static inline void CH_JOIN(TB, insert)(TB *self, TN *position, T value) {
  TN *node = CH_JOIN(TN, init)(value);
  CH_JOIN(TB, connect)(self, position, node, 1);
}

static inline void CH_JOIN(TB, clear)(TB *self) {
  while (!CH_JOIN(TB, empty)(self))
    CH_JOIN(TB, pop_back)(self);
}

static inline void CH_JOIN(TB, free)(TB *self) {
  CH_JOIN(TB, clear)(self);
  *self = CH_JOIN(TB, init)();
}

static inline void CH_JOIN(TB, print)(TB *self) {
  TN *node = self->head;
  while (node) {
    if (self->print_item) {
      self->print_item(&node->value);
    }else{
      printf("%p", &node->value);
    }
    node = node->next;
    if (node) {
      printf("->");
    }
  }
  printf("\n");
}
#define CH_FOREACH(X, LOOP)                                                    \
  CH_JOIN(CH_JOIN(CH_JOIN(CH_API_PREFIX, list), X), node) *node = self->head;  \
  while (node) {                                                               \
    LOOP;                                                                      \
    node = node->next;                                                         \
  }

#undef T
#undef TB
#undef TN
#undef TI

#ifdef __cplusplus
}
#endif

// #endif
