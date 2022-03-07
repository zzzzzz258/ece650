#include <stdio.h>
#include <stdlib.h>

struct blockNode {
  size_t size;
  int isFree;
  struct blockNode * next;
  struct blockNode * prev;
};
typedef struct blockNode node;

struct list {
  node * head;
  node * rear;
};
typedef struct list list;

void * ts_malloc_lock(size_t size);
void ts_free_lock(void * ptr);

void * ts_malloc_nolock(size_t size);
void ts_free_nolock(void * ptr);
