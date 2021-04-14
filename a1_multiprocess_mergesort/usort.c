/************************************************************************
 *
 * CSE130 Assignment 1
 *
 * UNIX Shared Memory Multi-Process Merge Sort
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
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

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
  // generate unique key key_t
  int ID = 64;
  key_t key = ftok("shmseg", ID); // returns a shared memory identifier
  int shmid = shmget(key, 100000, 0666 | IPC_CREAT);
  /* Attach to shared memory */
  int *shmarray = (int *)shmat(shmid, (void *)0, 0);
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
    shmarray = (int *)shmat(
        shmid, (void *)0,
        0); // attach. In this case it's not needed since the child is as exact
            // copy, but it's good practice because the parent can detach.
    singleProcessMergeSort(shmarray, middle + 1, right); // sort
    shmdt(shmarray);                                     // detach
    exit(EXIT_SUCCESS);
  default:
    singleProcessMergeSort(arr, left, middle); // sort
    while (wait(&status) > 0) {
    };
    for (int i = middle + 1; i <= right; i++) { // is right inclusive?
      arr[i] = shmarray[i];
    }
    shmdt(shmarray);                 // detach
    shmctl(shmid, IPC_RMID, NULL);   // delete
    merge(arr, left, middle, right); // merge to local
  }
}
