#include "spinlock.h"
#include "../smash/SharedFine/scheduler.h"
struct fifo_node {
  void * data;
  struct fifo_node * next;
};

struct simple_fifo {
  struct fifo_node * head;
  struct fifo_node * tail;
};

struct duallock_fifo {
  c_Spinlock head_lock,tail_lock;
  struct fifo_node * head, * tail;
};

// This structure is only used by lockfree algorithms
struct _pointer_t {
  struct fifo_node * p; //po
  long counter;
};

struct lockfree_fifo {
  struct _pointer_t head,tail;
};

int simple_fifo_init(struct simple_fifo * queue);
int simple_fifo_enqueue(void * data, struct simple_fifo * queue);
void * simple_fifo_dequeue(struct simple_fifo * queue);
void simple_fifo_destroy(struct simple_fifo * queue);


int duallock_fifo_init(struct duallock_fifo * queue);
int duallock_fifo_enqueue(void * data, struct duallock_fifo * queue);
void * duallock_fifo_dequeue(struct duallock_fifo * queue);
void duallock_fifo_destroy(struct duallock_fifo * queue);


int lockfree_fifo_init(struct lockfree_fifo * queue);
int lockfree_fifo_enqueue(void * data, struct lockfree_fifo * queue);
void * lockfree_fifo_dequeue(struct lockfree_fifo * queue);
