#include "ch_list.h"
#include "ch_macro.h"
#include <stdio.h>
#include <stdlib.h>
#define T float
#include "ch_list.h"
typedef struct XT {
  int a;
  int b;
} XT;
#define T XT
#include "ch_list.h"

static void print_float(float *f) { printf("%f", *f); }
static void print_int(int *i) { printf("%d", *i); }
static void print_pxt(XT *p) { printf("%d,%d", p->a, p->b); }
int main(int argc, char *argv[]) {
  CH_VAR_LIST(int, l);
  l.print_item = print_int;
  l.push_back(&l, 1);
  l.push_back(&l, 2);
  l.push_back(&l, 3);
  l.print(&l);
  l.free(&l);

  CH_VAR_LIST(float, l2);
  l2.push_back(&l2, 1.0);
  l2.push_back(&l2, 3.0);
  l2.push_back(&l2, 4.0);
  l2.print_item = print_float;
  l2.print(&l2);

  CH_LIST_FOREACH(float, l2, cur) {
    printf("%f", cur->value);
    cur = cur->next;
    if (cur) {
      printf("=>");
    }
  }
  printf("\n");
  printf("size: %ld\n", l2.size);
  l2.free(&l2);

  CH_VAR_LIST(XT, l3);
  l3.print_item = print_pxt;
  l3.push_back(&l3, (XT){1, 2});
  l3.push_back(&l3, (XT){3, 4});
  l3.print(&l3);
  l3.free(&l3);
  return EXIT_SUCCESS;
}
