#ifndef _CH_MD5_H
#define _CH_MD5_H
#include "ch_string.h"
void ChHashMD5(const char *key, unsigned int length, char *result);
ch_str_t ChHashMd5CStr(const char *key, unsigned int length);
#endif
