// File:	my_pthread_t.h
// Author:	Amogh Kulkarni, Sukumar Gaonkar, Vatsal Parikh
// Date:	09/23/2017

// name:
// username of iLab:
// iLab Server:
#ifndef MY_PTHREAD_T_H
#define MY_PTHREAD_T_H

#define _GNU_SOURCE

/* include lib header files that you need here: */
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <assert.h>
#include <signal.h>
#include <errno.h>

#define MEM 1024

#define LEVELS 20
#define TIME_QUANTUM 25
#define RUNNING_TIME 500

typedef uint my_pthread_t;

typedef enum state {
	READY, RUNNING, WAITING, TERMINATED,
} state;

typedef struct threadControlBlock {
	my_pthread_t tid;
	struct threadControlBlock *next;
	ucontext_t ucontext;
	enum state state;
	uint timeExecuted;
	struct tcb_queue *tcb_wait_queue;
	uint priority;
} tcb;

/*Pointers to the start and end of the queue*/
typedef struct tcb_queue {
	tcb *start;
	tcb *end;
} tcb_list;

/* mutex struct definition */
typedef struct my_pthread_mutex_t {
	int lock;
	my_pthread_t tid;
	struct my_pthread_mutex_t *next;
	struct tcb_queue *m_wait_queue;
	int initialized;
} my_pthread_mutex_t;

/*Scheduler defintion*/
typedef struct my_scheduler_t {
	tcb *running_thread;
	tcb_list *priority_queue[LEVELS];
	my_pthread_mutex_t *mutex;
} my_scheduler;

typedef struct my_pthread_return_values {
	my_pthread_t tid;
	void *return_val;
	struct my_pthread_return_values *next;
} thread_ret_val;

/* define your data structures here: */
const int no_of_queues = 5;

/* create a new thread */
int my_pthread_create(my_pthread_t *thread, pthread_attr_t *attr,
		void *(*function)(void *), void *arg);

/* give CPU pocession to other user level threads voluntarily */
int my_pthread_yield();

/* terminate a thread */
void my_pthread_exit(void *value_ptr);

/* wait for thread termination */
int my_pthread_join(my_pthread_t thread, void **value_ptr);

/* initial the mutex lock */
int my_pthread_mutex_init(my_pthread_mutex_t *mutex,
		const pthread_mutexattr_t *mutexattr);

/* aquire the mutex lock */
int my_pthread_mutex_lock(my_pthread_mutex_t *mutex);

/* release the mutex lock */
int my_pthread_mutex_unlock(my_pthread_mutex_t *mutex);

/* destroy the mutex */
int my_pthread_mutex_destroy(my_pthread_mutex_t *mutex);

#endif
