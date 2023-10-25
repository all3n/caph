#include "ch_logging.h"
void set_log_level(const char *log_level_str) {
  if (log_level_str != NULL) {
    if (strcasecmp(log_level_str, "DEBUG") == 0) {
      LOG_LEVEL = XLOG_DEBUG;
    } else if (strcasecmp(log_level_str, "INFO") == 0) {
      LOG_LEVEL = XLOG_INFO;
    } else if (strcasecmp(log_level_str, "WARNING") == 0) {
      LOG_LEVEL = XLOG_WARNING;
    } else if (strcasecmp(log_level_str, "ERROR") == 0) {
      LOG_LEVEL = XLOG_ERROR;
    } else {
      LOG_LEVEL = atoi(log_level_str);
    }
  } else {
    LOG_LEVEL = XLOG_INFO;
  }
}
int get_log_level() {
  if (LOG_LEVEL == -1) {
    char *log_debug = getenv("DEBUG");
    if (log_debug != NULL && (strcasecmp(log_debug, "1") == 0 ||
                              strcasecmp(log_debug, "true") == 0)) {
      LOG_LEVEL = XLOG_DEBUG;
      return LOG_LEVEL;
    }
    char *log_level_str = getenv("LOG_LEVEL");
    set_log_level(log_level_str);
  }
  return LOG_LEVEL;
}
