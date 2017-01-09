#include "../ADTs/spinlock.h"
#include "../smash/SharedFine/scheduler.h"
struct __channel {
  cthread * communicator;
};

typedef struct __channel channel;

struct __channel_set {
  channel ** chan_array;
  int chan_num;
  c_Spinlock c_lock;
};

typedef struct __channel_set channel_set;

int channel_set_init(channel_set * chan_set, channel * chan[], int chan_num);
inline void channel_in_alt(channel_set * chan_set, void * message, int length);
inline void channel_out_alt(channel_set * chan_set, int chan_index, void * message, int length);

