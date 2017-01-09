#include "../ADTs/generic_fifo.h"

struct message_queue {
  cthread * waiting_thread;
  c_Spinlock lock;
  struct simple_fifo * messages;
};

int message_queue_init(struct message_queue * queue);
int send_message(void * message, struct message_queue * queue);
void * fetch_message(struct message_queue * queue);
