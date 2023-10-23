#include "ch_string.h"
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
char *ch_fmt(const char *format, ...) {
  int bufferSize = 100;
  char *buffer = (char *)malloc(bufferSize * sizeof(char));
  va_list args;
  va_start(args, format);
  int result = vsnprintf(buffer, bufferSize, format, args);
  va_end(args);
  if (result >= bufferSize) {
    bufferSize = result + 1;
    buffer = (char *)realloc(buffer, bufferSize * sizeof(char));
    va_start(args, format);
    vsnprintf(buffer, bufferSize, format, args);
    va_end(args);
  }
  return buffer;
}

char *ch_append_fmt(char **str, const char *format, ...) {
  va_list args;
  va_start(args, format);
  // 计算格式化字符串所需的长度
  int len = vsnprintf(NULL, 0, format, args);
  va_end(args);
  // 分配足够的内存来容纳源字符串和格式化字符串
  int oldLen = (*str != NULL) ? strlen(*str) : 0;
  int newLen = oldLen + len + 1;
  char *newStr = (char *)realloc(*str, newLen);
  // 将格式化字符串追加到源字符串之后
  va_start(args, format);
  vsnprintf(newStr + oldLen, len + 1, format, args);
  va_end(args);
  *str = newStr;
  return *str;
}

ch_str_t ch_str_new(unsigned int init_size) {
  ch_str_t str;
  if (init_size == 0) {
    init_size = CH_STRING_INIT_SIZE;
  }
  str.str = (char *)malloc(init_size * sizeof(char));
  memset(str.str, 0, init_size * sizeof(char));
  str.len = 0;
  str.capacity = init_size;
  str.is_mutable = 1;
  str.own = 1;
  return str;
}
void ch_str_free(ch_str_t *str) {
  if (str && str->own) {
    free(str->str);
  }
}
int ch_str_own(const ch_str_t str) { return str.own; }
int ch_str_empty(const ch_str_t str) { return str.len == 0; }

void ch_str_append(ch_str_t *str, const char *format, ...) {
  assert(str != NULL && str->is_mutable);
  va_list args;
  if (str->own) {
    va_start(args, format);
    int len = vsnprintf(NULL, 0, format, args);
    va_end(args);
    if (str->len + len > str->capacity * CH_STRING_GROW_RATE) {
      str->capacity = (str->len + len) * CH_STRING_GROW_FACTOR;
      printf("str->capacity = %d\n", str->capacity);
      str->str = (char *)realloc(str->str, str->capacity);
    }
    va_start(args, format);
    size_t rln =
        vsnprintf(str->str + str->len, str->capacity - str->len, format, args);
    str->len += rln;
    va_end(args);
  } else {
    if (str->len + 1 == str->capacity) {
      fprintf(stderr, "ch_str capacity full %d\n", str->capacity);
      return;
    }
    va_start(args, format);
    vsnprintf(str->str + str->len, str->capacity - str->len, format, args);
    str->len = strlen(str->str);
    va_end(args);
  }
}
ch_str_t ch_str_tk(ch_str_t *str, const char *sep) {
  ch_str_t token = {NULL};
  if (!str->str) {
    return token;
  }
  char *p = strnstr(str->str, sep, str->len);
  if (p) {
    token.str = str->str;
    token.len = p - str->str;
    str->str = p + strlen(sep);
    str->len -= token.len + strlen(sep);
  } else {
    token.str = str->str;
    token.len = str->len;
    str->len = 0;
    str->str = NULL;
  }
  return token;
}

ch_str_t ch_str_range(const ch_str_t str, int start, int end) {
  assert(start >= 0 && end >= 0);
  assert(start < str.len && end < str.len);
  ch_str_t range = {0};
  range.str = str.str + start;
  range.len = end - start;
  return range;
}
int ch_str_cmp(const ch_str_t str1, const ch_str_t str2) {
  if (str1.len != str2.len) {
    return str1.len - str2.len;
  }
  return strncmp(str1.str, str2.str, str1.len);
}

int ch_str_ccmp(const ch_str_t str1, const char *cstr2) {
  return strncmp(str1.str, cstr2, str1.len);
}
