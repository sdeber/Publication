#include "generic_fifo.h"
#include <stdlib.h>

int simple_fifo_init(struct simple_fifo * queue) {
  struct fifo_node * dummy = (struct fifo_node *)malloc(sizeof(struct fifo_node));
   if(dummy == NULL )
    return -1;
  dummy->next = NULL;
  queue->head=queue->tail=dummy;
  return 0;
}

int simple_fifo_enqueue(void * data, struct simple_fifo * queue)
{
  struct fifo_node * new = (struct fifo_node*)malloc(sizeof(struct fifo_node));
  if(new == NULL)
    return -1;
  new->next = NULL;
  new->data = data;
  queue->tail->next = new;
  queue->tail = new;
  return 0;
}

void * simple_fifo_dequeue(struct simple_fifo * queue)
{
  struct fifo_node * node = queue->head->next;
  if(node != NULL) {
    free(queue->head);
    queue->head = node;
    return node->data;
  }
  //Queue is empty.
  return NULL;
}

void simple_fifo_destroy(struct simple_fifo * queue)
{
  struct fifo_node *next, *tmp_node = queue->head;
  while(tmp_node != NULL)
    {
      next = tmp_node->next;
      free(tmp_node);
      tmp_node = next;
    }
}

int duallock_fifo_init(struct duallock_fifo * queue)
{
struct fifo_node * dummy_node = (struct fifo_node *)malloc(sizeof(struct fifo_node));
 if(dummy_node == NULL)
   return -1;
  queue->head_lock = FREE;
  queue->tail_lock = FREE;
  dummy_node->data=NULL;
  dummy_node->next=NULL;
  queue->head=dummy_node;
  queue->tail=dummy_node;
  return 0;
}

int duallock_fifo_enqueue(void * data, struct duallock_fifo * queue)
{
  struct fifo_node * new = (struct fifo_node*)malloc(sizeof(struct fifo_node));
  if(new == NULL)
    return -1;
  new->data = data;
  new->next = NULL;
  getSpinlock(&queue->tail_lock);
  queue->tail->next=new;
  queue->tail=new;
  releaseSpinlock(&queue->tail_lock);
  return 0;
}

void * duallock_fifo_dequeue(struct duallock_fifo * queue)
{
  struct fifo_node * head,* node=NULL;
  void * result = NULL;
  getSpinlock(&queue->head_lock);
  head=queue->head;
  node=head->next;
  if(node == NULL)
    //queue is empty
    return NULL;
  result = node->data;
  queue->head=node; //Note that result is equal to head->next.
  releaseSpinlock(&queue->head_lock);
  free(head);
  return result;
}

//Lock free fifo
int lockfree_fifo_init(struct lockfree_fifo * queue)
{
  struct fifo_node * dummyhead=(struct fifo_node *)malloc(sizeof (struct fifo_node));
  if(dummyhead==NULL)
    return -1;
  dummyhead->data=NULL; //dummyhead contains no data
  dummyhead->next=NULL;
  queue->head.p=dummyhead;
  queue->head.counter=0;
  queue->tail.p=dummyhead;
  queue->tail.counter=0;
  return 0;
}

int lockfree_fifo_enqueue(void * data, struct lockfree_fifo * queue)
{
  struct fifo_node * new = (struct fifo_node*)malloc(sizeof(struct fifo_node));
  if(new == NULL)
    return -1;
  new->next = NULL;
  new->data = data;
  struct _pointer_t tail;
  while(1)
    {
      tail=queue->tail;
      if(CAS(&((tail.p)->next),NULL,(long)new))
	break;
      else
	CAS2(&(queue->tail),
	     (long)tail.p,tail.counter,
	     (long)(tail.p)->next,tail.counter+1);
    }
  CAS2(&(queue->tail),
       (long)tail.p,tail.counter,
       (long)new,tail.counter+1);
  return 0;
} 

void * lockfree_fifo_dequeue(struct lockfree_fifo * queue)
{
  struct _pointer_t tail,head;
  struct fifo_node * old,* next;
  void * result = NULL;
  while(1)
    {
      head=queue->head;
      tail=queue->tail;
      next=head.p->next;
      if(head.counter==queue->head.counter)
	if(head.p==queue->tail.p) { //both head and tail point to the same node
	  if(next==NULL) //That node is the dummy node, so queue is empty. 
	    return NULL;
	  CAS2(&(queue->tail),
	       (long)head.p,tail.counter,
	       (long)next,tail.counter+1);
	}
	else if(next!=NULL)
	  {
	    result=next->data; //copy the value.
	    old=queue->head.p; //the old dummy head node.
	    if(CAS2(&queue->head,
		    (long)head.p,head.counter,
		    (long)next,head.counter+1))
	      {
		free(old);
		break;
	      }
	  }
    }
  return result;
}

/*We still have memory management issue, the current solution may not be efficient*/
