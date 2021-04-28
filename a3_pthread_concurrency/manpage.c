
/*********************************************************************
 *
 * Copyright (C) 2020-2021 David C. Harrison. All right reserved.
 *
 * You may not use, distribute, publish, or modify this code without 
 * the express written permission of the copyright holder.
 *
 ***********************************************************************/


/*
 * Create a thread for each of seven manpage paragraphs and have them synchronize
 * their invocations of showParagraph() so the manual page displays as follows:
 *
 --------- 8< ---------------------------------------------------
 
A semaphore S is an unsigned-integer-valued variable.
Two operations are of primary interest:
 
P(S): If processes have been blocked waiting on this semaphore,
 wake one of them, else S <- S + 1.
 
V(S): If S > 0 then S <- S - 1, else suspend execution of the calling process.
 The calling process is said to be blocked on the semaphore S.
 
A semaphore S has the following properties:

1. P(S) and V(S) are atomic instructions. Specifically, no
 instructions can be interleaved between the test that S > 0 and the
 decrement of S or the suspension of the calling process.
 
2. A semaphore must be given an non-negative initial value.
 
3. The V(S) operation must wake one of the suspended processes. The
 definition does not specify which process will be awakened.

 --------- 8< ---------------------------------------------------
 *
 * As supplied, shows random single messages.
 */

#include "manpage.h"
#include <pthread.h>
#include <stdio.h>

#define NUM_PARAGRAPHS 7

pthread_mutex_t lock;
pthread_t threads[NUM_PARAGRAPHS];
pthread_cond_t condvars[NUM_PARAGRAPHS];

void *thread_build_paragraph(void* argp);

void manpage() {
    /* init types */
    pthread_mutex_init(&lock, NULL);

    for (int i = 0; i < NUM_PARAGRAPHS; i++) {
        pthread_cond_init(&condvars[i], NULL);
    }

    /* Create and run threads */
    for (int i = 0; i < NUM_PARAGRAPHS; i++) {
        pthread_create(&threads[i], NULL, thread_build_paragraph, NULL);
    }

    /* Wait for threads to finish */
    for(int i = 0; i < NUM_PARAGRAPHS; i++){
        pthread_join(threads[i], NULL);
    }

    return;
}

void *thread_build_paragraph(void* argp) {
    return 0;
}

