#include "ch_curl_utils.h"
#include <string.h>
int main(int argc, char *argv[]) {
  ch_requests requests = CH_MAKE_REQUESTS();
  requests.init(&requests);
  ch_http_request *request =
      requests.build_request(NULL, "http://www.baidu.com");
  ch_http_response *response = requests.do_request(&requests, request);
  requests.clean_request(request);
  requests.clean_response(response);
  ch_http_request *request2 =
      requests.build_request(NULL, "http://www.baidu.com");
  ch_http_response *response2 = requests.do_request(&requests, request2);
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
  CH_HTTP_AUTO(
      x, requests, { req_x->flags |= FLAG_RES_HEADER; },
      "http://www.baidu.com") {
    ch_print_response(res_x);
  }
  else {
    ch_print_response(res_x);
  }
  CH_REQUEST_GET(test, requests, "http://www.baidu.com") {
    ch_print_response(res_test);
  }
  requests.cleanup(&requests);
  return 0;
}
