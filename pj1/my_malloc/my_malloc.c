#include "my_malloc.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// declare global variables of lists
node * usedList = null;
node * freeList = null;

void * ff_malloc(size_t size) {
  // find the right(first fit free) block to use
  node * curr = freeList;
  while (curr != null) {
    if (curr)
  }
  // cannot find any free block to use, request for new space
}

void ff_free(void *ptr);

void * bf_malloc(size_t size);
void bf_free(void *ptr);

size_t get_data_segment_size();
size_t get_free_space_segment_size();
