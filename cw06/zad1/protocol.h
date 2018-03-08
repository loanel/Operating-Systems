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
#define MSGSZ (sizeof(struct msgbuf) - sizeof(long))/// message size doesnt include mtype field, which is a long

/// stucture used for messaging
struct msgbuf{
  long mtype;
  pid_t userpid;
  int userid;
  int userqid;
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
};

#endif
