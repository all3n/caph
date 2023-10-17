#ifndef _CH_OPTS_H
#define _CH_OPTS_H
#include "hashmap.h"
#include <stdbool.h>
#include <stddef.h>

struct ArgOpt;
typedef struct ChArgParser {
  char *prog;
  char *description;
  struct hashmap *opts_map;
  struct hashmap *alias;
  char *error_msg;
  void (*AddArg)(struct ChArgParser *parser, struct ArgOpt *opt);
  int (*Parse)(struct ChArgParser *parser, int argc, char **argv);
  void (*Cleanup)(struct ChArgParser *parser);
} ChArgParser;

typedef enum ArgOptType {
  kArgString = 0,
  kArgInt,
  kArgLong,
  kArgFloat,
  kArgBool,
  kArgEnum,
  kArgObj
} ArgOptType;

typedef int (*ParseFunc)(ChArgParser *, char *, void *data);
typedef void (*ArgFunc)(void *data);
typedef struct ArgAlias {
  char *name;
  char *alias;
} ArgAlias;
typedef struct ArgOpt {
  void *data;
  char *s_name;
  char *l_name;
  char *desc;
  ArgOptType type;
  bool required;
  bool set_flag;
  void *def_val;
  ParseFunc func;
  ArgFunc arg_free;
  ArgFunc arg_print;
} ArgOpt;

#define ARG_END                                                                \
  { NULL }

#define ARG_OBOOL(obj, s_name, l_name, ...)                                    \
  &(ArgOpt) { obj, #s_name, #l_name, .type = kArgBool, __VA_ARGS__ }
#define ARG_BOOL(s_name, l_name, ...)                                          \
  &(ArgOpt) { &opts.l_name, #s_name, #l_name, .type = kArgBool, __VA_ARGS__ }

#define ARG_OSTR(obj, s_name, l_name, ...)                                     \
  &(ArgOpt) { obj, #s_name, #l_name, .type = kArgString, __VA_ARGS__ }
#define ARG_STR(s_name, l_name, ...)                                           \
  &(ArgOpt) { &opts.l_name, #s_name, #l_name, .type = kArgString, __VA_ARGS__ }

#define ARG_OINT(obj, s_name, l_name, ...)                                     \
  &(ArgOpt) { obj, #s_name, #l_name, .type = kArgInt, __VA_ARGS__ }
#define ARG_INT(s_name, l_name, ...)                                           \
  &(ArgOpt) { &opts.l_name, #s_name, #l_name, .type = kArgInt, __VA_ARGS__ }

#define ARG_OBJ(obj, s_name, l_name, ...)                                      \
  &(ArgOpt) { obj, #s_name, #l_name, __VA_ARGS__ }

int ch_parse_host(ChArgParser * parser, char *t_str, void *data);
int ch_parse_port(ChArgParser * parser, char *t_str, void *data);
void *ch_get_arg(struct ChArgParser *parser, char *name);
void ch_print_help(ChArgParser *parser);
void ch_print_opts(ChArgParser *parser);
int ch_parser_args(struct ChArgParser *parser, int argc, char **argv);
void ch_add_args(struct ChArgParser *parser, struct ArgOpt *opt);
void ch_cleanup_args(struct ChArgParser *parser);
#define CH_PARSER(prog, desc, ...)                                             \
  {                                                                            \
    prog, desc, .AddArg = ch_add_args, .Parse = ch_parser_args,                \
                .Cleanup = ch_cleanup_args, .opts_map = NULL, __VA_ARGS__      \
  }

#define CH_ADD_ARG(parser, opt) parser.AddArgument(&parser, opt)
#endif
