#ifndef CW10_HEADER_H
#define CW10_HEADER_H

#define MAX_NAME 10


/// we will need a structure that will be sent between sockets
/// union works wonders here
typedef struct{
  int action;
  int a;
  int b;
  int jobid;
}task_type;

typedef struct{
  int result;
  /// which job
  int jobid;
}result_type;

typedef struct{
  int type;
  union{
    char name[MAX_NAME];
    task_type task;
    result_type result;
  } message;
}message_holder;

#endif
