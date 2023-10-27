#include "ch_path.h"
#include "ch_zlib.h"
#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define HEADER_COMMON                                                          \
  "#ifndef _RES_%d_H\n#define _RES_%d_H\n#include <stdlib.h>\n#include "       \
  "\"compile.h\"\n"
#define HEADER_END "\n#endif"
#define RES_STRUCT_FMT "  {\"%s\", CH_RES_%s, %lu, %lu, CH_RES_COMPRESS_%s},\n"
static FILE *output;

typedef struct compile_arg {
  char *input;
  char *output;
  char *compress;
} c_args;
#define ADD_ARGS(name, alias)                                                  \
  if (strcmp(argv[i], "-" #name) == 0 || strcmp(argv[i], "--" #alias) == 0) {  \
    args->alias = argv[++i];                                                   \
  }
#define ELSE_ADD_ARGS(name, alias) else ADD_ARGS(name, alias)

void parse_args(int argc, char *argv[], c_args *args) {
  for (int i = 1; i < argc; ++i) {
    ADD_ARGS(i, input)         // input
    ELSE_ADD_ARGS(o, output)   // output
    ELSE_ADD_ARGS(c, compress) // compress
  }
}
#define CHECK_OPEN(f)                                                          \
  do {                                                                         \
    if ((f) == NULL) {                                                         \
      fprintf(stderr, "Could not open %s\n", #f);                              \
      exit(1);                                                                 \
    }                                                                          \
  } while (0)

void convert_name(char *str) {
  size_t len = strlen(str);
  for (size_t i = 0; i < len; i++) {
    if (isalpha(str[i])) {
      str[i] = toupper(str[i]);
    } else if (str[i] == '.') {
      str[i] = '_';
    }
  }
}
struct ch_compile_res {
  const char *name;
  char *macro_name;
  unsigned long len;
  unsigned long compress_len;
  char *compress_name;
};

struct Node {
  struct ch_compile_res *data;
  struct Node *next;
};

static struct Node *head;
// head insert
void insert_node(struct Node **head, struct ch_compile_res *data) {
  struct Node *node = malloc(sizeof(struct Node));
  node->data = data;
  node->next = *head;
  *head = node;
}
void free_link(struct Node *node) {
  while (node != NULL) {
    struct Node *tmp = node;
    if (tmp->data) {
      free(tmp->data->macro_name);
      free(tmp->data);
      tmp->data = NULL;
    }
    node = node->next;
    free(tmp);
  }
}

void convert_file(const char *input_file, FILE *output, const char *compress) {
  FILE *input = fopen(input_file, "rb");
  CHECK_OPEN(input);
  fseek(input, 0, SEEK_END);
  unsigned long file_size = ftell(input);
  rewind(input);
  char *data = (char *)malloc(file_size);
  fread(data, 1, file_size, input);
  char *c_data = NULL;
  uLong output_size = file_size;
  char *ctype = "NONE";
  if (compress && strcasecmp(compress, "ZLIB") == 0) {
    ch_zlib_compress(data, file_size, &c_data, &output_size);
    ctype = "ZLIB";
  }

  char *input_name = ch_get_file_no_ext(input_file);
  convert_name(input_name);
  fprintf(output, "#define CH_RES_%s  \"", input_name);

  if (c_data) {
    // compress
    for (int i = 0; i < output_size; ++i) {
      fprintf(output, "\\x%02X", (unsigned char)c_data[i]);
    }
  } else {
    // raw
    for (int i = 0; i < file_size; ++i) {
      fprintf(output, "\\x%02X", (unsigned char)data[i]);
    }
  }
  fprintf(output, "\"\n");
  struct ch_compile_res *res = malloc(sizeof(struct ch_compile_res));
  memset(res, 0, sizeof(struct ch_compile_res));
  res->name = input_file;
  res->macro_name = input_name;
  res->len = file_size;
  res->compress_len = output_size;
  res->compress_name = ctype;
  insert_node(&head, res);
  free(data);
  if (c_data) {
    free(c_data);
  }
  fclose(input);
}

int loop_callback(const char *path, const char *name, int flags,
                  void *user_data) {
  c_args *args = (c_args *)user_data;
  printf("convert %s\n", path);
  convert_file(path, output, args->compress);
  return 0;
}

int main(int argc, char *argv[]) {
  srand(time(NULL));
  int rnd_version = rand() % INT_MAX;
  c_args args = {};
  parse_args(argc, argv, &args);
  if (args.input == NULL || args.output == NULL) {
    fprintf(stderr, "Usage: %s -i input_file -o output_file\n", argv[0]);
    exit(1);
  }
  head = NULL;
  // output is global
  output = fopen(args.output, "w");
  CHECK_OPEN(output);
  fprintf(output, HEADER_COMMON, rnd_version, rnd_version);
  if (ch_is_dir(args.input)) {
    ch_loop_dir(args.input, loop_callback, &args);
  } else {
    convert_file(args.input, output, args.compress);
  }

  fprintf(output, "\nstruct ch_res_t CH_COMPILE_RES[] = {\n");
  struct Node *cur = head;
  while (cur) {
    fprintf(output, RES_STRUCT_FMT, cur->data->name, cur->data->macro_name,
            cur->data->len, cur->data->compress_len, cur->data->compress_name);
    cur = cur->next;
  }
  fprintf(output, "\t{NULL}\n");
  fprintf(output, "};\n");

  free_link(head);
  fprintf(output, HEADER_END);
  fclose(output);
  printf("Conversion completed successfully.\n");
  return 0;
}
