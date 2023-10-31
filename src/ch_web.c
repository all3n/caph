#include "ch_web.h"
#include "ch_json.h"
#include "ch_logging.h"
#include "ch_string.h"
#include "json-c/json.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define WEB_FAVICON "/favicon.ico"
#define _H(HTML) HTML "\n"
#define DEFAULT_PAGE "/index.html"

pthread_key_t thread_key;
struct web_thread_data {
  ch_str_t buf;
};

enum MHD_Result default_handler(ch_web *self, const void *cls,
                                struct MHD_Connection *connection,
                                struct MHD_Response **response) {
  struct Page *page = (struct Page *)cls;
  (void)page;
  char buffer[1024] = {0};
  snprintf(buffer, 1024, "<html><body>404</body></html>");
  *response = MHD_create_response_from_buffer(strlen(buffer), (void *)buffer,
                                              MHD_RESPMEM_MUST_COPY);
  return MHD_queue_response(connection, MHD_HTTP_OK, *response);
}
void free_json_response(struct json_response **data) {
  if (*data == NULL) {
    return;
  }
  struct json_response *response = *(struct json_response **)data;
  if (response->message != NULL) {
    free(response->message);
    response->message = NULL;
  }
  free(response);
  *data = NULL;
}
struct json_object *res_to_json(struct json_response *res) {
  struct json_object *json = json_object_new_object();
  json_object_object_add(json, "data", res->data);
  json_object_object_add(json, "code", json_object_new_int(200));
  if (res->message != NULL) {
    json_object_object_add(json, "message",
                           json_object_new_string(res->message));
  }
  return json;
}

enum MHD_Result json_handler(ch_web *self, const void *cls,
                             struct MHD_Connection *connection,
                             struct MHD_Response **response) {
  struct Page *page = (struct Page *)cls;
  (void)page;
  auto_res(res);
  json_set_s(res->data, "name", "John");
  json_set_i(res->data, "age", 30);
  json_set_b(res->data, "active", 1);
  auto_json json = res_to_json(res);
  const char *json_str = json_object_to_json_string(json);
  *response = MHD_create_response_from_buffer(
      strlen(json_str), (void *)json_str, MHD_RESPMEM_MUST_COPY);
  return MHD_queue_response(connection, MHD_HTTP_OK, *response);
}

static struct Page pages[] = {
    {"/404", CH_WEB_METHOD_GET, "text/html", default_handler, NULL},
    {"/json", CH_WEB_METHOD_GET, "text/html", json_handler, NULL},
    {NULL} // END
};

#define GET_QUERY(c, query)                                                    \
  MHD_lookup_connection_value(c, MHD_GET_ARGUMENT_KIND, #query)
#define GET_POST(c, query)                                                     \
  MHD_lookup_connection_value(c, MHD_POSTDATA_KIND, #query)
#define GET_COOKIE(c, query)                                                   \
  MHD_lookup_connection_value(c, MHD_COOKIE_KIND, #query)
#define GET_HEADER(c, query)                                                   \
  MHD_lookup_connection_value(c, MHD_HEADER_KIND, #query)

int ch_web_auth(struct MHD_Connection *connection) {
  char *password = NULL;
  char *username = MHD_basic_auth_get_username_password(connection, &password);
  int fail = ((username == NULL) || (0 != strcmp(username, "root")) ||
              (0 != strcmp(password, "pa$$w0rd")));
  if (username != NULL)
    free(username);
  if (password != NULL)
    free(password);
  return fail;
}

static enum MHD_Result ch_web_router(struct ch_web *web,
                                     struct MHD_Connection *conn,
                                     const char *path, const char *method,
                                     void **con_cls) {
  // if (!strcmp(path, WEB_FAVICON)) {
    // return MHD_NO;
  // }
  printf("router: %s\n", path);
  struct Page *page =
      (struct Page *)hashmap_get(web->pages, &(struct Page){.url = path});
  if (page == NULL && strcmp(path, "/") == 0) {
    page = (struct Page *)hashmap_get(web->pages,
                                      &(struct Page){.url = DEFAULT_PAGE});
  }
  enum MHD_Result result = MHD_NO;
  if (page == NULL) {
    page = web->default_page;
    result = MHD_YES;
  } else if (page->flags & CH_WEB_METHOD_ANY) {
    result = MHD_YES;
  } else if (strcmp(method, MHD_HTTP_METHOD_GET) == 0 &&
             (page->flags & CH_WEB_METHOD_GET)) {
    result = MHD_YES;
  } else if (strcmp(method, MHD_HTTP_METHOD_POST) == 0 &&
             (page->flags & CH_WEB_METHOD_POST)) {
    result = MHD_YES;
  } else if (strcmp(method, MHD_HTTP_METHOD_PUT) == 0 &&
             (page->flags & CH_WEB_METHOD_PUT)) {
    result = MHD_YES;
  } else if (strcmp(method, MHD_HTTP_METHOD_DELETE) == 0 &&
             (page->flags & CH_WEB_METHOD_DELETE)) {
    result = MHD_YES;
  } else {
    fprintf(stderr, "Method %s not allowed for %s\n", method, path);
  }
  if (result == MHD_YES) {
    *con_cls = page;
  }
  return result;
}

enum MHD_Result answer_to_connection(void *cls,
                                     struct MHD_Connection *connection,
                                     const char *url, const char *method,
                                     const char *version,
                                     const char *upload_data,
                                     size_t *upload_data_size, void **con_cls) {
  ch_web *web = (ch_web *)cls;
  int fail;
  enum MHD_Result ret;
  struct MHD_Response *response;
  if (NULL == *con_cls) {
    return ch_web_router(web, connection, url, method, con_cls);
  }
  struct Page *page = *con_cls;
  fail = web->auth(connection);
  if (fail) {
    const char *page = "<html><body>Go away.</body></html>";
    response = MHD_create_response_from_buffer(strlen(page), (void *)page,
                                               MHD_RESPMEM_PERSISTENT);
    ret = MHD_queue_basic_auth_fail_response(connection, "my realm", response);
  } else {
    XLOG(INFO, "page handler: %s", page->url);
    ret = page->handler(web, page, connection, &response);
  }
  MHD_destroy_response(response);
  return ret;
}
// hash Page hash use
static uint64_t hash_str(const void *item, uint64_t seed0, uint64_t seed1) {
  struct Page *page = (struct Page *)item;
  return hashmap_xxhash3(page->url, strlen(page->url), seed0, seed1);
}
static int compare_strs(const void *a, const void *b, void *udata) {
  struct Page *page_a = (struct Page *)a;
  struct Page *page_b = (struct Page *)b;
  return strcmp(page_a->url, page_b->url);
}

void ch_web_init(struct ch_web *web) {
  srand(time(NULL));
  pthread_key_create(&thread_key, NULL);
  web->handler_callback = answer_to_connection;
  web->add_page = ch_web_add_page;
  uint64_t seed = rand() % INT64_MAX;
  // config page handler callback
  web->pages = hashmap_new(sizeof(struct Page), 0, seed, seed, hash_str,
                           compare_strs, NULL, NULL);

  for (int i = 0; pages[i].url != NULL; i++) {
    hashmap_set(web->pages, &pages[i]);
  }
  int pn = sizeof(pages) / sizeof(struct Page);
  web->default_page = &pages[pn - 2];
}

void ch_web_add_page(struct ch_web *web, struct Page *page) {
  hashmap_set(web->pages, page);
}

void ch_web_start(struct ch_web *web, int wait) {
  web->daemon =
      MHD_start_daemon(MHD_USE_AUTO_INTERNAL_THREAD, web->port, NULL, NULL,
                       web->handler_callback, web, MHD_OPTION_END);
  if (NULL == web->daemon) {
    XLOG(ERROR, "MHD_start_daemon failed");
    return;
  }
  if (wait) {
    getchar();
  }
}
void ch_web_destory(struct ch_web *web) { MHD_stop_daemon(web->daemon); }

ch_web_ret ch_web_static_handler(struct ch_web *self, const void *cls,
                                 ch_web_con *conn, ch_web_res **res) {
  struct Page *page = (struct Page *)cls;
  (void)page;
  CH_WEB_CSTR_TO_RES(res, page->handler_cls);
  return CH_WEB_RES_OK(conn, res);
}
