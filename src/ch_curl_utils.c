#include "ch_logging.h"
#include <assert.h>
#include <curl/curl.h>
#include <curl/easy.h>
#include <json-c/json_tokener.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "ch_curl_utils.h"

bool ch_is_http_ok(int code) { return code == CURLE_OK; }
const char *ch_httpcode2str(int code) { return curl_easy_strerror(code); }

void ch_add_header(ch_http_request *req, const char *header) {
  req->headers = curl_slist_append(req->headers, header);
}

void ch_print_response(ch_http_response *response) {
  if (response == NULL) {
    XLOG(INFO, "Response is NULL\n");
    return;
  }
  if (CH_HTTP_OK(response)) {
    if (response->type == TEXT) {
      int size = response->size;
      if (size > 1000) {
        size = 1000;
      }
      XLOG(INFO, "Response size:%d\n%.*s%s", response->size, size,
           response->data, size < response->size ? "..." : "");
    } else if (response->type == JSON) {
      XLOG(INFO, "Response: %s",
           json_object_to_json_string_ext(response->json,
                                          JSON_C_TO_STRING_PRETTY));
    }
  } else {
    XLOG(INFO, "ERROR HTTP Code:%d CURL Code:\"%s\"", response->http_code,
         ch_httpcode2str(response->code));
  }
}

static size_t write_callback_file(void *ptr, size_t size, size_t nmemb,
                                  FILE *stream) {
  return fwrite(ptr, size, nmemb, stream);
}

static int progress_callback(void *clientp, curl_off_t dltotal,
                             curl_off_t dlnow, curl_off_t ultotal,
                             curl_off_t ulnow) {
  if (dltotal > 0) {
    double progress = (double)dlnow / (double)dltotal * 100.0;
    printf("Download progress: %.2f%%\r", progress);
  }
  return 0;
}
static size_t header_callback(char *buffer, size_t size, size_t nitems,
                              void *userdata) {
  ch_http_response *response = (ch_http_response *)userdata;
  int total_size = size * nitems;
  printf("%.*s\n", total_size, buffer);
  curl_slist_append(response->headers, buffer);
  return total_size;
}

// 回调函数，用于处理curl的返回结果
static size_t write_callback(void *ptr, size_t size, size_t nmemb,
                             void *userdata) {
  ch_http_response *data = (ch_http_response *)userdata;
  size_t real_size = size * nmemb;
  data->data = (char *)realloc(data->data, data->size + real_size + 1);
  if (data->data == NULL) {
    XLOG(ERROR, "realloc failed\n");
    return 0;
  }
  memcpy(data->data + data->size, ptr, real_size);
  data->size += real_size;
  data->data[data->size] = '\0';
  return real_size;
}

// 回调函数，用于上传文件
static size_t read_callback(void *ptr, size_t size, size_t nmemb,
                            void *stream) {
  FILE *file = fopen((char *)stream, "rb");
  size_t nread;
  if (file) {
    nread = fread(ptr, size, nmemb, file);
    fclose(file);
    return nread;
  }
  return 0;
}

void ch_init_requests(ch_requests *requests) {
  memset(requests, 0, sizeof(struct ch_requests));
  requests->do_request = ch_do_http_request;
  requests->build_request = ch_build_http_request;
  requests->add_header = ch_add_http_header;
  requests->clean_request = ch_clean_http_request;
  requests->clean_response = ch_clean_http_response;
  requests->cleanup = ch_cleanup_requests;
  curl_global_init(CURL_GLOBAL_DEFAULT);
}

ch_http_request *ch_build_http_request(ch_http_request *request,
                                       const char *format, ...) {
  if (request == NULL) {
    request = (ch_http_request *)malloc(sizeof(ch_http_request));
    if (request == NULL) {
      XLOG(ERROR, "malloc failed");
      return NULL;
    }
    memset(request, 0, sizeof(ch_http_request));
  } else {
    if (request->url) {
      free(request->url);
      request->url = NULL;
    }
  }
  request->curl = curl_easy_init();
  va_list args;
  va_start(args, format);
  int len = vsnprintf(NULL, 0, format, args);
  va_end(args);
  request->url = (char *)malloc(len + 1);
  va_start(args, format);
  vsnprintf(request->url, len + 1, format, args);
  va_end(args);
  return request;
}
ch_http_response *ch_do_http_request(ch_requests *requests,
                                     ch_http_request *request) {
  CURLcode res;
  ch_http_response *response =
      (ch_http_response *)malloc(sizeof(ch_http_response));
  assert(response != NULL);
  memset(response, 0, sizeof(ch_http_response));
  FILE *fp = NULL;
  if (request->curl) {
    curl_easy_setopt(request->curl, CURLOPT_URL, request->url);
    // support 302 redirect
    curl_easy_setopt(request->curl, CURLOPT_FOLLOWLOCATION, 1);
    if (request->mode == MODE_DOWNLOAD) {
      XLOG(DEBUG, "download %s\n", request->file_path);
      fp = fopen(request->file_path, "wb");
      if (fp == NULL) {
        // fopen error
        XLOG(ERROR, "fopen error:%s\n", request->file_path);
        return NULL;
      }
      curl_easy_setopt(request->curl, CURLOPT_WRITEFUNCTION,
                       write_callback_file);
      curl_easy_setopt(request->curl, CURLOPT_WRITEDATA, fp);
      // FOR No Content-Length
      curl_easy_setopt(request->curl, CURLOPT_XFERINFOFUNCTION,
                       progress_callback);
      curl_easy_setopt(request->curl, CURLOPT_NOPROGRESS, 0L);
      //      curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, &down_file);
    } else {
      curl_easy_setopt(request->curl, CURLOPT_WRITEFUNCTION, write_callback);
      curl_easy_setopt(request->curl, CURLOPT_WRITEDATA, response);
    }
    switch (request->method) {
    case GET:
      break;
    case POST:
      curl_easy_setopt(request->curl, CURLOPT_POST, 1);
      if (request->data && strlen(request->data) > 0) {
        curl_easy_setopt(request->curl, CURLOPT_POSTFIELDS, request->data);
      } else {
        curl_easy_setopt(request->curl, CURLOPT_POSTFIELDS, "");
      }

      if (request->mode == MODE_UPLOAD && request->file_path &&
          strlen(request->file_path) > 0) {
        curl_easy_setopt(request->curl, CURLOPT_UPLOAD, 1L);
        curl_easy_setopt(request->curl, CURLOPT_READFUNCTION, read_callback);
        curl_easy_setopt(request->curl, CURLOPT_READDATA, request->file_path);
      }
      break;
    default:
      break;
    }
    if (request->headers) {
      curl_easy_setopt(request->curl, CURLOPT_HTTPHEADER, request->headers);
    }
    if (request->flags & FLAG_RES_HEADER) {
      curl_easy_setopt(request->curl, CURLOPT_HEADER, 1);
      // curl_easy_setopt(request->curl, CURLOPT_HEADERDATA, response);
      // curl_easy_setopt(request->curl, CURLOPT_HEADERFUNCTION,
      // header_callback);
    }
    res = curl_easy_perform(request->curl);
    long http_code = 0;
    if (res == CURLE_OK) {
      curl_easy_getinfo(request->curl, CURLINFO_RESPONSE_CODE, &http_code);
      XLOG(DEBUG, "http_code:%ld", http_code);
    }
    response->http_code = (int)http_code;
    response->code = res;
    if ((request->flags & FLAG_JSON) && response->data) {
      response->type = JSON;
      struct json_object *json =
          json_tokener_parse(response->data); // free txt response
      free(response->data);
      response->data = NULL;
      response->size = 0;
      response->json = json;
    }
    if (fp) {
      printf("\n");
      fclose(fp);
    }
  }
  return response;
}
void ch_add_http_header(ch_requests *requests, const char *key,
                        const char *value) {}
void ch_clean_http_request(ch_http_request *request) {
  if (request->url) {
    free(request->url);
    request->url = NULL;
  }
  if (request->headers) {
    curl_slist_free_all(request->headers);
    request->headers = NULL;
  }
  if (request->curl) {
    curl_easy_cleanup(request->curl);
    request->curl = NULL;
  }
  free(request);
}
void ch_clean_http_response(ch_http_response *response) {
  if (response == NULL) {
    return;
  }
  if (response->data) {
    free(response->data);
    response->data = NULL;
  }
  if (response->json) {
    json_object_put(response->json);
    response->json = NULL;
  }
  free(response);
}
void ch_cleanup_requests(ch_requests *requests) { curl_global_cleanup(); }
