
#include <ch_opts.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
struct EmbArgs {
  int a;
  int b;
};
typedef struct TestOpts {
  bool help;
  bool verbose;
  bool bVal;
  const char *host;
  int port;
  const char *log_level;
  struct EmbArgs *emb_args;
} TestOpts;

void free_emb_args(void *data) {
  struct EmbArgs **e_args = (struct EmbArgs **)data;
  free(*e_args);
  *e_args = NULL;
}
int ch_parse_emb_args(ChArgParser *parser, char *t_str, void *data) {
  struct EmbArgs **e_args = (struct EmbArgs **)data;
  *e_args = (struct EmbArgs *)malloc(sizeof(struct EmbArgs));
  char *t = strchr(t_str, ':');
  if (t) {
    *t = '\0';
    t++;
  }
  (*e_args)->a = atoi(t_str);
  (*e_args)->b = atoi(t);
  return 0;
}
void ch_print_emb_args(void *data) {
  struct EmbArgs *e_args = *(struct EmbArgs **)data;
  printf("%d:%d", e_args->a, e_args->b);
}

int main(int argc, char **argv) {
  TestOpts opts = {.host = "127.0.0.1", .port = 8888, .log_level = "DEBUG"};
  ChArgParser parser = CH_PARSER(argv[0], "this is arg parser test");
  parser.AddArg(&parser, ARG_BOOL(h, help));
  parser.AddArg(&parser, ARG_BOOL(v, verbose));
  parser.AddArg(&parser, ARG_BOOL(b, bVal));
  parser.AddArg(&parser, ARG_STR(H, host, .func = ch_parse_host,
                                 .required = true, .desc = "host name"));
  parser.AddArg(&parser,
                ARG_INT(p, port, .func = ch_parse_port, .desc = "port number"));
  parser.AddArg(&parser, ARG_STR(l, log_level, .desc = "log level"));

  parser.AddArg(&parser,
                ARG_OBJ(&opts.emb_args, e, emb_args, .func = ch_parse_emb_args,
                        .arg_print = ch_print_emb_args,
                        .arg_free = free_emb_args, .desc = "emb_args"));
  if (parser.Parse(&parser, argc, argv)) {
    printf("%s\n", parser.error_msg);
    ch_print_help(&parser);
    parser.Cleanup(&parser);
    return -1;
  }
  ch_print_opts(&parser);
  parser.Cleanup(&parser);
  return 0;
}
