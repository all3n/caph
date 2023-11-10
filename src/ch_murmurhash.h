#ifndef _CH_MURMURHASH3_H
#define _CH_MURMURHASH3_H
#include "ch_macro.h"
#include <stddef.h>
// Platform-specific functions and macros
// Microsoft Visual Studio
#if defined(_MSC_VER) && (_MSC_VER < 1600)
typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef unsigned __int64 uint64_t;
// Other compilers
#else // defined(_MSC_VER)
#include <stdint.h>
#endif // !defined(_MSC_VER)
#ifdef __cplusplus
extern "C" {
#endif

CH_CAPI_EXPORT void MurmurHash3_x86_32(const void *key, int len, uint32_t seed,
                                       void *out);
CH_CAPI_EXPORT void MurmurHash3_x86_128(const void *key, int len, uint32_t seed,
                                        void *out);
CH_CAPI_EXPORT void MurmurHash3_x64_128(const void *key, int len, uint32_t seed,
                                        void *out);
CH_CAPI_EXPORT uint64_t ch_murmurhash3(const void *key, const int len,
                                       uint32_t seed);
#ifdef __cplusplus
}
#endif
#endif
