#include <stdio.h>
#include <stdlib.h>

#include "my_malloc.h"

#ifdef FF
#define MALLOC(sz) ff_malloc(sz)
#define FREE(p) ff_free(p)
#endif
#ifdef BF
#define MALLOC(sz) bf_malloc(sz)
#define FREE(p) bf_free(p)
#endif

void testUnit(int * sizes, int index, int * expected_sum, int * sum, int ** array) {
  int i;
  int size = sizes[index];
  *expected_sum += size * size;
  printf("size in this iteration %lx + %lx, ", size * sizeof(int), sizeof(node));
  array[index] = (int *)MALLOC(size * sizeof(int));
  printf("%p \n", array[index]);
  for (i = 0; i < size; i++) {
    array[index][i] = size;
  }  //for i
  for (i = 0; i < size; i++) {
    *sum += array[index][i];
  }
}

void freeUnit(int ** array, int index) {
  FREE(array[index]);
  printf("free array[%d]\n", index);
}

int main(int argc, char * argv[]) {
  const unsigned NUM_ITEMS = 10;
  int i;
  int sizes[] = {4, 16, 8, 32, 7, 256, 23, 4, 10, 32};
  int sum = 0;
  int expected_sum = 0;
  int * array[NUM_ITEMS];

  for (i = 0; i < 4; i++) {
    testUnit(sizes, i, &expected_sum, &sum, array);
  }
  printFreeList();

  freeUnit(array, 0);
  freeUnit(array, 2);
  printFreeList();

  for (i = 4; i < 6; i++) {
    testUnit(sizes, i, &expected_sum, &sum, array);
  }
  printFreeList();

  freeUnit(array, 5);
  freeUnit(array, 1);
  freeUnit(array, 3);
  printFreeList();

  for (i = 6; i < 8; i++) {
    testUnit(sizes, i, &expected_sum, &sum, array);
  }
  printFreeList();

  freeUnit(array, 4);
  printFreeList();

  for (i = 8; i < 10; i++) {
    testUnit(sizes, i, &expected_sum, &sum, array);
  }
  printFreeList();

  freeUnit(array, 6);
  freeUnit(array, 7);
  freeUnit(array, 8);
  freeUnit(array, 9);
  printFreeList();

  if (sum == expected_sum) {
    printf("Calculated expected value of %d\n", sum);
    printf("Test passed\n");
  }
  else {
    printf("Expected sum=%d but calculated %d\n", expected_sum, sum);
    printf("Test failed\n");
  }  //else

  return 0;
}
