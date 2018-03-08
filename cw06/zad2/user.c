#include "protocol.h"
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <mqueue.h>
#include <unistd.h>

int user_queueid;
int service_queueid;
int userid;
int ENDFLAG = 0;
/// holder for each new qname that will be built using build_queue
char qname[DEFAULT_TEXT_SIZE];

mqd_t build_queue(void){
  struct mq_attr attributes;
  attributes.mq_maxmsg = MQSZ;
  attributes.mq_msgsize = MSGSZ;
  /// build the queue name, will be using userpid
  sprintf(qname, "/userq%d", getpid());
  mqd_t helper = mq_open(qname, O_CREAT | O_RDONLY, 0600, &attributes);
  if(helper == -1){
    printf("Can't open user queue, something went wrong.\n");
    exit(1);
  }
  return helper;
}

void sigint_handler(int sig){
  exit(0);
}

void send_msg(struct msgbuf msg){
  msg.userid = userid;
  if(access("/dev/mqueue/service_queue", W_OK)==-1 && ENDFLAG == 0){
    printf("Can't access service queue, which means that either it has already terminated or service has not started working yet, exiting.\n");
    ENDFLAG = 1;
    exit(1);
  }
  if(mq_send(service_queueid, (char*)&msg, MSGSZ, 0)){
    printf("Can't connect with server. Most likely server is already shut down.\n");
    exit(1);
  }

}

void shutdown(void){
   struct msgbuf msg;
   msg.mtype = USER_LOGOUT;
   send_msg(msg);
   mq_close(user_queueid);
   mq_close(service_queueid);
   mq_unlink(qname);
}

void recieve_msg(void){
  struct msgbuf msg;
  mq_receive(user_queueid, (char*)&msg, MSGSZ, NULL);

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
  service_queueid = mq_open("/service_queue", O_WRONLY);
  if(service_queueid == -1){
    printf("Could not open service queue. Something went wrong.\n");
    exit(1);
  }

  user_queueid = build_queue();

  printf("User has started running.\n");

  /// start working
  /// sending first request to start communicating with service
  msg.mtype = REQUEST;
  msg.userpid = getpid();
  strcpy(msg.message, qname); /// sending qname as message in the first signal so service knows what to open
  send_msg(msg);

  /// get RESPONSE
  recieve_msg();

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
    recieve_msg();
  }
}
