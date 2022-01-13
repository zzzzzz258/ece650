#include <stdio.h>
#include <stdlib.h>

struct blockNode {
  size_t size;
  struct blockNode * next;
  struct blockNode * prev;
};
typedef struct blockNode node;

void * ff_malloc(size_t size);
void ff_free(void *ptr);

void * bf_malloc(size_t size);
void bf_free(void *ptr);

size_t get_data_segment_size();
size_t get_free_space_segment_size();

