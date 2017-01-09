typedef unsigned long int32;
inline unsigned char CAS(int32 * current, int32 old_value, int32 new_value);
inline unsigned char CAS2(int32 * current, int32 old_value,int32 old_counter, int32 new_value, int32 new_counter);
inline unsigned char TAS(unsigned char * location);
inline long Swap(int32 * location, int32 value);
inline long FetchAndAdd(long * location, long num);
