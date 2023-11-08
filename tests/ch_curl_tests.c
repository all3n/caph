#include "ch_curl_utils.h"
#include <string.h>
#include <unity.h>

static ch_requests requests;
void setUp(void) { ch_init_requests(&requests); }
void tearDown(void) { requests.cleanup(&requests); }

void test_simple_get() {
  ch_http_request *request =
      requests.build_request(NULL, "http://www.baidu.com");
  ch_http_response *response = requests.do_request(&requests, request);
  ch_print_response(response);
  requests.clean_request(request);
  requests.clean_response(response);
}

void test_reuse_requests() {
  ch_http_request *request2 =
      requests.build_request(NULL, "http://www.baidu.com");
  ch_http_response *response2 = requests.do_request(&requests, request2);
  // not clean for reuse
  // requests.CleanRequest(request2);
  requests.clean_response(response2);
  // reuse request2
  requests.build_request(request2, "http://www.taobao.com");
  ch_http_response *res3 = requests.do_request(&requests, request2);
  if (CH_HTTP_OK(res3)) {
    printf("OK\n");
  }
  requests.build_request(request2, "http://asdfadsf.com");
  res3 = requests.do_request(&requests, request2);
  if (CH_HTTP_OK(res3)) {
    printf("OK\n");
  }
  // ch_print_response(res3);
  requests.clean_response(res3);
}
void test_requests_auto() {
  CH_HTTP_AUTO(
      x, requests, { req_x->flags |= FLAG_RES_HEADER; },
      "http://www.baidu.com") {
    ch_print_response(res_x);
  }
  else {
    ch_print_response(res_x);
  }
}

void test_requests_get_macro() {
  CH_REQUEST_GET(test, requests, "http://www.baidu.com") {
    ch_print_response(res_test);
  }
}

int main(int argc, char *argv[]) {
  UNITY_BEGIN();
  RUN_TEST(test_simple_get);
  RUN_TEST(test_reuse_requests);
  RUN_TEST(test_requests_auto);
  RUN_TEST(test_requests_get_macro);
  return UNITY_END();

  return 0;
}
