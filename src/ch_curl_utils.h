#ifndef _CURL_UTILS_H
#define _CURL_UTILS_H

#include "ch_macro.h"
#include "ch_string.h"
#include <curl/curl.h>
#include <json-c/json.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum ChHTTPMethod {
  GET = 0,
  POST,
  PUT,
  DELETE,
  HEAD,
  PATCH
} ChHTTPMethod;
typedef enum ChResponseType { TEXT = 0, JSON } ChResponseType;

#define MODE_NORMAL 0
#define MODE_UPLOAD 1
#define MODE_DOWNLOAD 2

#define FLAG_ALLOC (1 << 0) // IS ALLOC
#define FLAG_JSON (1 << 1)  // JSON
#define FLAG_RES_HEADER (1 << 2)  // GET RES HEADERS

typedef struct ch_http_request {
  // internal curl handler
  CURL *curl;
  // request url
  char *url;
  // response convert to json
  ChHTTPMethod method;
  // post data
  const char *data;
  // request headers
  struct curl_slist *headers;
  const char *file_path;
  int mode;
  int flags;
  int64_t down_size;
} ch_http_request;

typedef struct ch_http_response {
  enum ChResponseType type;
  // 0-599
  // https://developer.mozilla.org/zh-CN/docs/Web/HTTP/Status
  int http_code : 16;
  // curl code
  CURLcode code : 16;
  struct curl_slist *headers;
  union {
    struct {
      char *data;
      unsigned int size;
    };
    struct json_object *json;
  };
} ch_http_response;

struct file_meta {
  int64_t size;
  int64_t dsize;
  FILE *fp;
};
typedef struct ch_requests ch_requests;
typedef struct ch_requests {
  CURL *curl;
  struct curl_slist *headers;
  void (*init)(ch_requests *requestss);
  ch_http_request *(*build_request)(ch_http_request *req, const char *url, ...);
  ch_http_response *(*do_request)(ch_requests *requestss,
                                  ch_http_request *request);
  void (*add_header)(ch_requests *requestss, const char *key,
                     const char *value);
  void (*clean_request)(ch_http_request *request);
  void (*clean_response)(ch_http_response *response);
  void (*cleanup)(ch_requests *requestss);
} ch_requests;

CH_CAPI_EXPORT bool ch_is_http_ok(int code);
CH_CAPI_EXPORT const char *ch_httpcode2str(int code);
// CH_CAPI_EXPORT void ch_build_query(ch_http_request *req, const char * base,
// ...);
CH_CAPI_EXPORT void ch_add_header(ch_http_request *req, const char *header);
CH_CAPI_EXPORT void ch_print_response(ch_http_response *response);

CH_CAPI_EXPORT void ch_init_requests(ch_requests *requestss);
CH_CAPI_EXPORT ch_http_request *ch_build_http_request(ch_http_request *request,
                                                      const char *url, ...);
CH_CAPI_EXPORT ch_http_response *ch_do_http_request(ch_requests *requestss,
                                                    ch_http_request *request);
CH_CAPI_EXPORT void ch_add_http_header(ch_requests *requestss, const char *key,
                                       const char *value);
CH_CAPI_EXPORT void ch_clean_http_request(ch_http_request *request);
CH_CAPI_EXPORT void ch_clean_http_response(ch_http_response *response);
CH_CAPI_EXPORT void ch_cleanup_requests(ch_requests *requestss);

// CH_CAPI_EXPORT void CH_REQUEST_GET(const char * base, ...);
// CH_CAPI_EXPORT void CH_REQUEST_POST(const char * base, ...);
// CH_CAPI_EXPORT void ch_requests_put(const char * base, ...);
// CH_CAPI_EXPORT void ch_requests_delete(const char * base, ...);

#define CH_MAKE_REQUESTS()                                                     \
  {                                                                            \
    .curl = NULL, .headers = NULL, .init = ch_init_requests,                   \
    .build_request = ch_build_http_request, .do_request = ch_do_http_request,  \
    .add_header = ch_add_http_header, .clean_request = ch_clean_http_request,  \
    .clean_response = ch_clean_http_response, .cleanup = ch_cleanup_requests   \
  }

#define CH_HTTP_OK(res) (res->code == CURLE_OK && res->http_code == 200)

#define CH_HTTP_AUTO(tag, requests, INIT, ...)                                 \
  ch_http_request *req_##tag = requests.build_request(NULL, __VA_ARGS__);      \
  ch_http_response *res_##tag = requests.do_request(&requests, req_##tag);     \
  INIT for (int _m = 1; _m--;                                                  \
            requests.clean_request(req_##tag),                                 \
            requests.clean_response(res_##tag)) if (CH_HTTP_OK(res_##tag))

#define CH_REQUEST_METHOD(tag, m, d, requests, ...)                            \
  ch_http_request *req_##tag = requests.build_request(NULL, __VA_ARGS__);      \
  req_##tag->method = m;                                                       \
  req_##tag->data = d;                                                         \
  ch_http_response *res_##tag = requests.do_request(&requests, req_##tag);     \
  for (int _m = 1; _m--;                                                       \
       requests.clean_request(req_##tag), requests.clean_response(res_##tag))  \
    if (CH_HTTP_OK(res_##tag))

#define CH_REQUEST_GET(tag, requests, ...)                                     \
  CH_REQUEST_METHOD(tag, GET, NULL, requests, __VA_ARGS__)
#define CH_REQUEST_POST(tag, data, requests, ...)                              \
  CH_REQUEST_METHOD(tag, POST, data, requests, __VA_ARGS__)
#define CH_REQUEST_PUT(tag, data, requests, ...)                               \
  CH_REQUEST_METHOD(tag, PUT, data, requests, __VA_ARGS__)
#define CH_REQUEST_DELETE(tag, data, requests, ...)                            \
  CH_REQUEST_METHOD(tag, DELETE, data, requests, __VA_ARGS__)
#ifdef __cplusplus
}
#endif
#endif // !#ifndef _CURL_UTILS_H
