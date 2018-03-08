#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdlib.h>

/// setting consant variables for use in code
#define DEFAULT_TEXT_SIZE 128
#define MAX_USERS 16
/// projid argument for msgget function, has to be a char, lowest 7 bites used
#define PROJID 4
/// generated on "HOME"
#define PATH getenv("HOME")
/// size of message used in various functions, has to be
#define MSGSZ sizeof(struct msgbuf) /// going to cast the structure to a char required in mq_send, this time it will include the long mtype
/// new parameter required for the biggest size of queue, required in posix standard
#define MQSZ 8 /// HAS TO BE LOWER THAN 10, becasue it must be lower than default msg_max limit which is 10
/// stucture used for messaging
struct msgbuf{
  long mtype;
  pid_t userpid;
  int userid;
  char message[DEFAULT_TEXT_SIZE]; /// arrays have to be static
};

enum message_type{
  REQUEST = 1,
  ACCEPT,
  DECLINE,
  ECHO,
  TOUPPER,
  TIME,
  TERMINATE,
  SERVICE_RESPONSE,
  /// required in this part
  USER_LOGOUT
};

#endif
