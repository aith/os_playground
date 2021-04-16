#include "merge.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_THREADS 4
#define AUX_THREADS 2

int *array;

typedef struct mergeArgs {
  int l;
  int r;
  int *a;

} mergeArgs;

void singleThreadedMergeSort(int arr[], int left, int right) {
  if (left < right) {
    int middle = (left + right) / 2;
    singleThreadedMergeSort(arr, left, middle);
    singleThreadedMergeSort(arr, middle + 1, right);
    merge(arr, left, middle, right);
  }
}

void *lowerSort(void *argp) {
  mergeArgs *args;
  args = (mergeArgs *)argp;
  singleThreadedMergeSort(array, args->l, args->r);
  pthread_exit(NULL);
}

void *upperSort(void *argp) {
  mergeArgs *args;
  args = (mergeArgs *)argp;
  int middle = (args->l + args->r) / 2;
  mergeArgs *ml = malloc(sizeof(*ml));
  mergeArgs *mr = malloc(sizeof(*mr));
  ml->l = args->l;
  ml->r = middle;
  ml->a = args->a;
  mr->l = middle + 1;
  mr->r = args->r;
  ml->a = args->a;
  pthread_t tl;
  pthread_create(&tl, NULL, lowerSort, (void *)ml);
  pthread_t tr;
  pthread_create(&tr, NULL, lowerSort, (void *)mr);
  pthread_join(tl, NULL);
  pthread_join(tr, NULL);
  free(ml);
  free(mr);
  merge(array, args->l, middle, args->r);
  pthread_exit(NULL);
}

void multiThreadedMergeSort(int arr[], int left, int right) {
  array = arr;
  int middle = (left + right) / 2;
  mergeArgs *ml = malloc(sizeof(*ml));
  mergeArgs *mr = malloc(sizeof(*mr));
  ml->l = left;
  ml->r = middle;
  ml->a = arr;
  mr->l = middle + 1;
  mr->r = right;
  mr->a = arr;
  pthread_t tl;
  pthread_create(&tl, NULL, upperSort, (void *)ml);
  pthread_t tr;
  pthread_create(&tr, NULL, upperSort, (void *)mr);
  pthread_join(tl, NULL);
  pthread_join(tr, NULL);
  free(ml);
  free(mr);
  merge(array, left, middle, right);
}
