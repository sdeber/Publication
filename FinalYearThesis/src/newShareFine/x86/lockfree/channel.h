#include "../smash/SharedFine/scheduler.h"
#include "../primitives.h"

struct __waitfree_channel {
  cthread * communicator;
};

typedef struct __waitfree_channel channel;

void channel_out(channel * chan, void * message, int length);
void channel_in(channel * chan, void * message, int lenght);
void channel_in_alt(channel ** chan_array, int chan_num, void * message, int length);
