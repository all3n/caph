#include "ch_zlib.h"
#include <stdio.h>
#include <stdlib.h>
#ifdef __APPLE__ // macOS
#define LIB_ZLIB "libz.dylib"
#include <dlfcn.h>
#elif defined(_WIN32) // Windows
#include <windows.h>
#define LIB_ZLIB "zlib1.dll"
#elif defined(__linux__) // Linux
#define LIB_ZLIB "libz.so.1"
#include <dlfcn.h>
#else
#define LIB_ZLIB "libz.so"
#include <dlfcn.h>
#endif

#define CHUNK_SIZE 1024

#define CH_deflateInit(strm, level)                                            \
  f_deflateInit_((strm), (level), ZLIB_VERSION, (int)sizeof(z_stream))

#define CH_inflateInit(strm)                                                   \
  f_inflateInit_((strm), ZLIB_VERSION, (int)sizeof(z_stream))

static int(*f_deflateInit_)
    OF((z_streamp strm, int level, const char *version, int stream_size));
static uLong(*f_deflateBound) OF((z_streamp strm, uLong sourceLen));
static int(*f_deflate) OF((z_streamp strm, int flush));
static int(*f_deflateEnd) OF((z_streamp strm));

static int(*f_inflate) OF((z_streamp strm, int flush));
static int(*f_inflateInit_)
    OF((z_streamp strm, const char *version, int stream_size));
static int(*f_inflateEnd) OF((z_streamp strm));

static void load_library() {
  printf("load library %s\n", LIB_ZLIB);
  void *zlibHandle = dlopen(LIB_ZLIB, RTLD_LAZY);
  if (zlibHandle) {
    f_deflateInit_ = (int (*)(z_streamp, int, const char *, int))dlsym(
        zlibHandle, "deflateInit_");
    f_deflate = (int (*)(z_streamp, int))dlsym(zlibHandle, "deflate");
    f_deflateBound =
        (uLong(*)(z_streamp, uLong))dlsym(zlibHandle, "deflateBound");
    f_deflateEnd = (int (*)(z_streamp))dlsym(zlibHandle, "deflateEnd");

    f_inflateInit_ = (int (*)(z_streamp, const char *, int))dlsym(
        zlibHandle, "inflateInit_");
    f_inflate = (int (*)(z_streamp, int))dlsym(zlibHandle, "inflate");

    f_inflateEnd = (int (*)(z_streamp))dlsym(zlibHandle, "inflateEnd");
    dlclose(zlibHandle);
  } else {
    fprintf(stderr, "Unable to load zlib library\n");
    exit(1);
  }
}
int ch_zlib_compress(const char *input, uLong input_size, char **output,
                     uLong *output_size) {
  if (f_deflateInit_ == NULL) {
    load_library();
  }
  z_stream stream;
  int ret;

  // 初始化z_stream结构
  stream.zalloc = Z_NULL;
  stream.zfree = Z_NULL;
  stream.opaque = Z_NULL;
  ret = CH_deflateInit(&stream, Z_DEFAULT_COMPRESSION);
  if (ret != Z_OK) {
    return ret;
  }
  // 设置输入数据
  stream.next_in = (Bytef *)input;
  stream.avail_in = input_size;
  // 创建输出缓冲区
  uLong max_output_size = f_deflateBound(&stream, input_size);
  *output = (char *)malloc(max_output_size);
  if (*output == NULL) {
    f_deflateEnd(&stream);
    return Z_MEM_ERROR;
  }
  *output_size = 0;
  // 压缩数据
  do {
    uLong chunk_size =
        (stream.avail_in < CHUNK_SIZE) ? stream.avail_in : CHUNK_SIZE;
    stream.avail_out = CHUNK_SIZE;
    stream.next_out = (Bytef *)(*output + *output_size);
    ret = f_deflate(&stream, Z_FINISH);
    if (ret != Z_OK && ret != Z_STREAM_END) {
      free(*output);
      f_deflateEnd(&stream);
      return ret;
    }
    *output_size += CHUNK_SIZE - stream.avail_out;
  } while (stream.avail_out == 0);
  f_deflateEnd(&stream);
  return Z_OK;
}

int ch_zlib_decompress(const char *input, uLong input_size, char **output,
                       uLong *output_size) {
  if (f_deflateInit_ == NULL) {
    load_library();
  }
  z_stream stream;
  int ret;
  // 初始化z_stream结构
  stream.zalloc = Z_NULL;
  stream.zfree = Z_NULL;
  stream.opaque = Z_NULL;
  stream.avail_in = 0;
  stream.next_in = Z_NULL;
  ret = CH_inflateInit(&stream);
  if (ret != Z_OK) {
    return ret;
  }
  // 设置输入数据
  stream.next_in = (Bytef *)input;
  stream.avail_in = input_size;
  // 创建输出缓冲区
  *output = (char *)malloc(CHUNK_SIZE);
  if (*output == NULL) {
    f_inflateEnd(&stream);
    return Z_MEM_ERROR;
  }
  *output_size = 0;
  // 解压缩数据
  do {
    stream.avail_out = CHUNK_SIZE;
    stream.next_out = (Bytef *)(*output + *output_size);
    ret = f_inflate(&stream, Z_NO_FLUSH);
    if (ret != Z_OK && ret != Z_STREAM_END) {
      free(*output);
      f_inflateEnd(&stream);
      return ret;
    }
    *output_size += CHUNK_SIZE - stream.avail_out;
  } while (stream.avail_out == 0);
  f_inflateEnd(&stream);
  return Z_OK;
}
#undef LIB_ZLIB
