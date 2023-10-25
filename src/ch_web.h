#ifndef _CH_WEB_H
#define _CH_WEB_H
#include <microhttpd.h>
#include "hashmap.h"
#ifdef __cplusplus
extern "C" {
#endif

struct Session {
  /**
   * We keep all sessions in a linked list.
   */
  struct Session *next;
  /**
   * Unique ID for this session.
   */
  char sid[33];
  /**
   * Reference counter giving the number of connections
   * currently using this session.
   */
  unsigned int rc;
  /**
   * Time when this session was last active.
   */
  time_t start;
  /**
   * String submitted via form.
   */
  char value_1[64];
  /**
   * Another value submitted via form.
   */
  char value_2[64];
};

typedef enum MHD_Result (*PageHandler)(const void *cls, const char *mime,
                                       struct Session *session,
                                       struct MHD_Connection *connection);
struct Page {
  const char *url;
  const char *mime;
  PageHandler handler;
  const void *handler_cls;
};

typedef struct ch_web {
  struct MHD_Daemon *daemon;
  struct MHD_Connection *connection;
  struct MHD_Request *request;
  MHD_AccessHandlerCallback handler_callback;
  int (*auth)(char *username, char *password);
  struct hashmap * pages;
  int port;
} ch_web;

void ch_web_init(struct ch_web *web);
void ch_web_destory(struct ch_web *web);
int ch_web_auth(char *username, char *password);

#define MAKE_CH_WEB(...)                                                       \
  { .auth = ch_web_auth, __VA_ARGS__ }

#ifdef __cplusplus
}
#endif
#endif
