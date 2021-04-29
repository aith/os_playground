/* Solution inspired by
 * https://github.com/jabowen/CSE130/blob/main/Assignment3/cartman.c */
#include "cartman.h"
#include "pthread.h"
#include "semaphore.h"
#include "stdio.h"
#include "stdlib.h"

#define DEF_JUNCS 7
#define DEF_TRACKS 7
#define MAX_CARTS 300

/* void *memset(void *str, int c, size_t n); */

int N_JUNCS = DEF_JUNCS;
int N_TRACKS = DEF_TRACKS;
sem_t junc_locks[17];
int track_taken_flags[17];

struct Cart {
  unsigned int cart;
  enum track track;
  enum junction junction;
};

struct Cart carts[MAX_CARTS];

enum junction get_far_junction(enum track track, enum junction junc_near) {
  /* Each tracks' pair of juncs are (track#, track#+1) */
  if (track == N_JUNCS - 1)
    return junc_near == 0 ? N_JUNCS - 1 : 0; /* last junction */
  int offset =
      (1 ^ ((track - junc_near) % 2)) * 2 - 1; /* if +1, ret -1, & vice versa */
  return junc_near + offset;
}

/* all the logic goes in here */
void *thread_arrive(void *argp) {
  struct Cart *cart_struct = (struct Cart *)argp;
  unsigned int cart = cart_struct->cart;
  enum track track = cart_struct->track;
  enum junction junc_near = cart_struct->junction;
  enum junction junc_far = get_far_junction(track, junc_near);

  while (track_taken_flags[track] > 0)
    sched_yield();

  /* Close neighboring tracks */
  track_taken_flags[(track + 1) % N_TRACKS]++;
  track_taken_flags[(track + (N_TRACKS - 1)) % N_TRACKS]++;

  sem_wait(&junc_locks[junc_near]);
  sem_wait(&junc_locks[junc_far]);

  reserve(cart, track);
  reserve(cart, (track + 1) % N_JUNCS);

  cross(cart, track, junc_near);

  track_taken_flags[(track + 1) % N_JUNCS]--;
  track_taken_flags[(track + (N_TRACKS - 1)) % N_JUNCS]--;

  // printf("\nrelease(%d,%d,%d,%d)\n", cart, track, (track+4)%5,
  // (track+1)%5);
  release(cart, track);
  release(cart, (track + 1) % N_JUNCS);

  sem_post(&junc_locks[junc_near]);
  sem_post(&junc_locks[junc_far]);
  /* sem_post(&junc_locks[track]); */
  /* sem_post(&junc_locks[(track + 1) % n_juncs]); */

  pthread_exit(NULL);
}

/*
 * Callback when CART on TRACK arrives at JUNCTION in preparation for
 * crossing a critical section of track.
 */
void arrive(unsigned int cart, enum track track, enum junction junction) {
  pthread_t t;
  struct Cart args = {cart, track, junction};
  carts[cart] = args;
  pthread_create(&t, NULL, &thread_arrive, (void *)&carts[cart]);
  /* pthread_join(t, NULL); */
}

/*
 * Start the CART Manager.
 *
 * Return is optional, i.e. entering an infinite loop is allowed, but not
 * recommended. Some implementations will do nothing, most will initialise
 * necessary concurrency primitives.
 */
void cartman(unsigned int tracks) {
  N_TRACKS = tracks;
  N_JUNCS = tracks;
  /* junc_locks = (sem_t *)malloc(N_JUNCS * sizeof(sem_t)); */
  /* track_taken_flags = (int *)malloc(N_TRACKS * sizeof(int)); */
  printf("%d\n", N_TRACKS);
  for (int i = 0; i < N_JUNCS; i++) {
    sem_init(&junc_locks[i], 0, 1);
  }
}
