#ifndef _CH_JSON_H
#define _CH_JSON_H
#include <json-c/json.h>

#ifdef __cplusplus
extern "C" {
#endif
void free_json_obj(struct json_object **data) {
  if (*data == NULL) {
    return;
  }
  struct json_object *obj = *(struct json_object **)data;
  if (obj != NULL) {
    json_object_put(obj);
  }
}

#define auto_json __attribute__((cleanup(free_json_obj))) json_object *

void json_set_s(json_object *obj, const char *key, const char *value) {
  json_object_object_add(obj, key, json_object_new_string(value));
}
void json_set_i(json_object *obj, const char *key, int value) {
  json_object_object_add(obj, key, json_object_new_int(value));
}

void json_set_l(json_object *obj, const char *key, int64_t value) {
  json_object_object_add(obj, key, json_object_new_int64(value));
}

void json_set_b(json_object *obj, const char *key, int value) {
  json_object_object_add(obj, key, json_object_new_boolean(value));
}

void json_set_o(json_object *obj, const char *key, json_object *value) {
  json_object_object_add(obj, key, value);
}

void json_array_add(json_object *obj, json_object *a) {
  json_object_array_add(obj, a);
}
void json_array_addi(json_object *obj, int v) {
  json_object_array_add(obj, json_object_new_int(v));
}

void json_array_addl(json_object *obj, int64_t v) {
  json_object_array_add(obj, json_object_new_int64(v));
}

void json_array_adds(json_object *obj, const char *v) {
  json_object_array_add(obj, json_object_new_string(v));
}

#ifdef __cplusplus
}
#endif
#endif
