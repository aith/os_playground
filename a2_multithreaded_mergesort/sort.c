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

/*
 * This function stub needs to be completed
 */

struct mergeArgs {
  int tid;
  int start_index;
  int length;
  int *arr;
} mergeA;

void *threadMerge(void *argp) {
  /* tid = (int)argp[0]; */
  struct mergeArgs *targs = argp;
  printf("I'm process %d, I have %d args\n,", targs->tid, targs->length);
  pthread_exit(NULL);
}

// right is the final number
void multiThreadedMergeSort(int arr[], int left, int right) {
  int length = right - left + 1; // numbor of variables in arr
  int amt_per_t = (int)length / 4;
  int amt_extra = (int)length % 4;
  struct mergeArgs args[NUM_THREADS];
  pthread_t threads[NUM_THREADS];
  for (int i = 0; i < NUM_THREADS; i++) {
    args[i].tid = i;
    args[i].start_index = amt_per_t * i; // TODO this might be wrong
    args[i].length = amt_per_t;
    args[i].arr = arr;
    if (i == 0) {
      args[i].length += amt_extra;
    }
    pthread_t thread = pthread_create(&threads[i], NULL, threadMerge, &args[i]);
    if (thread) {
      printf("Error: could not create pthread!\n");
      exit(1);
    }
  }

  for (int i = 0; i < NUM_THREADS; i++) {
    pthread_join(threads[i], NULL);
  }

  printf("After Thread\n");
  exit(0);
}
