#include "merge.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_THREADS 4

/* LEFT index and RIGHT index of the sub-array of ARR[] to be sorted */
void singleThreadedMergeSort(int arr[], int left, int right) {
  if (left < right) {
    int middle = (left + right) / 2;
    singleThreadedMergeSort(arr, left, middle);
    singleThreadedMergeSort(arr, middle + 1, right);
    merge(arr, left, middle, right);
  }
}

struct mergeArgs {
  int tid;
  int start_index;
  int length;
  int **arr;
} mergeA;

void *threadMerge(void *argp) {
  /* tid = (int)argp[0]; */
  struct mergeArgs *targs = argp;
  int l = targs->start_index;
  int r = targs->start_index + targs->length - 1;
  singleThreadedMergeSort(*(targs->arr), l, r);
  /* printf("I'm process %d. I'm processing [%d, %d]\n. My merge is ",
   * targs->tid, */
  /*        l, r); */
  /* for (int i = l; i <= r; i++) { */
  /*   printf("%d ", (*(targs->arr))[i]); */
  /* } */
  /* printf("\n"); */

  pthread_exit(NULL);
}

// right is the final number
void multiThreadedMergeSort(int arr[], int left, int right) {
  int length = right - left + 1; // numbor of variables in arr
  /* printf("   Given "); */
  /* for (int i = 0; i < length; i++) { */
  /*   printf("%d ", arr[left + i]); */
  /* } */
  /* printf("\n"); */

  int amt_per_t = (int)length / 4;
  int amt_extra = (int)length % 4;
  struct mergeArgs args[NUM_THREADS];
  pthread_t threads[NUM_THREADS];

  for (int i = 0; i < NUM_THREADS; i++) {
    args[i].tid = i;
    args[i].start_index = amt_per_t * i + left; // TODO this might be wrong
    args[i].length = amt_per_t;
    args[i].arr = &arr;
    if (i + 1 == NUM_THREADS) {
      args[i].length += amt_extra;
    }
    pthread_t thread = pthread_create(&threads[i], NULL, threadMerge, &args[i]);
    if (thread) {
      printf("Error: could not create pthread!\n");
      printf("FAIL\n");
      exit(1);
    }
  }

  for (int i = 0; i < NUM_THREADS; i++) {
    pthread_join(threads[i], NULL);
  }

  // merge the quarters

  int l1 = left;
  int r1 = (right - amt_extra + left) / 2;
  int m1 = (l1 + r1) / 2;
  /* printf("\nFirst Merging from %d to %d to %d\n", l1, m1, r1); */
  merge(arr, l1, m1, r1);

  int l2 = r1 + 1;
  int r2 = right;
  int m2 = (l2 + r2) / 2 - amt_extra;
  /* printf("Second Merging from %d to %d to %d\n", l2, m2, r2); */
  /* printf("where Right is %d", r2); */
  merge(arr, l2, m2, r2);
  merge(arr, left, (right + left) / 2 - amt_extra, right);

  printf("      Result is ");
  for (int i = 0; i < length; i++) {
    printf("%d ", arr[left + i]);
  }
  printf("PASS\n");
  exit(EXIT_SUCCESS);
}
