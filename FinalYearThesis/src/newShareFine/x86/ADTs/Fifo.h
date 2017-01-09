#include "spinlock.h"
#include "../smash/SharedFine/scheduler.h"

struct fifo_node {
  void * data;
  struct fifo_node * next;
};

struct simple_fifo {
  node * head;
  node * tail;
};

struct duallock_fifo {
  c_Spinlock head_lock,tail_lock;
  struct fifo_node * head, * tail;
};

// This structure is only used by lockfree algorithms
struct _pointer_t {
  node * p; //po
  int32 counter;
};

struct lockfree_fifo {
  struct _pointer_t head,tail;
};

int simple_fifo_init(struct simple_fifo * queue);
int simple_fifo_enqueue(cthread * data, struct simple_fifo * queue);
void * simple_fifo_dequeue(struct simple_fifo * queue);
void simple_fifo_destroy(struct simple_fifo * queue);


int duallock_fifo_init(struct duallock_fifo * queue);
int duallock_fifo_enqueue(void * data, struct duallock_fifo * queue);
void * duallock_fifo_dequeue(struct duallock_fifo * queue);
void duallock_fifo_destroy(struct duallock_fifo * queue);


int lockfree_fifo_init(struct lockfree_fifo * queue);
int lockfree_fifo_enqueue(cthread * thread, struct lockfree_fifo * queue);
cthread * lockfree_fifo_dequeue(struct lockfree_fifo * queue);
