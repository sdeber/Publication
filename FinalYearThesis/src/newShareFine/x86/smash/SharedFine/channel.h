#ifndef __CHANNEL_H
#define __CHANNEL_H

#include "scheduler.h"

// channel implememtation with locks
// mainly for debugging
#define LOCKFREE

typedef struct {
   cthread * communicator;
#ifndef LOCKFEE
   int clock; // lock on channel
#endif
} channel;

channel * channel_init();
extern void channel_in(channel * chan, void * message, int length);
extern void channel_out(channel * chan, void * message, int length);

#endif
