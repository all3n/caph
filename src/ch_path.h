#ifndef _CH_PATH_H
#define _CH_PATH_H

#include "ch_macro.h"
#ifdef __cplusplus
extern "C" {
#endif
/* get user path, you should free path if your not use avoid memory leak */
CH_CAPI_EXPORT char *ch_get_user_path(const char *sub_path);
CH_CAPI_EXPORT int ch_is_dir(const char *path);
CH_CAPI_EXPORT char *ch_expanduser(const char *path);

#ifdef __cplusplus
}
#endif
#endif
