#ifndef _CH_PATH_H
#define _CH_PATH_H

#include "ch_macro.h"
#include "ch_string.h"
#ifdef __cplusplus
extern "C" {
#endif

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

typedef int (*ch_path_callback)(const char *root, const char *sub_path,
                                const char *name, int flags, void *user_data);

// get user path with sub_path, return pointer you need to free
// return:
//    1. NULL if get HOME env fail
//    2. char * if success and you need to free it
CH_CAPI_EXPORT char *ch_get_user_path(const char *sub_path);
// convert dot to underscore inplace
// str: in, out
CH_CAPI_EXPORT void ch_convert_dot_to_underscore(char *str);
// check if path is directory
CH_CAPI_EXPORT int ch_is_dir(const char *path);
// expand user path, like python os.path.expanduser
// path: input path
// return: expand path(need free)
CH_CAPI_EXPORT char *ch_expanduser(const char *path);
CH_CAPI_EXPORT char *ch_get_file_no_ext(const char *path);
CH_CAPI_EXPORT char *ch_get_file_name(const char *path);
// must end with NULL
CH_CAPI_EXPORT ch_str_t ch_path_join(const char *base, ...);
CH_CAPI_EXPORT int ch_loop_dir(const char *root, const char *sub_path,
                               ch_path_callback callback, void *user_data);

#ifdef __cplusplus
}
#endif
