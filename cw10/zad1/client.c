#define _DEFAULT_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>
#include <memory.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/un.h>
#include <arpa/inet.h>

#include "header.h"

#define PATH_MAX 108
char name[MAX_NAME];
in_port_t port;
char *ip_address;
char path_unix_so[PATH_MAX];
int mode;
int sock = -1;

void ending(){
  if(sock != -1){
    if(shutdown(sock, SHUT_RDWR) == -1){
      perror("Failed to shut the sock down");
    }

    if(close(sock) == -1){
      perror("Failed to close the sock");
    }
    sock = -1;
  }
}

void sighandler(int signum){
  ending();
  exit(0);
}

void help(){
  printf("Usage for client:\n name mode[1 - unix client, 0 - tcp client] IP/UNIX_PATH PORT/nothing");
}

void initialize_unix_socket(){
  sock = socket(AF_UNIX, SOCK_STREAM, 0);
  if(sock == -1){
    perror("Failed in creating unix sock");
    exit(1);
  }

  struct sockaddr_un address;
  memset(&address, 0, sizeof(address));
  address.sun_family = AF_UNIX;
  strncpy(address.sun_path, path_unix_so, PATH_MAX);
  connect(sock, (const struct sockaddr *) &address, sizeof(address));
}

void initialize_inet_socket(){
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if(sock == -1){
    perror("Failed in creating internet sock");
    exit(1);
  }

  struct sockaddr_in address;
  memset(&address, 0, sizeof(address));
  address.sin_family = AF_INET;
  address.sin_port = htons(port);
  inet_aton(ip_address, &address.sin_addr);
  connect(sock, (const struct sockaddr *) &address, sizeof(address));
}

void evaluate(task_type task){
  message_holder msg;
  msg.type = 2; /// result

  result_type result;
  result.jobid = task.jobid;
  switch(task.action){
    case 1: /// add
      result.result = task.a + task.b;
      break;
    case 2:
      result.result = task.a - task.b;
      break;
    case 3:
      result.result = task.a * task.b;
      break;
    case 4:
      result.result = task.a / task.b;
      break;
  }
  msg.message.result = result;
  if(send(sock, &msg, sizeof(msg), 0) == -1){
    perror("Failed to send result");
  }
}

void send_introduction(){
  message_holder msg;
  msg.type = 0; /// 0 == HELLO
  strncpy(msg.message.name, name, MAX_NAME);
  if(send(sock, &msg, sizeof(msg), 0) == -1){
    perror("Failure in sending introduction to server");
  }
}
int main(int argc, char **argv){

    if(argc != 4 && argc != 5){
      help();
      return 0;
    }
    atexit(ending);
    signal(SIGINT, sighandler);

    /// setting global variables
    mode = atoi(argv[2]);
    if(mode){
      if(argc != 4){
        help();
        exit(1);
      }
      /// copy path
      strncpy(path_unix_so, argv[3], PATH_MAX);
      path_unix_so[PATH_MAX -1] = '\0';
    }
    else{
      if(argc != 5){
        help();
        exit(1);
      }
      ip_address = argv[3];
      port = (in_port_t) atoi(argv[4]);
    }

    if(mode){
      initialize_unix_socket();
    }
    else{
      initialize_inet_socket();
    }

    /// send introduction to server to get recognized
    strncpy(name, argv[1], MAX_NAME);
    name[MAX_NAME - 1] = '\0';
    send_introduction();
    while(1){
      message_holder msg;
      ssize_t count = recv(sock, &msg, sizeof(msg), MSG_WAITALL);
      if(count == -1){
        if(errno != EAGAIN){
          perror("Recv error");
          return 1;
        }
      }
      else if(count == 0){
          return 0;
        }
        else if(count < sizeof(msg)){
          continue;
        }
      switch(msg.type){
        case 1: /// TASK
          printf("Recieved task from server\n");
          fflush(stdout);
          evaluate(msg.message.task);
          break;
        case 3: /// PING
          printf("Recieved ping from server, im still alive !\n");
          fflush(stdout);
          break;
        case 5: // KILL SIGNAL
          return 0;
        default:
          break;
      }
    }

}
