#ifndef _FIFOQ_H
#define _FIFOQ_H
#define BARBER "/barber"
#define FIFO "/fifo"
#define BARBER_STATUS "/barber_status"
#define SHARED_MEM "/shared_mem"
typedef struct fifo_queue{
  int tail;
  int size;
  pid_t queue[64];
}fifo_queue;

fifo_queue *initialization(fifo_queue *queue, int size);
int push(fifo_queue *queue, pid_t pid);
pid_t pop(fifo_queue *queue);
void qfree(fifo_queue *queue);
void qshow(fifo_queue *queue);
char *get_time();

#endif
