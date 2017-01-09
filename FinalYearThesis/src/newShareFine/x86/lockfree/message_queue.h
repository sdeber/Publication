#include "../ADTs/generic_fifo.h"
#define WAITING 1
#define NOWAITING 0

struct lockfree_message_queue {
  struct lockfree_fifo * messages;
  cthread * waiting_thread;
  unsigned long waiting_mark;
};

int lockfree_message_queue_init(struct lockfree_message_queue * queue);
int send_message(void * content, struct lockfree_message_queue * queue);
void * fetch_message(struct lockfree_message_queue * queue);
