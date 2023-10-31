#include "ch_path.h"
#include "tests_common.h"
#include <stdio.h>
#include <stdlib.h>
#include <unity.h>

static char *data_path;
void setUp(void) {}
void tearDown(void) {}
int path_callback(const char *path, const char *sub_path, const char *name,
                  int flags, void *user_data) {
  printf("%s:%s\n", path, name);
  return 0;
}

void path_joins_tests() {
  // basic join
  ch_as out = ch_path_join("/home/user/test", "fasdf", "afsd", NULL);
  TEST_ASSERT_EQUAL_STRING("/home/user/test/fasdf/afsd", out.str);
  // test with slash
  ch_as out2 = ch_path_join("/home/user/test/", "fasdf", "afsd", NULL);
  TEST_ASSERT_EQUAL_STRING("/home/user/test/fasdf/afsd", out2.str);
  // test with slash
  ch_as out3 = ch_path_join("home/user/test/", "fasdf", "afsd", NULL);
  TEST_ASSERT_EQUAL_STRING("home/user/test/fasdf/afsd", out3.str);
}
void file_name_tests() {
  const char *path1 = "/data/fadsf/aaa.txt";
  char *file_name = ch_get_file_name(path1);
  TEST_ASSERT_EQUAL_STRING("aaa.txt", file_name);
  free(file_name);
}
void file_name_no_ext_tests() {
  const char *path1 = "/data/fadsf/aaa.txt";
  char *file_name = ch_get_file_no_ext(path1);
  TEST_ASSERT_EQUAL_STRING("aaa", file_name);
  free(file_name);
}
void loop_dir_tests() {
  int ret1 = ch_loop_dir("not_exists_dir", NULL, path_callback, NULL);
  TEST_ASSERT_EQUAL(-1, ret1);
  int ret = ch_loop_dir(data_path, NULL, path_callback, NULL);
  TEST_ASSERT_EQUAL(0, ret);
}

int main(int argc, char *argv[]) {
  data_path = test_data(argv[0], "data/path_test");
  UNITY_BEGIN();
  RUN_TEST(path_joins_tests);
  RUN_TEST(file_name_tests);
  RUN_TEST(file_name_no_ext_tests);
  RUN_TEST(loop_dir_tests);
  free(data_path);
  return UNITY_END();
}
