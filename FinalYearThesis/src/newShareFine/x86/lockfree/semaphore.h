#include "common.h"
struct c_semaphore {
  long int counter;
  struct lockfree_fifo * waiting_queue;
};
int csem_init(struct c_semaphore * sem, int init_count);
int csem_wait(struct c_semaphore * sem);
int csem_signal(struct c_semaphore * sem);
