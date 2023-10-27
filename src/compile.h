#ifndef _CH_RES_COMPILE_H
#define _CH_RES_COMPILE_H
enum ch_res_compress_type {
  CH_RES_COMPRESS_NONE = 0,
  CH_RES_COMPRESS_GZIP = 1,
  CH_RES_COMPRESS_ZLIB = 2,
  CH_RES_COMPRESS_BZIP2 = 3,
  CH_RES_COMPRESS_LZ4 = 4,
};
struct ch_res_t {
  const char *name;
  const char *value;
  long len;
  unsigned int compress_len;
  enum ch_res_compress_type compress_type;
};

#endif
