#include "ch_string.h"
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
