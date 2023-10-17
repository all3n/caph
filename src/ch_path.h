#ifndef _CH_PATH_H
#define _CH_PATH_H

/* get user path, you should free path if your not use avoid memory leak */
char *get_user_path(const char *sub_path);
int is_dir(const char *path);
char *expanduser(const char *path);
#endif
