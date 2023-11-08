#include "ch_md5.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unity.h>

void setUp(void) {}
void tearDown(void) {}
void md5_str(){
  char out[33] = {0};
  char buffer[] = "admin";
  ch_hash_md5(buffer, strlen(buffer), out);
  UnityAssertEqualString("21232f297a57a5a743894a0e4a801fc3", out, "md5 not same", 1);
}
void md5_cstr(){
  char buffer[] = "admin";
  ch_str_t md5 = ch_hash_md5_cstr(buffer, strlen(buffer));
  printf_cstr(md5);
  ch_str_free(&md5);
}
int main(int argc, char *argv[]) {
  UNITY_BEGIN();
  RUN_TEST(md5_str);
  RUN_TEST(md5_cstr);
  return UNITY_END();
}
