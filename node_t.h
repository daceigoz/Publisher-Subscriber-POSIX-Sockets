#ifndef STRUCT_NODE_T
#define STRUCT_NODE_T
#define MAX_SIZE    256
#include <sys/socket.h>

typedef struct node{
  
  struct node * next;
  char topic[MAX_SIZE];
  char addr [INET6_ADDRSTRLEN];
  int port;

} node_t;

#endif
