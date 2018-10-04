// File:	my_pthread.c
// Author:	Yujie REN
// Date:	09/23/2017

// name:
// username of iLab:
// iLab Server:

#include "my_pthread_t.h"

#define USE_MY_PTHREAD 1 //(comment it if you want to use pthread)

#ifdef USE_MY_PTHREAD
#define pthread_t my_pthread_t
#define pthread_mutex_t my_pthread_mutex_t
#define pthread_create my_pthread_create
#define pthread_exit my_pthread_exit
#define pthread_join my_pthread_join
#define pthread_mutex_init my_pthread_mutex_init
#define pthread_mutex_lock my_pthread_mutex_lock
#define pthread_mutex_unlock my_pthread_mutex_unlock
#define pthread_mutex_destroy my_pthread_mutex_destroy
#endif

ucontext_t curr_context, main_context, t1, t2;
static my_pthread_t threadNo = 0;

my_scheduler scheduler;
struct itimerval timeslice;

/*alarm(unsigned int sec){
 timeslice.it_interval.tv_usec=0;
 timeslice.it_interval.tv_sec=3;
 setitimer(ITIMER_VIRTUAL, &timeslice, NULL);
 }*/

void sigalarmhandler(int signal) {
	if (signal == SIGVTALRM) {
		timeslice.it_interval.tv_usec = 0;
		timeslice.it_interval.tv_sec = 3;
		setitimer(ITIMER_VIRTUAL, &timeslice, NULL);
	}
}

my_pthread_t tid_generator() {
	return ++threadNo;
}
void make_scheduler() {
	scheduler.running_thread = NULL;

}

/* create a new thread */
int my_pthread_create(my_pthread_t *thread, pthread_attr_t *attr,
		void *(*function)(void *), void *arg) {

	assert(thread != NULL);
	getcontext(&curr_context);
	*thread = tid_generator();

	curr_context.uc_link = 0;
	curr_context.uc_stack.ss_sp = malloc(MEM);
	if (curr_context.uc_stack.ss_sp == NULL) {
		printf("Memory Allocation Error!!!\n");
		return 1;
	}
	curr_context.uc_stack.ss_size = MEM;
	curr_context.uc_stack.ss_flags = 0;

	tcb block;
	block.tid = *thread;
	block.ucontext = curr_context;
	block.next = NULL;

	if (scheduler.running_thread != NULL) {
		scheduler.waiting_queue->end->next = &block;
		scheduler.waiting_queue->end = &block;
	} else {

		scheduler.waiting_queue->start = &block;
		scheduler.waiting_queue->end = &block;
	}

	makecontext(&curr_context, &function, 0);

	return 0;
}
;

/* give CPU pocession to other user level threads voluntarily */
int my_pthread_yield() {
	//assert(schd.ready_queue!=NULL);
	assert(scheduler.waiting_queue!=NULL);
	getcontext(&t1);
	t1.uc_link = 0;
	t1.uc_stack.ss_sp = malloc(MEM);
	if (t1.uc_stack.ss_sp == NULL) {
		printf("Memory Allocation Error!!!\n");
		return 1;
	}
	t1.uc_stack.ss_size = MEM;
	t1.uc_stack.ss_flags = 0;

	tcb *curr = scheduler.waiting_queue;

	swapcontext(&curr_context, &t1);

	return 0;
}
;

/* terminate a thread */
void my_pthread_exit(void *value_ptr) {
}
;

/* wait for thread termination */
int my_pthread_join(my_pthread_t thread, void **value_ptr) {
	return 0;
}
;

/* initial the mutex lock */
int my_pthread_mutex_init(my_pthread_mutex_t *mutex,
		const pthread_mutexattr_t *mutexattr) {
	return 0;
}
;

/* aquire the mutex lock */
int my_pthread_mutex_lock(my_pthread_mutex_t *mutex) {
	assert(mutex != NULL);
	if (mutex->lock == 1) {
		return 0;
	} else {
		mutex->lock = 1;
		mutex->tid = 0;
	}
	return 0;
}
;

/* release the mutex lock */
int my_pthread_mutex_unlock(my_pthread_mutex_t *mutex) {
	assert(mutex != NULL);
	if (mutex->lock == 0) {
		return 1;
	} else {
		mutex->lock = 0;
		mutex->tid = 0;
	}
	return 0;
}
;

/* destroy the mutex */
int my_pthread_mutex_destroy(my_pthread_mutex_t *mutex) {
	assert(mutex != NULL);
	if (mutex->lock == 1) {
		printf("Mutex is locked by thread %d", mutex->tid);
		return 1;
	}
	free(mutex);
	return 0;
}
;

void pf(void *arg) {
	printf(": I Rock!!!\n");
}

/**
 * Implementing queue functions
 **/

tcb_list* init_queue( t) {
	tcb_list *queue = (tcb_list*) malloc(sizeof(tcb_list));
	queue->start = (tcb*) malloc(sizeof(tcb));
	if (queue->start == NULL) {
		printf("queue start initialization failed");
	} else {
		queue->start = NULL;
	}
	queue->end = (tcb*) malloc(sizeof(tcb));
	if (queue->end == NULL) {
		printf("queue end initialization failed");
	} else {
		queue->end = NULL;
	}
	return queue;
}

void enqueue(tcb_list *queue, tcb *new_thread) {

	if (queue->start == NULL) {
		queue->start = new_thread;
		queue->end = new_thread;
	} else {
		queue->end->next = new_thread;
		queue->end = new_thread;
	}
}

void dequeue(tcb_list *queue) {

	tcb *temp;

	if (queue->start == NULL) {
		printf("Nothing in queue to dequeue");
	}
	if (queue->start->next == NULL) {
		queue->start = NULL;
		queue->end = NULL;
	} else {
		temp = queue->start;
		queue->start = queue->start->next;
		if (queue->start == NULL) {
			queue->end = NULL;
		}
		free(temp);
	}
}

tcb* getTcb(ucontext_t t, int id) {
	tcb *temp = (tcb*) malloc(sizeof(tcb));
	temp->tid = id;
	temp->priority = 1;
	temp->state = READY;
	temp->ucontext = t;
	temp->next = NULL;
	return temp;
}

int main(int argc, char **argv) {
	getcontext(&main_context);
	tcb *temp = getTcb(main_context, 1);
	tcb *t2 = getTcb(main_context, 2);
	tcb_list *q = init_queue();
	enqueue(q, temp);
	enqueue(q, t2);
	tcb *s = q->start;
	while (s != NULL) {
		printf("Value in queue %d\n", s->tid);
		s = s->next;
	}
	dequeue(q);
	printf("After dequeue\n");
	s = q->start;
	while (s != NULL) {
		printf("Value in queue %d\n", s->tid);
		s = s->next;
	}
	return 0;
}
