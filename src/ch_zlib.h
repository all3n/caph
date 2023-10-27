#ifndef _CH_ZLIB_H
#define _CH_ZLIB_H
#include "ch_macro.h"
#include <zlib.h>
#ifdef __cplusplus
extern "C" {
#endif
CH_CAPI_EXPORT int ch_zlib_compress(const char *input, uLong input_size,
                                    char **output, uLong *output_size);
CH_CAPI_EXPORT int ch_zlib_decompress(const char *input, uLong input_size,
                                      char **output, uLong *output_size);
#ifdef __cplusplus
}
#endif
#endif
