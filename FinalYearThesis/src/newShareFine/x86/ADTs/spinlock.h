#include "../primitives.h"
#define FREE 0
#define LOCKED 1
typedef unsigned char  c_Spinlock;
inline void getSpinlock(c_Spinlock * lock);
inline void releaseSpinlock(c_Spinlock * lock);
