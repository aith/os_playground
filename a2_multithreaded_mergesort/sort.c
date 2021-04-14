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

void *threadStart(void *argp) {
  int tid;
  tid = (int)argp;
  printf("hi, i just started! I'm process %d\n", tid);
  /* return NULL; */
  pthread_exit(NULL);
}

void multiThreadedMergeSort(int arr[], int left, int right) {
  // Delete this line, it's only here to fail the code quality check
  /* int i = 0; */

  // Your code goes here
  pthread_t threads[NUM_THREADS];
  for (int i = 0; i < NUM_THREADS; i++) {
    pthread_t thread =
        pthread_create(&threads[i], NULL, threadStart, (void *)i);
    if (thread) {
      printf("Error: could not create pthread!");
      exit(1);
    }
  }

  for (int i = 0; i < NUM_THREADS; i++) {
    pthread_join(threads[i], NULL);
  }

  printf("After Thread\n");
  exit(0);
}
