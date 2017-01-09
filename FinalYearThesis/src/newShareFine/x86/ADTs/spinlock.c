#include "spinlock.h"
#include "../smash/SharedFine/scheduler.h"

inline void getSpinlock(c_Spinlock * lock)
{
  while(TAS(lock));
}

inline void releaseSpinlock(c_Spinlock * lock)
{
  *lock=FREE;
}
