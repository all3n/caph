#ifndef _CH_PATH_H
#define _CH_PATH_H

#include "ch_macro.h"
#ifdef __cplusplus
extern "C" {
#endif
/* get user path, you should free path if your not use avoid memory leak */

#ifdef _WIN32
#define CH_PATH_SEP '\\'
#define CH_PATH_SEP_STR "\\"
#else
#define CH_PATH_SEP '/'
#define CH_PATH_SEP_STR "/"
#define CH_PATH_SEP_STR_LEN 1
#endif

#define CH_PATH_SEP_STR_LEN 1
#else
#define CH_PATH_SEP '/'
#define CH_PATH_SEP_STR "/"
#endif

typedef int (*ch_path_callback)(const char *path, const char *name, int flags,
                                void *user_data);

CH_CAPI_EXPORT char *ch_get_user_path(const char *sub_path);
CH_CAPI_EXPORT int ch_is_dir(const char *path);
CH_CAPI_EXPORT char *ch_expanduser(const char *path);
CH_CAPI_EXPORT char *ch_get_file_no_ext(const char *path);
CH_CAPI_EXPORT int ch_loop_dir(const char *dir, ch_path_callback callback,
                               void *user_data);

#ifdef __cplusplus
}
#endif
