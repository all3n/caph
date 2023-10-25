#include "ch_web.h"
#include <stdlib.h>
int main(int argc, char *argv[]) {
  ch_web web = MAKE_CH_WEB(.port = 9999);
  ch_web_init(&web);
  // ch_web_start(&web);
  ch_web_destory(&web);
  return EXIT_SUCCESS;
}
