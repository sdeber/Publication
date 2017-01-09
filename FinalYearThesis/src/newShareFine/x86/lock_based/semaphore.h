#include "../ADTs/Fifo.h"
typedef struct _lockbased_csemaphore {
  c_Spinlock lock;
  int counter;
  struct simple_fifo * waiting_queue;
} lockbased_csem ;

int lockbased_csem_init(lockbased_csem * s,int init_counter);
inline int lockbased_csem_wait(lockbased_csem *s);
inline int lockbased_csem_signal(lockbased_csem * s);
