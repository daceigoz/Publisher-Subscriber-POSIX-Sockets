#ifndef STRUCT_NODE_T
#define STRUCT_NODE_T
#define MAX_SIZE    256

typedef struct node{
  struct node * next;
  char topic[MAX_SIZE];
  char text[MAX_SIZE*4];
} node_t;

#endif
