#include "my_malloc.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// declare global variables of lists
list usedList = {NULL};
list freeList = {NULL};
int meta = sizeof(node *);

void initNode(node * newSpace, size_t size) {
  newSpace->next = NULL;
  newSpace->prev = NULL;
  newSpace->size = size;
}

node * mergeTwoNodes(node * first, node * second) {
  first->size += (meta + second->size);
  first->next = second->next;
  return first;
}

void checkAndMerge(node * curr) {
  // check prev
  if (curr->prev != NULL && curr == curr->prev + curr->prev->size + meta) {
    curr = mergeTwoNodes(curr->prev, curr);
  }
  // check next
  if (curr->next != NULL && curr + curr->size + meta == curr->next) {
    mergeTwoNodes(curr, curr->next);
  }
}

/*
  insert a node into a list
  consider merging problem if list is freeList
  ifUsedList: 1 for usedList, 0 for freelist
*/
void insert(node * target, list * l, int ifUsedList) {
  if (l->head == NULL || l->head > target) {
    // insert to the first position
    target->next = l->head;
    if (l->head != NULL) {
      l->head->prev = target;
    }
    l->head = target;
  }
  else {
    node * curr = l->head;
    while (curr->next != NULL && curr->next < target) {
      curr = curr->next;
    }
    // insert after curr
    if (curr->next != NULL) {
      target->next = curr->next;
      curr->next->prev = target;
    }
    target->prev = curr;
    curr->next = target;
    // merge adjacent blocks for free list
    if (0 == ifUsedList) {
      checkAndMerge(target);
    }
  }
}

/*
  remove node from a list
*/
void removeNode(node * curr) {
  if (curr->prev != NULL) {
    curr->prev->next = curr->next;
  }
  if (curr->next != NULL) {
    curr->next->prev = curr->prev;
  }
  curr->next = NULL;
  curr->prev = NULL;
}

/*
  request new space of size (size + metadata) from system
*/
node * requestNewSpace(size_t size) {
  // request new space
  node * newSpace = (node *)sbrk(size + meta);
  initNode(newSpace, size);
  // add to usedlist
  insert(newSpace, &usedList, 1);
  return newSpace;
}

/*
  split a block node
  insert first part into usedlist
  insert second part node into freelist
  return: first node
*/
node * splitBlock(node * target, size_t size) {
  if (target->size > size + meta) {
    node * newBorn = target + size + meta;
    initNode(newBorn, target->size - size - meta);
    insert(newBorn, &freeList, 0);
  }
  target->size = size;
  insert(target, &usedList, 1);
  return target;
}

void * ff_malloc(size_t size) {
  // find the right(first fit free) block to use
  node * curr = freeList.head;
  // fetch first
  while (curr != NULL) {
    if (curr->size >= size) {
      // remove curr from free space list
      removeNode(curr);
      // split curr and put them into seperate lists
      node * allocated = splitBlock(curr, size);
      return allocated + meta;
    }
    curr = curr->next;
  }
  // cannot find any free block to use, request for new space
  node * newSpace = requestNewSpace(size);
  //return the start of real data (not including metadata)
  return newSpace + meta;
}

void * bf_malloc(size_t size);

/*
  This function actually only checks if target is in list l
  If in, return target itself, is not return NULL
*/
node * findInList(list * l, node * target) {
  node * curr = l->head;
  while (curr != NULL) {
    if (curr == target) {
      return curr;
    }
    curr = curr->next;
  }
  printf("Node not found in list!!!\n");
  return NULL;
}

/*
  real free function to free a selected block
*/
void freeNode(node * curr) {
  // find this in usedlist
  if (NULL != findInList(&usedList, curr)) {
    removeNode(curr);
    insert(curr, &freeList, 0);
  }
}

/*
  ptr is a pointer to real data, which follows metadata
*/
void ff_free(void * ptr) {
  freeNode((node *)(ptr - meta));
}

void bf_free(void * ptr);

size_t get_data_segment_size();
size_t get_free_space_segment_size();
