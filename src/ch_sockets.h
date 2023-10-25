#ifndef _CH_SOCKETS_H
#define _CH_SOCKETS_H

#include "ch_macro.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_CLIENTS 32


typedef struct {
  int sockfd;
  struct sockaddr_in addr;
} Socket;

typedef struct {
  Socket sockets[MAX_CLIENTS];
  int count;
  pthread_mutex_t mutex;
} SocketPool;

typedef struct {
  SocketPool *pool;
  int index;
} ThreadArg;

#define ERR_EXIT(func, msg)                                                    \
  do {                                                                         \
    if ((func) == -1) {                                                        \
      XLOG(ERROR, msg);                                                        \
      exit(EXIT_FAILURE);                                                      \
    }                                                                          \
  } while (0)
#define ERR_BREAK(func, msg)                                                   \
  if ((func) == -1) {                                                          \
    XLOG(ERROR, msg);                                                          \
    break;                                                                     \
  }

// server socket
// bind -> listen -> accept
CH_CAPI_EXPORT int socket_bind(Socket *socket, int port);
CH_CAPI_EXPORT int socket_listen(Socket *socket, int backlog);
CH_CAPI_EXPORT int socket_accept(Socket *server_socket, Socket *client_socket);
// client socket
// connect -> send -> recv
CH_CAPI_EXPORT int socket_connect(Socket *socket, const char *server_ip, int server_port);

// common
CH_CAPI_EXPORT int socket_create(Socket *s);
CH_CAPI_EXPORT ssize_t socket_nsend(Socket *socket, const char *data, int n);
CH_CAPI_EXPORT ssize_t socket_send(Socket *socket, const char *data);
CH_CAPI_EXPORT ssize_t socket_receive(Socket *socket, char *buffer, size_t buffer_size);
CH_CAPI_EXPORT void socket_close(Socket *socket);

// pool
CH_CAPI_EXPORT void socket_pool_init(SocketPool *pool);
CH_CAPI_EXPORT void socket_pool_add(SocketPool *pool, int sockfd, struct sockaddr_in addr);
CH_CAPI_EXPORT void socket_pool_remove(SocketPool *pool, int index);

#ifdef __cplusplus
}
#endif

#endif
