#include "ch_logging.h"
#include "ch_string.h"
#include "ch_web.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define T_INT 1
#define T_FLOAT 2
#define T_STRING 3
#define T_ARRAY 4
#define T_OBJECT 5
#define T_BOOL 6
#define T_NULL 7
#define T_STRING_PTR 8

struct KV {
  int type : 8;
  int variable : 1;
  char *key;
  union {
    int *iVal;
    float *fVal;
    long *lVal;
    char *sVal;
    bool *bVal;
    void *oVal;
    struct {
      char **value;
      int *size;
    } str_arr;
  };
};
static uint64_t hash_kv(const void *item, uint64_t seed0, uint64_t seed1) {
  struct KV *k = (struct KV *)item;
  return hashmap_xxhash3(k->key, strlen(k->key), seed0, seed1);
}
static int compare_kv(const void *a, const void *b, void *udata) {
  struct KV *item_a = (struct KV *)a;
  struct KV *item_b = (struct KV *)b;
  return strcmp(item_a->key, item_b->key);
}

struct MonitorVars {
  struct hashmap *vars;
};
ch_web_ret test_handler(struct ch_web *self, const void *cls, ch_web_con *conn,
                        ch_web_res **res) {
  ch_str_t buf = ch_str_new(150);
  ch_str_append(&buf, "<html><body>");
  struct MonitorVars *mv = (struct MonitorVars *)self->data;
  if (mv->vars) {
    size_t iter = 0;
    void *iter_val;
    while (hashmap_iter(mv->vars, &iter, &iter_val)) {
      struct KV *kv = (struct KV *)iter_val;
      if (kv->type == T_INT) {
        ch_str_append(&buf, "<p>%s int %s=%d</p>",
                      kv->variable ? "var" : "const", kv->key,
                      *(int *)kv->iVal);
      } else if (kv->type == T_STRING_PTR) {
        for (int i = 0; i < *kv->str_arr.size; i++) {
          ch_str_append(&buf, "<p>%s arr %s[%d]=%s</p>",
                        kv->variable ? "var" : "const", kv->key, i,
                        kv->str_arr.value[i]);
        }
      }
    }
  }
  ch_str_append(&buf, "</body></html>");
  CH_WEB_PTR_TO_RES(res, buf.str, buf.len);
  return CH_WEB_RES_OK(conn, res);
}
ch_web_ret test2_handler(struct ch_web *self, const void *cls, ch_web_con *conn,
                         ch_web_res **res) {
  struct Page *page = (struct Page *)cls;
  (void)page;
  CH_WEB_CSTR_TO_RES(res, page->handler_cls);
  return CH_WEB_RES_OK(conn, res);
}

#define HTML_PAGE2 "<html><body>Hello page2</body></html>"
#define HTML_PAGE3 "<html><body>Hello page3</body></html>"
#define HTML_PAGE4 "<html><body>Hello page4</body></html>"

int main(int argc, char *argv[]) {
  ch_web web = MAKE_CH_WEB(.port = 9999);
  struct MonitorVars vars = {};
  web.data = &vars;
  vars.vars =
      hashmap_new(sizeof(struct KV), 0, 0, 0, hash_kv, compare_kv, NULL, NULL);
  hashmap_set(vars.vars,
              &(struct KV){.key = "argc", .iVal = &argc, .type = T_INT});
  hashmap_set(vars.vars, &(struct KV){.key = "argv",
                                      .str_arr = {argv, &argc},
                                      .type = T_STRING_PTR});

  ch_web_init(&web);
  web.add_page(&web, &(struct Page){"/test", CH_WEB_METHOD_GET, "text/html",
                                    test_handler, NULL});
  web.add_page(&web, &(struct Page){"/page2", CH_WEB_METHOD_GET, "text/html",
                                    test2_handler, HTML_PAGE2});
  web.add_page(&web, &(struct Page){"/page3", CH_WEB_METHOD_GET, "text/html",
                                    ch_web_static_handler, HTML_PAGE3});
  web.add_page(&web, STATIC_PAGE(/fff, HTML_PAGE4);

  ch_web_start(&web, 1);
  ch_web_destory(&web);
  return EXIT_SUCCESS;
}
