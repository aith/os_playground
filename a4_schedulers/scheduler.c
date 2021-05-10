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
void *running_q;
int t_total = 0;
thread_t *t_current;
int t_current_id = 0;
int cpu_occupied = 0;
stats_t *all_stats;
void *t_stats_q;
int t_count = 0;

int wait_start = 0;
int wait_end = 0;
int turnaround_start = 0;
int turnaround_end = 0;
int waiting_time_start[MAX_THREADS];
int waiting_time_end[MAX_THREADS];
int turnaround_time_start[MAX_THREADS];
int turnaround_time_end[MAX_THREADS];

typedef struct __stats_by_thread_t {
  thread_t *t;
  unsigned int waiting_time_start;
  unsigned int waiting_time_end;
  unsigned int turnaround_time_start;
  unsigned int turnaround_time_end;
  struct __stats_t
      *stats; // Address of the first of THREAD_COUNT stats_t 'objects'
} stats_by_thread_t;

stats_by_thread_t *current;
unsigned int waiting_time_sum = 0;
unsigned int turnaround_time_sum = 0;

stats_by_thread_t *init_stats_by_thread_t(thread_t *t) {
  /* printf("initing\n"); */
  stats_by_thread_t *stats_by_t = malloc(sizeof(stats_by_thread_t));
  stats_by_t->stats = malloc(sizeof(stats_t));
  stats_by_t->stats->tstats = malloc(sizeof(stats_t)); // testing
  stats_by_t->stats->tid = t_count + 1;
  /* stats_by_t->t = t; */
  stats_by_t->t = t; // malloc(sizeof(thread_t));
  stats_by_t->waiting_time_start = 0;
  stats_by_t->waiting_time_end = 0;
  stats_by_t->turnaround_time_start = 0;
  stats_by_t->turnaround_time_end = 0;
  t_count++;
  return stats_by_t;
}

/* void update_stats_array() { */

/* } */

/**
 * Initalise a scheduler implemeting the requested ALGORITHM. QUANTUM is only
 * meaningful for ROUND_ROBIN.
 */
void scheduler(enum algorithm algorithm, unsigned int quantum) {
  ready_q = (void *)queue_create();
  running_q = (void *)queue_create();
  t_stats_q = (void *)queue_create();
  for (int i = 0; i < MAX_THREADS; i++) {
    waiting_time_start[i] = 0;
    waiting_time_end[i] = 0;
    turnaround_time_start[i] = 0;
    turnaround_time_end[i] = 0;
  }
}

/**
 * Programmable clock interrupt handler. Call sim_time() to find out
 * what tick this is. Called after all calls to sys_exec() for this
 * tick have been made.
 */
void tick() {

  /* printf("did exit\n"); */
  /* stats_by_thread_t *next; */
  if (queue_size(ready_q) != 0) {
    if (queue_size(running_q) == 0) {
      stats_by_thread_t *next = (stats_by_thread_t *)queue_dequeue(ready_q);
      next->waiting_time_end = sim_time();
      queue_enqueue(running_q, next);
      sim_dispatch(next->t);
    }
  }
}

static int same_t(void *a, void *b) {
  /* return &(((stats_by_thread_t *)a)->t) == b; */
  /* printf("tid is %d\n", ((stats_by_thread_t *)a)->tid); */
  /* printf("%d\n", ((stats_by_thread_t *)a)->stats->tid); */
  /* printf("%d\n", &(((stats_by_thread_t *)a)->t) == b); */
  return ((stats_by_thread_t *)a)->t == (thread_t *)b;
  /* ; */
  /* return ((outer_t*)outer)->threadname == (thread_t*)inner; */
}

/**
 * Thread T is ready to be scheduled for the first time.
 */
void sys_exec(thread_t *t) {
  /* Put thread in ready queue if CPU is occupied */
  /* printf("in sys_exec\n"); */
  stats_by_thread_t *next = init_stats_by_thread_t(t);
  next->waiting_time_start = sim_time();
  next->turnaround_time_start = sim_time();
  queue_enqueue(t_stats_q, next);
  queue_enqueue(ready_q, next);
}

/**
 * Thread T has completed execution and should never again be scheduled.
 */
void sys_exit(thread_t *t) {
  /* printf("%d", queue_size(running_q)); */
  /* printf("in sys_exit\n"); */
  stats_by_thread_t *fin = (stats_by_thread_t *)queue_dequeue(running_q);
  fin->turnaround_time_end = sim_time() + 1; /* specified by handout */

  unsigned int waiting_time = fin->waiting_time_end - fin->waiting_time_start;
  unsigned int turnaround_time =
      fin->turnaround_time_end - fin->turnaround_time_start;
  fin->stats->waiting_time = waiting_time;
  fin->stats->turnaround_time = turnaround_time;
  waiting_time_sum += waiting_time;
  turnaround_time_sum += turnaround_time;
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

static int tid_lt(void *a, void *b) {
  return ((stats_by_thread_t *)a)->tid < ((stats_by_thread_t *)b)->tid;
}

stats_t *stats() {
  /* Calc means */
  unsigned int mean_wait = waiting_time_sum / t_count;
  unsigned int mean_turnaround = turnaround_time_sum / t_count;
  all_stats = malloc(sizeof(stats_t));
  all_stats->thread_count = t_count;
  all_stats->waiting_time = mean_wait;
  all_stats->turnaround_time = mean_turnaround;
  /* printf("turnaround_time is %d\n", mean_turnaround); */
  /* printf("turnaround_time_start[0] is %d\n", turnaround_time_start[1]); */
  /* printf("turnaround_time_end[0] is %d\n", turnaround_time_end[1]); */
  /* all_stats->tstats = malloc(sizeof(stats_t) * t_count); */
  all_stats->tstats = malloc(sizeof(stats_t) * all_stats->thread_count);

  /* Fill in tstats */
  queue_sort(t_stats_q, tid_lt);
  for (int i = 0; i < t_count; i++) {
    stats_by_thread_t *sbt = queue_dequeue(t_stats_q);
    int tid = sbt->stats->tid;
    all_stats->tstats[i].tid = tid;
    all_stats->tstats[i].waiting_time = sbt->stats->waiting_time;
    /* sbt->waiting_time_end - sbt->waiting_time_start; */
    all_stats->tstats[i].turnaround_time = sbt->stats->turnaround_time;
    /* sbt->turnaround_time_end - sbt->turnaround_time_start; */
  }
  /* printf("turnaround_time[0] using tstats is is %d\n", */
  /*        all_stats->tstats->turnaround_time); */
  /* printf("wait_time[0] using tstats is is %d\n", */
  /*        all_stats->tstats->waiting_time); */
  return all_stats;
}
