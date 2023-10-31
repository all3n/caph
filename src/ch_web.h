#ifndef _CH_WEB_H
#define _CH_WEB_H
#include "hashmap.h"
#include <microhttpd.h>
#ifdef __cplusplus
extern "C" {
#endif

#define CH_WEB_METHOD_ANY (1)
#define CH_WEB_METHOD_GET (1 << 1)
#define CH_WEB_METHOD_POST (1 << 2)
#define CH_WEB_METHOD_PUT (1 << 3)
#define CH_WEB_METHOD_DELETE (1 << 4)
#define CH_WEB_METHOD_HEAD (1 << 5)
#define CH_WEB_METHOD_OPTIONS (1 << 6)
#define CH_WEB_METHOD_TRACE (1 << 7)
#define CH_WEB_METHOD_CONNECT (1 << 8)
#define CH_WEB_METHOD_PATCH (1 << 9)

typedef struct MHD_Connection ch_web_con;
typedef struct MHD_Response ch_web_res;
typedef enum MHD_Result ch_web_ret;
#define CH_WEB_RET_OK (MHD_YES)
#define CH_WEB_RET_ERR (MHD_NO)

#define CH_WEB_BUF_TO_RES(res, xb)                                             \
  *res = MHD_create_response_from_buffer(strlen(xb), (void *)xb,               \
                                         MHD_RESPMEM_MUST_COPY)
#define CH_WEB_PTR_TO_RES(res, xb, xb_len)                                     \
  *res = MHD_create_response_from_buffer(xb_len, (void *)xb,                   \
                                         MHD_RESPMEM_MUST_FREE)
#define CH_WEB_CSTR_TO_RES(res, xb)                                            \
  *res = MHD_create_response_from_buffer(strlen(xb), (void *)xb,               \
                                         MHD_RESPMEM_PERSISTENT)

#define CH_WEB_NCSTR_TO_RES(res, xb, nxb)                                      \
  *res =                                                                       \
      MHD_create_response_from_buffer(nxb, (void *)xb, MHD_RESPMEM_PERSISTENT)

#define CH_WEB_RES_OK(conn, res) MHD_queue_response(conn, MHD_HTTP_OK, *res);

#define STATIC_PAGE(url, content) &(struct Page){#url, CH_WEB_METHOD_GET, "text/html", \
                                    ch_web_static_handler, content});

struct ch_web;
typedef enum MHD_Result (*PageHandler)(struct ch_web *self, const void *cls,
                                       struct MHD_Connection *connection,
                                       struct MHD_Response **response);
struct Page {
  const char *url;
  uint32_t flags;
  const char *mime;
  PageHandler handler;
  const void *handler_cls;
};
struct json_response {
  struct json_object *data;
  int code;
  char *message;
};

typedef struct ch_web {
  void *data;
  struct MHD_Daemon *daemon;
  MHD_AccessHandlerCallback handler_callback;
  int (*auth)(struct MHD_Connection *connection);
  struct hashmap *pages;
  struct Page *default_page;
  void (*add_page)(struct ch_web *web, struct Page *page);
  int port;
} ch_web;

void free_json_response(struct json_response **data);
void ch_web_init(struct ch_web *web);
void ch_web_add_page(struct ch_web *web, struct Page *page);
void ch_web_start(struct ch_web *web, int wait);
void ch_web_destory(struct ch_web *web);
int ch_web_auth(struct MHD_Connection *connection);

ch_web_ret ch_web_static_handler(ch_web *self, const void *cls,
                                 ch_web_con *conn, ch_web_res **res);
#define auto_res(res)                                                          \
  __attribute__((cleanup(free_json_response))) struct json_response *res =     \
      malloc(sizeof(struct json_response));                                    \
  memset(res, 0, sizeof(struct json_response));                                \
  res->data = json_object_new_object()

#define MAKE_CH_WEB(...)                                                       \
  { .auth = ch_web_auth, __VA_ARGS__ }

#ifdef __cplusplus
}
#endif
#endif
