/************************************************************************
 *
 * CSE130 Assignment 1
 *
 * POSIX Shared Memory Multi-Process Merge Sort
 *
 * Copyright (C) 2020-2021 David C. Harrison. All right reserved.
 *
 * You may not use, distribute, publish, or modify this code without
 * the express written permission of the copyright holder.
 *
 ************************************************************************/

#include "merge.h"
#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h>
#include <sys/mman.h>
/* #include <time.h> */
#include <sys/wait.h>
#include <unistd.h>

#define NAME "shmseg"

/*
 * Merge Sort in the current process a sub-array of ARR[] defined by the
 * LEFT and RIGHT indexes.
 */
void singleProcessMergeSort(int arr[], int left, int right) {
  if (left < right) {
    int middle = (left + right) / 2;
    singleProcessMergeSort(arr, left, middle);
    singleProcessMergeSort(arr, middle + 1, right);
    merge(arr, left, middle, right);
  }
}

/*
 * Merge Sort in the current process and at least one child process a
 * sub-array of ARR[] defined by the LEFT and RIGHT indexes.
 */
void multiProcessMergeSort(int arr[], int left, int right) {
  if (left > right || left < 0 || right < 0) {
    exit(EXIT_FAILURE);
  }
  /* Create shared memory */
  const int SIZE = 1000000;
  int shmfd = shm_open(NAME, O_CREAT | O_RDWR, 0666);
  /* configure the size of the shared memory object */
  ftruncate(shmfd, SIZE);
  /* memory map the shared memory object */
  int *shmarray = (int *)mmap(0, SIZE, PROT_WRITE, MAP_SHARED, shmfd, 0);
  /* Copy right side of local memory into shared memory */
  int middle = (left + right) / 2;
  for (int i = middle + 1; i <= right; i++) { // is right inclusive?
    shmarray[i] = arr[i];
  }
  /* Fork */
  int level = fork();
  int status = 0;
  switch (level) {
  case -1:
    fprintf(stderr, "ERROR: Fork failed.");
    exit(EXIT_FAILURE);
  case 0: // Do stuff as child
    shmarray = (int *)mmap(0, SIZE, PROT_WRITE, MAP_SHARED, shmfd, 0);
    singleProcessMergeSort(shmarray, middle + 1, right); // sort
    munmap(shmarray, SIZE);
    close(shmfd);
    exit(EXIT_SUCCESS);
  default:
    singleProcessMergeSort(arr, left, middle); // sort
    while (wait(&status) > 0) {
    };
    for (int i = middle + 1; i <= right; i++) { // is right inclusive?
      arr[i] = shmarray[i];
    }
    munmap(shmarray, SIZE);
    close(shmfd);
    shm_unlink(NAME);
    merge(arr, left, middle, right); // merge to local
  }
}
