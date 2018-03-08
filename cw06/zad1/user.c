#include "protocol.h"
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

int user_queueid;
int service_queueid;
int userid;

void shutdown(void){
  msgctl(user_queueid, IPC_RMID, NULL);
}

void sigint_handler(int sig){
  exit(0);
}

void send_msg(struct msgbuf msg){
  msg.userid = userid;
  if(msgsnd(service_queueid, &msg, MSGSZ, 0)){
    printf("Can't connect with server. Most likely server is already shut down.\n");
    exit(1);
  }
}

void receive_msg(void){
  struct msgbuf msg;
  msgrcv(user_queueid, &msg, MSGSZ, 0, 0);

  switch(msg.mtype){

    case ACCEPT:
      userid = msg.userid;
      printf("Service sent ACCEPT. My given id = %d.\n", msg.userid);
      break;

    case DECLINE:
      printf("Service sent DECLINE. Most likely maximum amount of clients reached already. Have to wait for a spot.\n");
      /// safer to just end it here, user can try connecting later
      exit(0);
      break;

    case SERVICE_RESPONSE:
      printf("Service sent RESPONSE containing message: %s\n", msg.message);
  }
}

int main(void){
  ///initialization
  struct msgbuf msg;
  atexit(shutdown);
  signal(SIGINT, sigint_handler);
  service_queueid = msgget(ftok(PATH, PROJID), 0);
  if(service_queueid == -1){
    printf("Could not open service queue. Something went wrong.\n");
    exit(1);
  }

  user_queueid = msgget(IPC_PRIVATE, 0600); /// ipcprivate used when we give msgget value to other processes

  printf("User has started running.\n");

  /// start working
  /// sending first request to start communicating with service
  msg.mtype = REQUEST;
  msg.userpid = getpid();
  msg.userqid = user_queueid;
  send_msg(msg);

  /// get RESPONSE
  receive_msg();

  /// user commands handler
  printf("Available services :\n"
            " ECHO - send text to service, it will respond with sent text\n"
            " TOUPPER - send text to service, it will change all lowercase to uppercase\n"
            " TIME - service responds with date and time\n"
            " TERMINATE - service ends after doing all other requests from queue\n");
  char command[DEFAULT_TEXT_SIZE];
  while(1){
    scanf("%s", command);
    if (!strcmp(command, "ECHO")) {
        msg.mtype = ECHO;
        scanf("%s", msg.message);
        send_msg(msg);
    }
    else if (!strcmp(command, "TOUPPER")) {
            msg.mtype = TOUPPER;
            scanf("%s", msg.message);
            send_msg(msg);
        }
        else if (!strcmp(command, "TIME")) {
                msg.mtype = TIME;
                send_msg(msg);
            }
            else if (!strcmp(command, "TERMINATE")) {
                    msg.mtype = TERMINATE;
                    send_msg(msg);
                    printf("User has sent TERIMNATE, shutting down.\n");
                    exit(0);
                }
                else {
                    printf("No such command avilable: %s\n", command);
                    continue;
                }
  /// get RESPONSE
    receive_msg();
  }
}
