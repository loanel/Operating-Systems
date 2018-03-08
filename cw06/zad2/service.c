#include "protocol.h"
#include <signal.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <mqueue.h>
#include <unistd.h>

int queueid;
int user_queues[MAX_USERS];
int usercounter = 0;
/// name of the service POSIX queue, starts with /
const char qname[] = "/service_queue";

mqd_t build_queue(void){
  struct mq_attr attributes;
  attributes.mq_maxmsg = MQSZ;
  attributes.mq_msgsize = MSGSZ;
  mqd_t helper = mq_open(qname, O_RDONLY | O_CREAT, 0600, &attributes);
  if(helper == -1){
    perror("Can't open service queue, something went wrong ");
    exit(1);
  }
  return helper;
}

void shutdown(void){
  int i = 0;
  for(i = 0; i < usercounter; ++i){
    mq_close(user_queues[i]);/// closing all queues before shutdown
  }
  mq_close(queueid);
  mq_unlink(qname);
}

void sigint_handler(int sig){
  exit(0);
}

void gettime(char *a){
  time_t tholder;
  struct tm *timehelper; //structure used to convert to localtime
  time(&tholder);
  timehelper = localtime(&tholder);
  strcpy(a, asctime(timehelper));
}

void to_upper(char *a){
  char *pointer;
  for(pointer = a; *pointer != '\0'; pointer++){
    *pointer = toupper(*pointer);
  }
}

void message_handle(void){
  struct msgbuf msg;
  mq_receive(queueid, (char*)&msg, MSGSZ, NULL);

  switch(msg.mtype){

    case REQUEST:
      printf("User PID = %d sent REQUEST.\n", msg.userpid);
      if(usercounter < MAX_USERS){
        user_queues[usercounter] = mq_open(msg.message, O_WRONLY);
        msg.mtype = ACCEPT;
        msg.userid = usercounter;
        mq_send(user_queues[usercounter], (char*)&msg, MSGSZ, 0);
        printf("User PID = %d accepted, recieved id = %d.\n", msg.userpid, msg.userid);
        usercounter++;
      }
      else{
        msg.mtype = DECLINE;
        /// open exisiting user q to send him the DECLINE message
        int user_queueid = mq_open(msg.message, O_WRONLY);
        mq_send(user_queueid, (char*)&msg, MSGSZ, 0);
        mq_close(user_queueid);
        printf("User PID = %d declined, maximum amount of users reached.\n", msg.userpid);
      }
      break;

    case ECHO:
      printf("User ID = %d wants to use ECHO service.\n", msg.userid);
      msg.mtype = SERVICE_RESPONSE;
      mq_send(user_queues[msg.userid], (char*)&msg, MSGSZ, 0);
      break;

    case TOUPPER:
      printf("User ID = %d wants to use TOUPPER service.\n", msg.userid);
      msg.mtype = SERVICE_RESPONSE;
      to_upper(msg.message);
      mq_send(user_queues[msg.userid], (char*)&msg, MSGSZ, 0);
      break;

    case TIME:
      printf("User ID = %d wants to use TIME service.\n", msg.userid);
      msg.mtype = SERVICE_RESPONSE;
      gettime(msg.message);
      mq_send(user_queues[msg.userid], (char*)&msg, MSGSZ, 0);
      break;

    case USER_LOGOUT:
      printf("User ID = %d wants to use LOGOUT service.\n", msg.userid);
      mq_close(user_queues[usercounter]);
      break;

    case TERMINATE:
      printf("User ID = %d wants to use TERMINATE service.\n", msg.userid);
      exit(0);
  }
}

int main(void){
  ///initialization
  queueid = build_queue();
  atexit(shutdown);
  signal(SIGINT, sigint_handler);
  printf("Service has started running...\n");
  /// infinite loop of recieving messages for server runtime
  while(1) message_handle();
}
