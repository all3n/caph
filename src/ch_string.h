#ifndef _CH_STRING_H
#define _CH_STRING_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ch_macro.h"
#include <stdlib.h>
#include <string.h>

#define CH_STRING_INIT_SIZE 10
#define CH_STRING_GROW_RATE 0.8
#define CH_STRING_GROW_FACTOR 1.2
#define _CH_STRING_BASE                                                        \
  char *str;                                                                   \
  unsigned int own : 1;                                                        \
  unsigned int is_mutable : 1;                                                 \
  unsigned int len : 15;                                                       \
  unsigned int capacity : 15;

typedef struct ch_str_t {
  _CH_STRING_BASE
} ch_str_t;

#define ch_cstr(str)                                                           \
  { str, 0, 0, sizeof(str) - 1, 0 }

#define ch_str(str)                                                            \
  { str, 0, 1, strlen(str), sizeof(str) }

#define ch_zstr()                                                              \
  { NULL }

#define printf_cstr(cstr)                                                      \
  printf("ch_str: %.*s own:%d mutable:%d len:%d capacity:%d\n", cstr.len,      \
         cstr.str, cstr.own, cstr.is_mutable, cstr.len, cstr.capacity)

CH_CAPI_EXPORT char *ch_fmt(const char *format, ...);
CH_CAPI_EXPORT char *ch_append_fmt(char **str, const char *format, ...);
CH_CAPI_EXPORT ch_str_t ch_str_new(unsigned int init_size);
CH_CAPI_EXPORT void ch_str_free(ch_str_t *str);
CH_CAPI_EXPORT int ch_str_own(const ch_str_t str);
CH_CAPI_EXPORT int ch_str_empty(const ch_str_t str);
CH_CAPI_EXPORT void ch_str_append(ch_str_t *str, const char *s, ...);
CH_CAPI_EXPORT ch_str_t ch_str_tk(ch_str_t *str, const char *sep);
CH_CAPI_EXPORT ch_str_t ch_str_range(const ch_str_t str, int start, int end);
CH_CAPI_EXPORT int ch_str_cmp(const ch_str_t str1, const ch_str_t str2);
CH_CAPI_EXPORT int ch_str_ccmp(const ch_str_t str1, const char *cstr2);

#define CH_STR_TOKEN(INPUT, DELIMS, CUR)                                       \
  for (ch_str_t t_##CUR = INPUT, CUR = ch_str_tk(&t_##CUR, DELIMS); CUR.str;   \
       CUR = ch_str_tk(&t_##CUR, DELIMS))

#ifdef __cplusplus
}
#endif

#endif
