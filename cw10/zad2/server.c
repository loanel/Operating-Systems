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
#include <sys/epoll.h>
#include "header.h"

#define PATH_MAX 108
#define MAX_EVENTS 128
#define MAX_CLIENTS 15


typedef struct{
  struct sockaddr addr;
  socklen_t len;
  int fd;
  char name[MAX_NAME];
} client_structure;

in_port_t port;
pthread_t networking_thread;
pthread_t ping_thread;
int inet_socket = -1;
int unix_socket = -1;
char path_unix_so[PATH_MAX];
struct epoll_event events[MAX_EVENTS];
int efd = -1;

int clientholder_size = -1;
client_structure clientholder[MAX_CLIENTS];

pthread_mutex_t clientholder_mtx = PTHREAD_MUTEX_INITIALIZER;

void ending(){
  message_holder msg;
  msg.type = 5;
  for(int i = 0; i <= clientholder_size; i++){
    sendto(clientholder[i].fd, &msg, sizeof(msg), 0, &clientholder[i].addr, clientholder[i].len);
  }
  pthread_cancel(networking_thread);
  pthread_join(networking_thread, NULL);
  close(efd);
  efd = -1;
  if(unix_socket != -1){
    shutdown(unix_socket, SHUT_RDWR);
    close(unix_socket);
    unlink(path_unix_so);
    unix_socket = -1;
  }
  if(inet_socket != -1){
    shutdown(inet_socket, SHUT_RDWR);
    close(inet_socket);
    inet_socket = -1;
  }
  pthread_mutex_destroy(&clientholder_mtx);
}

void sighandler(int signum){
  ending();
  exit(0);
}


void help(){
  printf("Usage : ./server PORT UNIX_PATH");
}


int make_socket_non_blocking(int sfd) {
    int flags, s;

    flags = fcntl(sfd, F_GETFL, 0);
    if (flags == -1) return -1;

    flags |= O_NONBLOCK;
    s = fcntl(sfd, F_SETFL, flags);
    if (s == -1) return -1;

    return 0;
}

/// MUTEX LOCKED FUNCTIONS FOR SAFETY
void add_client(client_structure client) {
  pthread_mutex_lock(&clientholder_mtx);
  if (clientholder_size >= MAX_CLIENTS) {
      perror("Too many clients in table");
      pthread_mutex_unlock(&clientholder_mtx);
      exit(1);
    }
  clientholder[++clientholder_size] = client;
  pthread_mutex_unlock(&clientholder_mtx);
}

int find_random_client(void) {
  pthread_mutex_lock(&clientholder_mtx);
  if (clientholder_size == -1) {
    pthread_mutex_unlock(&clientholder_mtx);
    return -1;
  }
  else{
    int index = rand() % (clientholder_size + 1);
    pthread_mutex_unlock(&clientholder_mtx);
    return index;
  }
}

void close_client(client_structure client) {
  pthread_mutex_lock(&clientholder_mtx);
  for (int i = 0, j = 0; i < clientholder_size; i++) {
      if (memcmp(&(client.addr), &(clientholder[i].addr), client.len) == 0){
          message_holder msg;
          msg.type = 5;
          if (sendto(client.fd, &msg, sizeof(msg), 0, &client.addr, client.len) == -1){
              perror("Failed to close the client");
          }
          for(j = i ; j <= clientholder_size -1 ; j++){
            clientholder[j] = clientholder[j+1];
          }
      }
  }
  clientholder_size--;
  pthread_mutex_unlock(&clientholder_mtx);
}
//----------------------------------------------------------
void initialize_unix_socket(){
  unix_socket = socket(AF_UNIX, SOCK_DGRAM, 0);
  if(unix_socket == -1){
    perror("Failed in creating unix socket");
    exit(1);
  }

  struct sockaddr_un address;
  memset(&address, 0, sizeof(address));
  address.sun_family = AF_UNIX;
  strncpy(address.sun_path, path_unix_so, PATH_MAX);
  unlink(path_unix_so);
  bind(unix_socket, (const struct sockaddr *) &address, sizeof(address));
}

void initialize_inet_socket(){
  inet_socket = socket(AF_INET, SOCK_DGRAM, 0);
  if(inet_socket == -1){
    perror("Failed in creating internet socket");
    exit(1);
  }

  struct sockaddr_in address;
  memset(&address, 0, sizeof(address));
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(port);
  bind(inet_socket, (const struct sockaddr *) &address, sizeof(address));
}

void* ping_clients(void* args){
  while(1){
    message_holder msg;
    msg.type = 3;
    for(int i = 0; i <= clientholder_size; i++){
      sendto(clientholder[i].fd, &msg, sizeof(msg), 0, &clientholder[i].addr, clientholder[i].len);
    }
    sleep(3);
  }
}

void *networking_function(void *args){

  initialize_inet_socket();
  initialize_unix_socket();
  make_socket_non_blocking(inet_socket);
  make_socket_non_blocking(unix_socket);
  listen(inet_socket, 15);
  listen(unix_socket, 15);
  efd = epoll_create1(0);

  struct epoll_event event;

  event.data.fd = inet_socket;
  event.events = EPOLLIN | EPOLLRDHUP;
  epoll_ctl(efd, EPOLL_CTL_ADD, inet_socket, &event);

  event.data.fd = unix_socket;
  epoll_ctl(efd, EPOLL_CTL_ADD, unix_socket, &event);

  if ((errno = pthread_create(&ping_thread, NULL, ping_clients, NULL)) != 0){
     perror("ping thread creation error");
     exit(1);
   }

  while(1){
    int n = epoll_wait(efd, events, MAX_EVENTS, -1);
    fflush(stdout);
    for(int i = 0; i < n; i++){
      event = events[i];
      if ((event.events & EPOLLERR) || (event.events & EPOLLHUP) || (!(event.events & EPOLLIN))) {
              continue;
      }
      else{
          while(1){
            fflush(stdout);
            message_holder msg;
            struct sockaddr addr;
            socklen_t len;
            len = sizeof(addr);
            client_structure client;
            ssize_t count = recvfrom(event.data.fd, &msg, sizeof(msg), MSG_WAITALL, &addr, &len);
            if(count == -1){
              if(errno != EAGAIN){
                perror("Recv error");
                client.addr = addr;
                client.len = len;
                client.fd = event.data.fd;
                close_client(client);
              }
              break;
            }
            else if(count == 0){
                client.addr = addr;
                client.len = len;
                client.fd = event.data.fd;
                close_client(client);
                break;
              }
              else if(count < sizeof(msg)){
                    perror("Recieved only a part of the message \n");
                    continue;
                  }
                  else{
                      switch(msg.type){
                        case 2: /// RESULT
                          printf("Output[%d] = %d\n", msg.message.result.jobid, msg.message.result.result);
                          break;
                        case 0: /// introduction
                          client.addr = addr;
                          client.len = len;
                          client.fd = event.data.fd;
                          add_client(client);
                          printf("%s connected\n", msg.message.name);
                          break;
                        case 5: // KILL SIGNAL
                          client.addr = addr;
                          client.len = len;
                          client.fd = event.data.fd;
                          close_client(client);
                          break;
                        default:
                          break;
                        }
            }
          }
        }
      }
    }
}

int main(int argc, char **argv) {

    srand((unsigned int) time(NULL));

    if (argc != 3){
      help();
      return 0;
    }
    atexit(ending);
    signal(SIGINT, sighandler);

    /// setting variables
    port = (in_port_t) atoi(argv[1]);
    strncpy(path_unix_so, argv[2], PATH_MAX);
    path_unix_so[PATH_MAX - 1] = '\0';

    pthread_create(&networking_thread, NULL, networking_function, NULL);
    printf("number a b : 1 = +, 2 = -, 3 = *, 4 = /, \n5 = exit\n");
    fflush(stdout);

    int jobid = 0;
    while(1){
      task_type task;
      task.jobid = jobid;
      int action;
      printf("%d : ", jobid);
      fflush(stdout);
      if(scanf("%d", &action) == EOF) break;
      if(action == 5) return 0;
      if(action > 5 || action < 1){
        printf("Wrong action inputed\n");
        continue;
      }
      task.action = action;
      if(scanf("%d %d", &(task.a), &(task.b)) == EOF) break;
      message_holder msg;
      msg.type = 1;
      msg.message.task = task;
      int index = find_random_client();
      printf("Randomed index %d\n", index);
      if(index == -1){
         printf("No clients available\n");
         continue;
      }

      if(sendto(clientholder[index].fd, &msg, sizeof(msg), 0, &clientholder[index].addr, clientholder[index].len) == -1) {
         perror("Failed to send task ");
      }
      jobid++;
    }
  return 0;
}
