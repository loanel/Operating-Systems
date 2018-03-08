#include <time.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include "fifoq.h"

fifo_queue *initialization(fifo_queue *queue, int size){
  if(queue == NULL){
    queue = malloc(sizeof(fifo_queue));
  }
  queue->tail = -1;
  queue->size = size;
  return queue;
}

int push(fifo_queue *queue, pid_t pid){
	if(queue == NULL){
		printf("Queue doesn't exist");
		return -1;
	}
	if (queue->tail == queue->size - 1){
		return -1;
	}
	queue->tail++;
	queue->queue[queue->tail] = pid;
	return 0;
}

pid_t pop(fifo_queue *queue){
	if(queue == NULL)
		return -1;
	if(queue->tail == -1){
		get_time();
		printf("Queue is empty\n");
		return -1;
	}
	if(queue->tail == 0){
		return queue->queue[queue->tail--];
	}
	pid_t pid = queue->queue[0];
	int i;
	for (i = 0; i<queue->tail; i++)
		queue->queue[i] = queue->queue[i+1];
	queue->queue[queue->tail] = -1;
	queue->tail = queue->tail - 1;
	return pid;
}

void qfree(fifo_queue *queue){
	if(queue == NULL)
		return;
	if(queue->queue != NULL)
		free(queue->queue);
	free(queue);
}

void qshow(fifo_queue *queue){
	int i;
	for(i = 0; i<=queue->tail; i++)
		printf("%d - %d\n", i+1, queue->queue[i]);
}

/// addinational time function required

char *get_time(){
  struct timespec *timehelper;
  timehelper = malloc(sizeof(struct timespec));
  if(clock_gettime(CLOCK_MONOTONIC, timehelper) == -1){
    perror("Failed to read time. %s\n");
  }
  double timer = timehelper->tv_sec;
  timer += timehelper->tv_nsec/1000000000.0;
  printf("%f ", timer);
  return 0;
}
