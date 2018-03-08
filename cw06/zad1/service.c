#include "protocol.h"
#include <signal.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <ctype.h>

int queueid;
int user_queues[MAX_USERS];
int usercounter = 0;

void shutdown(void){
  msgctl(queueid, IPC_RMID, NULL);
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
  for(pointer = a; *pointer != '\0'; ++pointer){
    *pointer = toupper(*pointer);
  }
}

void message_handle(void){
  struct msgbuf msg;
  msgrcv(queueid, &msg, MSGSZ, 0 ,0);

  switch(msg.mtype){

    case REQUEST:
      printf("User PID = %d sent REQUEST.\n", msg.userpid);
      if(usercounter < MAX_USERS){
        user_queues[usercounter] = msg.userqid;
        msg.mtype = ACCEPT;
        msg.userid = usercounter;
        msgsnd(msg.userqid, &msg, MSGSZ, 0);
        printf("User PID = %d accepted, recieved id = %d.\n", msg.userpid, msg.userid);
        usercounter++;
      }
      else{
        msg.mtype = DECLINE;
        msgsnd(msg.userqid, &msg, MSGSZ, 0);
        printf("User PID = %d declined, maximum amount of users reached.\n", msg.userpid);
      }
      break;

    case ECHO:
      printf("User ID = %d wants to use ECHO service.\n", msg.userid);
      msg.mtype = SERVICE_RESPONSE;
      msgsnd(user_queues[msg.userid], &msg, MSGSZ, 0);
      break;

    case TOUPPER:
      printf("User ID = %d wants to use TOUPPER service.\n", msg.userid);
      msg.mtype = SERVICE_RESPONSE;
      to_upper(msg.message);
      msgsnd(user_queues[msg.userid], &msg, MSGSZ, 0);
      break;

    case TIME:
      printf("User ID = %d wants to use TIME service.\n", msg.userid);
      msg.mtype = SERVICE_RESPONSE;
      gettime(msg.message);
      msgsnd(user_queues[msg.userid], &msg, MSGSZ, 0);
      break;

    case TERMINATE:
      printf("User ID = %d wants to use TERMINATE service.\n", msg.userid);
      exit(0);
  }
}

int main(void){
  ///initialization
  queueid = msgget(ftok(PATH, PROJID), IPC_CREAT | 0600);
  atexit(shutdown);
  signal(SIGINT, sigint_handler);
  printf("Service has started running...\n");
  /// infinite loop of recieving messages for server runtime
  while(1) message_handle();
}
