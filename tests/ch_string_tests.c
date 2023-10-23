#include "ch_string.h"
#include <stdio.h>
#include <stdlib.h>
int main(int argc, char *argv[]) {
  ch_str_t s1 = ch_cstr("4,1,3|4,1|2");
  printf_cstr(s1);
  CH_STR_TOKEN(s1, ",", cur) {
    printf_cstr(cur);
    CH_STR_TOKEN(cur, "|", cur2) { printf_cstr(cur2); }
    printf("\n");
  }
  // useless
  ch_str_free(&s1);

  char buffer[10];
  ch_str_t s3 = ch_str(buffer);
  ch_str_append(&s3, "12345");
  ch_str_append(&s3, "678901");
  ch_str_append(&s3, "xxxx");
  printf_cstr(s3);
  // useless
  ch_str_free(&s3);

  ch_str_t sn = ch_str_new(0);
  ch_str_append(&sn, "12345");
  ch_str_append(&sn, "|12345");
  ch_str_append(&sn, "|12345");
  ch_str_append(&sn, "testestwetasetaestadfasdfa");
  printf_cstr(sn);
  // must free
  ch_str_free(&sn);

  ch_str_t cs1 = ch_cstr("adf");
  ch_str_t cs2 = ch_cstr("adf");
  if (!ch_str_cmp(cs1, cs2)) {
    printf("is same\n");
  }
  if (!ch_str_ccmp(cs1, "adf")) {
    printf("is same2\n");
  }
  ch_str_t cs3 = ch_str_range(cs1, 0, 1);
  printf_cstr(cs3);
  // invalid
  // ch_str_append(&cs3, "asdf");
  return 0;
}
