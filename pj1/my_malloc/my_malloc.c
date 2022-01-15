#include "my_malloc.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// declare global variables of lists
list usedList = {NULL, NULL};
list freeList = {NULL, NULL};
int meta = sizeof(node);

void initNode(node * newSpace, size_t size) {
  newSpace->next = NULL;
  newSpace->prev = NULL;
  newSpace->size = size;
}

node * mergeTwoNodes(node * first, node * second, list * l) {
  if (second == l->rear) {
    l->rear = first;
  }
  first->size += (meta + second->size);
  first->next = second->next;
  return first;
}

void checkAndMerge(node * curr, list * l) {
  // check prev
  if (curr->prev != NULL &&
      (void *)curr == (void *)curr->prev + curr->prev->size + meta) {
    curr = mergeTwoNodes(curr->prev, curr, l);
  }
  // check next
  if (curr->next != NULL && (void *)curr + curr->size + meta == (void *)curr->next) {
    mergeTwoNodes(curr, curr->next, l);
  }
}

/*
  insert a node into a list
  consider merging problem if list is freeList
*/
void insert(node * target, list * l) {
  if (l->head == NULL || l->head > target) {
    // insert to the first position
    target->next = l->head;
    if (l->head != NULL) {
      l->head->prev = target;
    }
    else {
      l->rear = target;
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
    else {
      l->rear = target;
    }
    target->prev = curr;
    curr->next = target;
  }
}

/*
  append a node to the end of a list
*/
void appendNode(node * target, list * l) {
  node * rear = l->rear;
  if (rear == NULL) {
    l->head = target;
    l->rear = target;
  }
  else {
    target->prev = rear;
    rear->next = target;
    l->rear = target;
  }
}

/*
  remove node from a list
*/
void removeNode(node * curr, list * l) {
  if (curr->prev != NULL) {
    curr->prev->next = curr->next;
  }
  else {
    l->head = curr->next;
  }
  if (curr->next != NULL) {
    curr->next->prev = curr->prev;
  }
  else {
    l->rear = curr->prev;
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
  appendNode(newSpace, &usedList);
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
    node * newBorn = (node *)((void *)target + size + meta);
    initNode(newBorn, target->size - size - meta);
    insert(newBorn, &freeList);
    checkAndMerge(newBorn, &freeList);
  }
  target->size = size;
  insert(target, &usedList);
  return target;
}

void * real_malloc(size_t size, node * match) {
  if (match != NULL) {
    // remove curr from free space list
    removeNode(match, &freeList);
    // split curr and put them into seperate lists
    node * allocated = splitBlock(match, size);
    return (void *)allocated + meta;
  }
  // cannot find any free block to use, request for new space
  node * newSpace = requestNewSpace(size);
  //return the start of real data (not including metadata)
  return (void *)newSpace + meta;
}

void * ff_malloc(size_t size) {
  // find the right(first fit free) block to use
  node * curr = freeList.head;
  node * match = NULL;
  // fetch first
  while (curr != NULL) {
    if (curr->size >= size) {
      match = curr;
      break;
    }
    curr = curr->next;
  }
  return real_malloc(size, match);
}

void * bf_malloc(size_t size) {
  // find the smallest block that bigger than size
  node * curr = freeList.head;
  node * match = NULL;
  while (curr != NULL) {
    if (curr->size >= size && (match == NULL || curr->size < match->size)) {
      match = curr;
    }
    curr = curr->next;
  }
  return real_malloc(size, match);
}

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
  //printf("Node not found in list!!!\n");
  return NULL;
}

/*
  real free function to free a selected block
*/
void freeNode(node * curr) {
  // find this in usedlist
  if (NULL != findInList(&usedList, curr)) {
    removeNode(curr, &usedList);
    insert(curr, &freeList);
    checkAndMerge(curr, &freeList);
  }
}

/*
  ptr is a pointer to real data, which follows metadata
*/
void ff_free(void * ptr) {
  freeNode((node *)(ptr - meta));
}

void bf_free(void * ptr) {
  freeNode((node *)(ptr - meta));
}

/*
  sum spaces of a list, including meta
*/
unsigned long sum(list * l) {
  node * curr = l->head;
  unsigned long s = 0;
  while (curr != NULL) {
    s += (meta + curr->size);
    curr = curr->next;
  }
  return s;
}

unsigned long get_data_segment_size() {
  return sum(&usedList) + sum(&freeList);
}
unsigned long get_free_space_segment_size() {
  return sum(&freeList);
}
