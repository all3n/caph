#ifndef _CH_TIME_H
#define _CH_TIME_H
#include "ch_macro.h"
#include <stdint.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

struct ch_time_metrics {
  clock_t start_time;
  clock_t end_time;
  double total_time;
  uint64_t total_count;
  uint64_t memory;
  uint64_t alloc;
};
CH_CAPI_EXPORT void ch_time_tik(struct ch_time_metrics *metrics);
CH_CAPI_EXPORT void ch_time_toc(struct ch_time_metrics *metrics);

#ifdef __cplusplus
}
#endif

#endif
