#include "ch_time.h"

void ch_time_tik(struct ch_time_metrics *metrics){
  metrics->start_time = clock();

}
void ch_time_toc(struct ch_time_metrics *metrics){
  metrics->end_time = clock();
  metrics->total_time += (double)(metrics->end_time - metrics->start_time) / CLOCKS_PER_SEC;
}
