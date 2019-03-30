#ifndef STRUCT_NODE_T
#define STRUCT_NODE_T
#define MAX_SIZE    256

typedef struct node{
  struct node * next;
  char topic[MAX_SIZE];
  struct sockaddr *addr;
  //Add IP and Socket addr of the subscriber in this node.
} node_t;

#endif
