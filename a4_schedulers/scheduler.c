/**
 * See scheduler.h for function details. All are callbacks; i.e. the simulator
 * calls you when something interesting happens.
 */
#include <stdlib.h>

#include "queue.h"
#include "scheduler.h"
#include "simulator.h"

/* thread_t thread_array[MAX_THREADS]; */
#define MAX_THREADS 2048
void *ready_q;
int t_total = 0;
thread_t *t_current;
int t_current_id = 0;
int cpu_occupied = 0;
stats_t *all_stats;
stats_t *t_stats_arr[MAX_THREADS];
int t_count = 0;

int wait_start = 0;
int wait_end = 0;
int turnaround_start = 0;
int turnaround_end = 0;
int waiting_time_start[MAX_THREADS];
int waiting_time_end[MAX_THREADS];
int turnaround_time_start[MAX_THREADS];
int turnaround_time_end[MAX_THREADS];

/* Create stats object* */
stats_t *init_stats(int thread_count) {
  stats_t *stats = malloc(sizeof(stats_t));
  stats->tid = thread_count + 1;
  return stats;
}

/* void update_stats_array() { */

/* } */

/**
 * Initalise a scheduler implemeting the requested ALGORITHM. QUANTUM is only
 * meaningful for ROUND_ROBIN.
 */
void scheduler(enum algorithm algorithm, unsigned int quantum) {
  ready_q = (void *)queue_create();
  for (int i = 0; i < MAX_THREADS; i++) {
    waiting_time_start[i] = 0;
    waiting_time_end[i] = 0;
    turnaround_time_start[i] = 0;
    turnaround_time_end[i] = 0;
  }
}

void update_t_current(thread_t *t) {
  t_current = t;
  t_current_id++;
}

/**
 * Programmable clock interrupt handler. Call sim_time() to find out
 * what tick this is. Called after all calls to sys_exec() for this
 * tick have been made.
 */
void tick() {}

void init_thread() {
  stats_t *t_stats = init_stats(t_count);
  t_stats_arr[t_count] = t_stats;
}

/**
 * Thread T is ready to be scheduled for the first time.
 */
void sys_exec(thread_t *t) {
  /* stats_t *stats = malloc(sizeof(stats_t)); /\* see scheduler.h *\/ */
  /* if (all_stats->thread_count == 0) { */
  /*   stats_t *t_stats = init_stats(all_stats->thread_count); */
  /*   all_stats->tstats = t_stats; */
  /*   cpu_occupied++; */
  /* } else */
  if (queue_size(ready_q) == 0) {
    /* t_current = &t; */
    /* t_current_id = 0; */
    init_thread(t_current_id);
    t_current = t;
    t_current_id = 0;
    turnaround_time_start[t_current_id] = sim_time();
    sim_dispatch(t);
  } else {
    waiting_time_start[t_count] = sim_time();
    queue_enqueue(ready_q, t);
  }
  t_count++;

  /* Put thread in ready queue if CPU is occupied */
}

/**
 * Thread T has completed execution and should never again be scheduled.
 */
void sys_exit(thread_t *t) {
  /* Run next ready thread */
  /* turnaround_time_end[t_current_id] = sim_time(); */
  turnaround_time_end[0] = sim_time() + 1;

  if (queue_size(ready_q) > 0) {
    waiting_time_end[t_current_id] = sim_time();

    thread_t *next_t = (thread_t *)queue_dequeue(ready_q);
    update_t_current(next_t);

    init_thread(next_t);
    turnaround_time_start[t_current_id] = sim_time();
    sim_dispatch(next_t);
  } else {
  }
}

/**
 * Thread T has requested a read operation and is now in an I/O wait queue.
 * When the read operation starts, io_starting(T) will be called, when the
 * read operation completes, io_complete(T) will be called.
 */
void sys_read(thread_t *t) {}

/**
 * Thread T has requested a write operation and is now in an I/O wait queue.
 * When the write operation starts, io_starting(T) will be called, when the
 * write operation completes, io_complete(T) will be called.
 */
void sys_write(thread_t *t) {}

/**
 * An I/O operation requested by T has completed; T is now ready to be
 * scheduled again.
 */
void io_complete(thread_t *t) {}

/**
 * An I/O operation requested by T is starting; T will not be ready for
 * scheduling until the operation completes.
 */
void io_starting(thread_t *t) {}

/**
 * Return dynamically allocated stats for the scheduler simulation, see
 * scheduler.h for details. Memory allocated by your code will be free'd
 * by the similulator. Do NOT return a pointer to a stack variable.
 */
stats_t *stats() {
  /* Calc means */
  unsigned int mean_wait = 0;
  unsigned int mean_turnaround = 0;
  for (int i = 0; i < t_count; i++) {
    unsigned int wait = waiting_time_end[i] - waiting_time_start[i];
    unsigned int turnaround = turnaround_time_end[i] - turnaround_time_start[i];
    t_stats_arr[i]->turnaround_time = turnaround;
    t_stats_arr[i]->waiting_time = wait;
    mean_wait += wait;
    mean_turnaround += turnaround;
  }
  mean_wait /= t_count;
  mean_turnaround /= t_count;
  printf("t_count is %d\n", t_count);

  all_stats = init_stats(t_count);
  all_stats->thread_count = t_count;
  all_stats->waiting_time = mean_wait;
  all_stats->turnaround_time = mean_turnaround;
  /* printf("turnaround_time is %d\n", mean_turnaround); */
  /* printf("turnaround_time_start[0] is %d\n", turnaround_time_start[0]); */
  /* printf("turnaround_time_end[0] is %d\n", turnaround_time_end[0]); */
  all_stats->tstats = malloc(sizeof(stats_t) * t_count);
  all_stats->tstats = t_stats_arr[0];
  /* printf("turnaround_time[0] using tstats is is %d\n", */
  /*        all_stats->tstats->turnaround_time); */
  /* printf("wait_time[0] using tstats is is %d\n", */
  /*        all_stats->tstats->waiting_time); */
  return all_stats;
}
