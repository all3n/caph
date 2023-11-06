#ifndef _CH_MD5_H
#define _CH_MD5_H

#include "ch_macro.h"
#include "ch_string.h"
#ifdef __cplusplus
extern "C" {
#endif

CH_CAPI_EXPORT void ch_hash_md5(const char *key, unsigned int length,
                              char *result);
CH_CAPI_EXPORT ch_str_t ch_hash_md5_cstr(const char *key, unsigned int length);

#ifdef __cplusplus
}
#endif

#endif
