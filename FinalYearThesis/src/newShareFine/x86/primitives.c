#include "primitives.h"
inline unsigned char CAS(int32 * current, int32 old_value, int32 new_value)
{
  unsigned char register success;
  asm volatile("lock\n\t"
	       "cmpxchgl %3,%1\n\t"
	       "sete %0\n\t"
	       :"=r"(success)
	       :"m"(*current),"a"(old_value),"q"(new_value)
	       :"cc","memory");
  return success;
}

inline unsigned char CAS2(int32 * current, int32 old_value, int32 old_counter, int32 new_value, int32 new_counter)
{
  unsigned char success;
  asm volatile("lock\n\t"
	       "cmpxchg8b %1\n\t"
	       "sete %0\n\t"
	       :"=q"(success)
	       :"m"(*current),"a"(old_value),"d"(old_counter),"b"(new_value),"c"(new_counter));
}

inline unsigned char TAS(unsigned char * location)
{
  unsigned char register success=1;
  asm volatile("lock\n\t"
	       "xchgb %0,%1\n\t"
	       :
	       :"r"(success),"m"(*location)
	       :"cc","memory");
  return success;
} 

inline long Swap(int32 * location, int32 value)
{
  long register result=value;
  asm volatile("lock\n\t"
	       "xchg %0,%1\n\t"
	       :
	       :"m"(*location),"b"(result)
	       :"cc","memory");
  return result;  
}

inline long FetchAndAdd(long * location, long num)
{
  long result=0;
  asm volatile("lock\n\t"
	       "xadd %2,%1\n\t"
	       :"=a"(result)
	       :"m"(*location),"a"(num));
  return result;
}
