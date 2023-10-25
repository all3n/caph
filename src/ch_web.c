#include "ch_web.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static struct Page pages[] = {
    {"/", "text/html", NULL, NULL},
    //    {"/2", "text/html", &fill_v1_v2_form, SECOND_PAGE},
    //    {"/S", "text/html", &serve_simple_form, SUBMIT_PAGE},
    //    {"/F", "text/html", &serve_simple_form, LAST_PAGE},
    {NULL} /* 404 */
};

int ch_web_auth(char *username, char *password) {
  int fail = ((username == NULL) || (0 != strcmp(username, "root")) ||
              (0 != strcmp(password, "pa$$w0rd")));
  if (username != NULL)
    free(username);
  if (password != NULL)
    free(password);
  return fail;
}

enum MHD_Result answer_to_connection(void *cls,
                                     struct MHD_Connection *connection,
                                     const char *url, const char *method,
                                     const char *version,
                                     const char *upload_data,
                                     size_t *upload_data_size, void **con_cls) {
  static int n_cnt = 0;
  ch_web *web = (ch_web *)cls;
  char *user;
  char *pass;
  int fail;
  enum MHD_Result ret;
  struct MHD_Response *response;

  // char *qchar = strchr(url, '?');
  // if()
  // struct Page *page = hashmap_get(web->pages, &(struct Page){.url = url});
  printf("recv:%s %s %s\n", method, url, version);

  if (0 != strcmp(method, MHD_HTTP_METHOD_GET))
    return MHD_NO;
  if (NULL == *con_cls) {
    *con_cls = connection;
    return MHD_YES;
  }
  pass = NULL;
  user = MHD_basic_auth_get_username_password(connection, &pass);
  fail = web->auth(user, pass);
  if (fail) {
    const char *page = "<html><body>Go away.</body></html>";
    response = MHD_create_response_from_buffer(strlen(page), (void *)page,
                                               MHD_RESPMEM_PERSISTENT);
    ret = MHD_queue_basic_auth_fail_response(connection, "my realm", response);
  } else {
    // const char *page = "<html><body>A secret.</body></html>";
    printf("CALL ANS\n");
    char buffer[1024] = {0};
    snprintf(buffer, 1024, "<html><body>Hello, %d!</body></html>", n_cnt++);
    response = MHD_create_response_from_buffer(strlen(buffer), (void *)buffer,
                                               MHD_RESPMEM_PERSISTENT);
    ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
  }
  // free(url);
  MHD_destroy_response(response);
  return ret;
}
static uint64_t hash_str(const void *item, uint64_t seed0, uint64_t seed1) {
  struct Page *page = *(struct Page **)item;
  return hashmap_xxhash3(page->url, strlen(page->url), seed0, seed1);
}

static int compare_strs(const void *a, const void *b, void *udata) {
  struct Page *page_a = *(struct Page **)a;
  struct Page *page_b = *(struct Page **)b;
  return strcmp(page_a->url, page_b->url);
}

void ch_web_init(struct ch_web *web) {
  srand(time(NULL));
  web->handler_callback = answer_to_connection;
  web->daemon =
      MHD_start_daemon(MHD_USE_INTERNAL_POLLING_THREAD, web->port, NULL, NULL,
                       web->handler_callback, web, MHD_OPTION_END);
  uint64_t seed = rand() % INT64_MAX;
  web->pages = hashmap_new(sizeof(struct Page), 0, seed, seed, hash_str,
                           compare_strs, NULL, NULL);
  for (int i = 0; pages[i].url != NULL; i++) {
    hashmap_set(web->pages, &pages[i]);
  }
  if (NULL == web->daemon)
    return;
  getchar();
}
void ch_web_destory(struct ch_web *web) { MHD_stop_daemon(web->daemon); }
