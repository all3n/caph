#include "ch_opts.h"
#include "ch_logging.h"
#include "ch_string.h"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int ch_parse_host(ChArgParser *parser, char *t_str, void *data) {
  *(char **)data = t_str;
  return 0;
}
int ch_parse_port(ChArgParser *parser, char *t_str, void *data) {
  int port = atoi(t_str);
  if (port <= 0 || port > 65535) {
    ch_append_fmt(&parser->error_msg, "Invalid port: %s (0,65535)\n", t_str);
    return -1;
  }
  *(int *)data = port;
  return 0;
}
const char *arg_type_to_str(ArgOptType t) {
  switch (t) {
  case kArgString:
    return "string";
  case kArgInt:
    return "int";
  case kArgLong:
    return "long";
  case kArgFloat:
    return "float";
  case kArgBool:
    return "bool";
  case kArgEnum:
    return "enum";
  default:
    return "unknown";
  }
}

void ch_print_help(ChArgParser *parser) {
  fprintf(stderr, "Usage: %s [options]\n%s", parser->prog, parser->description);
  fprintf(stderr, "\n");
  fprintf(stderr, "Options:\n");
  size_t idx = 0;
  void *item = NULL;
  while (hashmap_iter(parser->opts_map, &idx, &item)) {
    ArgOpt *opt = (ArgOpt *)item;
    fprintf(stderr, "  -%s,--%s %s %s", opt->s_name, opt->l_name,
            arg_type_to_str(opt->type),
            opt->required ? "required" : "optional");
    if (opt->desc != NULL) {
      fprintf(stderr, "    %s", opt->desc);
    }
    fprintf(stderr, "\n");
  }
  fprintf(stderr, "\n");
}
void ch_print_opts(ChArgParser *parser) {
  size_t idx = 0;
  void *item = NULL;
  printf("{\n");
  while (hashmap_iter(parser->opts_map, &idx, &item)) {
    ArgOpt *opt = (ArgOpt *)item;
    printf("  %s:", opt->l_name);
    if (opt->arg_print) {
      opt->arg_print(opt->data);
    } else {

      switch (opt->type) {
      case kArgString:
        printf("\"%s\"", *(char **)opt->data);
        break;
      case kArgInt:
        printf("%d", *(int *)opt->data);
        break;
      case kArgLong:
        printf("%ld", *(long *)opt->data);
        break;
      case kArgFloat:
        printf("%f", *(float *)opt->data);
        break;
      case kArgBool:
        printf("%s", *(bool *)opt->data ? "true" : "false");
        break;
      case kArgEnum:
        printf("%d", *(int *)opt->data);
        break;
      default:
        break;
      }
    }
    printf("\n");
  }
  printf("}\n");
}

static int ch_set_opt_value(ChArgParser *parser, ArgOpt *opt, char *value) {
  if (opt->func) {
    int ret = opt->func(parser, value, opt->data);
    opt->set_flag = true;
    if (ret) {
      ch_append_fmt(&parser->error_msg, "parse func fail:%s:%s fail\n",
                    opt->l_name, value);
    }
    return ret;
  }
  switch (opt->type) {
  case kArgString:
    *(const char **)opt->data = value;
    break;
  case kArgInt:
    *(int *)opt->data = atoi(value);
    break;
  case kArgLong:
    *(long *)opt->data = atol(value);
    break;
  case kArgFloat:
    *(double *)opt->data = atof(value);
    break;
  case kArgBool:
    *(bool *)opt->data =
        strcasecmp(value, "true") == 0 || strcmp(value, "1") == 0;
    break;
  case kArgEnum:
    *(int *)opt->data = atoi(value);
    break;
  default:
    break;
  }
  opt->set_flag = true;
  return 0;
}
int ch_parser_args(struct ChArgParser *parser, int argc, char **argv) {
  ArgOpt *opt = NULL;
  ArgAlias *alias = NULL;
  char *name, *eqchr, *value;
  for (int i = 0; i < argc; ++i) {
    value = NULL;
    alias = NULL;
    name = NULL;
    opt = NULL;
    char *arg = argv[i];
    if (arg[0] == '-') {
      name = arg[1] == '-' ? arg + 2 : arg + 1;
      eqchr = strchr(name, '=');
      if (eqchr) {
        *eqchr = '\0';
        value = eqchr + 1;
      }
      if (strlen(name) == 1) {
        alias = (struct ArgAlias *)hashmap_get(
            parser->alias, &(struct ArgAlias){.name = name});
        if (!alias) {
          ch_append_fmt(&parser->error_msg, "short %s is invalid", alias->name);
          return -1;
        }
        name = alias->alias;
      }
      opt = (struct ArgOpt *)hashmap_get(parser->opts_map,
                                         &(struct ArgOpt){.l_name = name});
      if (!opt) {
        ch_append_fmt(&parser->error_msg, "Unknown option: %s", name);
        return -1;
      }
      if (value) {
        if (ch_set_opt_value(parser, opt, value)) {
          XLOG(ERROR, "parser: %s:%s fail", opt->l_name, value);
          return -1;
        }
      } else if (opt->type == kArgBool) {
        *(bool *)opt->data = true;
        opt->set_flag = true;
      } else if (i == argc - 1 || argv[i + 1][0] == '-') {
        fprintf(stderr, "Option %s requires an argument\n", name);
        return -1;
      } else {
        if (ch_set_opt_value(parser, opt, argv[i + 1])) {
          XLOG(ERROR, "%s:%s fail", opt->l_name, value);
          return -1;
        }
        i++;
      }
    } else if ((eqchr = strchr(arg, '=')) != NULL) {
      name = arg;
      if (strlen(name) == 1) {
        alias = (struct ArgAlias *)hashmap_get(
            parser->alias, &(struct ArgAlias){.name = name});
        if (!alias) {
          ch_append_fmt(&parser->error_msg, "short %s is invalid\n",
                        alias->name);
          return -1;
        }
        name = alias->alias;
      }
      *eqchr = '\0';
      value = eqchr + 1;
      opt = (struct ArgOpt *)hashmap_get(parser->opts_map,
                                         &(struct ArgOpt){.l_name = name});
      if (!opt) {
        ch_append_fmt(&parser->error_msg, "Unknown option: %s\n", name);
        return -1;
      }
      if (ch_set_opt_value(parser, opt, value)) {
        XLOG(ERROR, "parser: %s:%s fail", opt->l_name, value);
        return -1;
      }
    }
  }
  size_t iter = 0;
  void *item;
  while (hashmap_iter(parser->opts_map, &iter, &item)) {
    ArgOpt *opt = (ArgOpt *)item;
    if (!opt->set_flag && opt->required) {
      ch_append_fmt(&parser->error_msg, "Option %s/%s is required\n",
                    opt->s_name, opt->l_name);
      return -1;
    }
  }
  void *ph = ch_get_arg(parser, "help");
  if (ph) {
    bool help = *(bool *)ph;
    if (help) {
      ch_print_help(parser);
      return 0;
    }
  }
  return 0;
}
static int arg_compare(const void *a, const void *b, void *udata) {
  const struct ArgOpt *ua = a;
  const struct ArgOpt *ub = b;
  return strcmp(ua->l_name, ub->l_name);
}
static int alias_compare(const void *a, const void *b, void *udata) {
  const struct ArgAlias *ua = a;
  const struct ArgAlias *ub = b;
  return strcmp(ua->name, ub->name);
}

bool arg_iter(const void *item, void *udata) {
  const struct ArgOpt *arg = item;
  XLOG(INFO, "%s,%s ", arg->s_name, arg->l_name);
  return true;
}

static uint64_t arg_hash(const void *item, uint64_t seed0, uint64_t seed1) {
  const struct ArgOpt *arg = item;
  return hashmap_sip(arg->l_name, strlen(arg->l_name), seed0, seed1);
}
static uint64_t alias_hash(const void *item, uint64_t seed0, uint64_t seed1) {
  const struct ArgAlias *a = item;
  return hashmap_sip(a->name, strlen(a->name), seed0, seed1);
}

void ch_add_args(struct ChArgParser *parser, struct ArgOpt *opt) {
  if (parser->opts_map == NULL) {
    parser->opts_map = hashmap_new(sizeof(struct ArgOpt), 0, 0, 0, arg_hash,
                                   arg_compare, NULL, NULL);
    parser->alias = hashmap_new(sizeof(struct ArgAlias), 0, 0, 0, alias_hash,
                                alias_compare, NULL, NULL);
  }
  opt->set_flag = false;
  hashmap_set(parser->opts_map, opt);
  hashmap_set(parser->alias,
              &(ArgAlias){.name = opt->s_name, .alias = opt->l_name});
}

void *ch_get_arg(struct ChArgParser *parser, char *name) {
  struct ArgOpt *opt = (struct ArgOpt *)hashmap_get(
      parser->opts_map, &(struct ArgOpt){.l_name = name});
  if (opt) {
    return opt->data;
  } else {
    return NULL;
  }
}

void ch_cleanup_args(struct ChArgParser *parser) {
  size_t idx = 0;
  void *item = NULL;
  while (hashmap_iter(parser->opts_map, &idx, &item)) {
    ArgOpt *opt = (ArgOpt *)item;
    if (opt->arg_free && opt->data) {
      opt->arg_free(opt->data);
    }
  }
  if (parser->opts_map) {
    hashmap_free(parser->opts_map);
    parser->opts_map = NULL;
  }
  if (parser->alias) {
    hashmap_free(parser->alias);
    parser->alias = NULL;
  }
  if (parser->error_msg) {
    free(parser->error_msg);
    parser->error_msg = NULL;
  }
}
