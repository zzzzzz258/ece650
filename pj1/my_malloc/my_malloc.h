#include <stdio.h>
#include <stdlib.h>

struct blockNode {
  size_t size;
  struct blockNode * next;
  struct blockNode * prev;
};
typedef struct blockNode node;

struct list {
  node * head;
  node * rear;
};
typedef struct list list;

void * ff_malloc(size_t size);
void ff_free(void * ptr);

void * bf_malloc(size_t size);
void bf_free(void * ptr);

unsigned long get_data_segment_size();
unsigned long get_free_space_segment_size();
