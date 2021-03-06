#include "my_malloc.h"

#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// declare global variables of lists
//list usedList = {NULL, NULL};
list freeList = {NULL, NULL};
int meta = sizeof(node);
unsigned long segment_size = 0;

// Lock for thread safe
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

// Thread Local Storage free list
__thread list tls_freeList = {NULL, NULL};

/*
  operations directly on node 
*/
void initNode(node * newSpace, size_t size) {
  newSpace->next = NULL;
  newSpace->prev = NULL;
  newSpace->isFree = 1;
  newSpace->size = size;
}

/*
  merge second node to the first one
  knowing that they are adjacent
*/
node * mergeTwoNodes(node * first, node * second, list * l) {
  if (second == l->rear) {
    l->rear = first;
  }
  first->size += (meta + second->size);
  first->next = second->next;
  if (second->next != NULL) {
    second->next->prev = first;
  }
  return first;
}

node * findPositionToInsert(node * target, list * l) {
  if (l->head == NULL || l->head > target) {
    return NULL;
  }
  else {
    node * curr = l->head;
    while (curr->next != NULL && curr->next < target) {
      curr = curr->next;
    }
    return curr;
  }
}

void insertNode(node * target, node * prev, list * l) {
  if (NULL == prev) {
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
    // insert after prev
    if (prev->next != NULL) {
      target->next = prev->next;
      prev->next->prev = target;
    }
    else {
      l->rear = target;
    }
    target->prev = prev;
    prev->next = target;
  }
}

/*
  insert a node into a list
*/
void findPositionAndInsertNode(node * target, list * l) {
  node * prev = findPositionToInsert(target, l);
  insertNode(target, prev, l);
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
  check if curr has adjacent blocks in list l, merge them if yes
 */
node * checkAndMerge(node * curr, list * l) {
  // check prev
  if (curr->prev != NULL &&
      (void *)curr == (void *)curr->prev + curr->prev->size + meta) {
    curr = mergeTwoNodes(curr->prev, curr, l);
  }
  // check next
  if (curr->next != NULL && (void *)curr + curr->size + meta == (void *)curr->next) {
    curr = mergeTwoNodes(curr, curr->next, l);
  }
  return curr;
}

/*
  split a block node
  insert second part node into freelist
  return: first node
*/
node * splitBlock(node * target, size_t size, list * l) {
  // only split if rest block are larger than a meta size
  if (target->size > size + meta) {
    // split out a new block and free it
    node * newBorn = (node *)((void *)target + meta + size);
    initNode(newBorn, target->size - size - meta);
    findPositionAndInsertNode(newBorn, l);
    // set size only if new block is splited
    target->size = size;
  }
  return target;
}

/*
  request new space of size (size + metadata) from system
*/
node * requestNewSpace(size_t size, int isLock) {
  node * newSpace;
  if (isLock) {
    newSpace = (node *)sbrk(size + meta);
  }
  else {
    pthread_mutex_lock(&lock);
    newSpace = (node *)sbrk(size + meta);
    pthread_mutex_unlock(&lock);
  }
  segment_size += (meta + size);
  initNode(newSpace, size);
  return newSpace;
}

/*
  malloc block of size from given block match
 */
void * real_malloc(size_t size, node * match, list * l, int isLock) {
  if (match == NULL) {
    match = requestNewSpace(size, isLock);
  }
  else {
    // remove curr from free space list
    removeNode(match, l);
  }
  match->isFree = 0;
  // split curr and put them into seperate lists
  node * allocated = splitBlock(match, size, l);
  return (void *)allocated + meta;
}

node * findBestFit(size_t size, list * l) {
  // find the smallest block that bigger than size
  node * curr = l->head;
  node * match = NULL;
  while (curr != NULL) {
    if (curr->size >= size && (match == NULL || curr->size < match->size)) {
      match = curr;
    }
    if (match != NULL && match->size <= size + meta) {
      break;
    }
    curr = curr->next;
  }
  return match;
}

/*
 * Best Fit malloc
 */
void * bf_malloc(size_t size, list * l, int isLock) {
  node * match = findBestFit(size, l);
  void * ans = real_malloc(size, match, l, isLock);
  return ans;
}

void * ts_malloc_lock(size_t size) {
  pthread_mutex_lock(&lock);
  void * ans = bf_malloc(size, &freeList, 1);
  pthread_mutex_unlock(&lock);
  return ans;
}

void * ts_malloc_nolock(size_t size) {
  return bf_malloc(size, &tls_freeList, 0);
}

/*
 * real free function to free a selected block 
*/
void real_free(node * curr, list * l) {
  curr->isFree = 1;
  node * prev = findPositionToInsert(curr, l);
  insertNode(curr, prev, l);
  checkAndMerge(curr, l);
}

/*
  ptr is a pointer to real data, which follows metadata
*/

void ts_free_lock(void * ptr) {
  pthread_mutex_lock(&lock);
  real_free((node *)(ptr - meta), &freeList);
  pthread_mutex_unlock(&lock);
}

void ts_free_nolock(void * ptr) {
  real_free((node *)(ptr - meta), &tls_freeList);
}
