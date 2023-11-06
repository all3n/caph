#ifndef _CH_OPTS_H
#define _CH_OPTS_H

#include "ch_macro.h"
#include "hashmap.h"
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif
struct arg_opt;
typedef struct ch_arg_parser {
  char *prog;
  char *description;
  struct hashmap *opts_map;
  struct hashmap *alias;
  char *error_msg;
  void (*add_arg)(struct ch_arg_parser *parser, struct arg_opt *opt);
  int (*parse)(struct ch_arg_parser *parser, int argc, char **argv);
  void (*cleanup)(struct ch_arg_parser *parser);
} ch_arg_parser;

typedef enum arg_opt_type {
  kArgString = 0,
  kArgInt,
  kArgLong,
  kArgFloat,
  kArgBool,
  kArgEnum,
  kArgObj
} arg_opt_type;

typedef int (*ParseFunc)(ch_arg_parser *, char *, void *data);
typedef void (*ArgFunc)(void *data);
typedef struct ArgAlias {
  char *name;
  char *alias;
} ArgAlias;
typedef struct arg_opt {
  void *data;
  char *s_name;
  char *l_name;
  char *desc;
  arg_opt_type type;
  bool required;
  bool set_flag;
  void *def_val;
  ParseFunc func;
  ArgFunc arg_free;
  ArgFunc arg_print;
} arg_opt;

#define ARG_END                                                                \
  { NULL }

#define ARG_OBOOL(obj, s_name, l_name, ...)                                    \
  &(arg_opt) { obj, #s_name, #l_name, .type = kArgBool, __VA_ARGS__ }
#define ARG_BOOL(s_name, l_name, ...)                                          \
  &(arg_opt) { &opts.l_name, #s_name, #l_name, .type = kArgBool, __VA_ARGS__ }

#define ARG_OSTR(obj, s_name, l_name, ...)                                     \
  &(arg_opt) { obj, #s_name, #l_name, .type = kArgString, __VA_ARGS__ }
#define ARG_STR(s_name, l_name, ...)                                           \
  &(arg_opt) { &opts.l_name, #s_name, #l_name, .type = kArgString, __VA_ARGS__ }

#define ARG_OINT(obj, s_name, l_name, ...)                                     \
  &(arg_opt) { obj, #s_name, #l_name, .type = kArgInt, __VA_ARGS__ }
#define ARG_INT(s_name, l_name, ...)                                           \
  &(arg_opt) { &opts.l_name, #s_name, #l_name, .type = kArgInt, __VA_ARGS__ }

#define ARG_OBJ(obj, s_name, l_name, ...)                                      \
  &(arg_opt) { obj, #s_name, #l_name, __VA_ARGS__ }

CH_CAPI_EXPORT int ch_parse_host(ch_arg_parser *parser, char *t_str, void *data);
CH_CAPI_EXPORT int ch_parse_port(ch_arg_parser *parser, char *t_str, void *data);
CH_CAPI_EXPORT void *ch_get_arg(struct ch_arg_parser *parser, char *name);
CH_CAPI_EXPORT void ch_print_help(ch_arg_parser *parser);
CH_CAPI_EXPORT void ch_print_opts(ch_arg_parser *parser);
CH_CAPI_EXPORT int ch_parser_args(struct ch_arg_parser *parser, int argc,
                                  char **argv);
CH_CAPI_EXPORT void ch_add_args(struct ch_arg_parser *parser, struct arg_opt *opt);
CH_CAPI_EXPORT void ch_cleanup_args(struct ch_arg_parser *parser);

#define CH_PARSER(prog, desc, ...)                                             \
  {                                                                            \
    prog, desc, .add_arg = ch_add_args, .parse = ch_parser_args,                \
                .cleanup = ch_cleanup_args, .opts_map = NULL, __VA_ARGS__      \
  }

#define CH_ADD_ARG(parser, opt) parser.add_argument(&parser, opt)

#ifdef __cplusplus
}
#endif

#endif
