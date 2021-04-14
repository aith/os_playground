#include "merge.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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

void *threadStart(void *vargp) {
  printf("hi, i just started!\n");
  return NULL;
}

void multiThreadedMergeSort(int arr[], int left, int right) {
  // Delete this line, it's only here to fail the code quality check
  /* int i = 0; */

  // Your code goes here
  pthread_t t;
  pthread_create(&t, NULL, threadStart, NULL);
  pthread_join(t, NULL);
  printf("After Thread\n");
  exit(0);
}
