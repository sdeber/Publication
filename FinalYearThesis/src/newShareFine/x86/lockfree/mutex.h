#include "common.h"
struct __lock_with_counter_t {
  long lock;
  long counter;
};
 
typedef struct _mutex {
  struct __lock_with_counter_t lock_with_counter;
  struct lockfree_fifo * waiting_queue;
} cmutex_t;

inline void GetMutex(cmutex_t * m);
inline void ReleaseMutex(cmutex_t * m);
int cmutex_init(cmutex_t * m);
