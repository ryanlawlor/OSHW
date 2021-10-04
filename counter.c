/* counter.c */

// Uses simple threads to increment a counter value incrementally.

#include <stdio.h> // for printf
#include <unistd.h> // for sleep
#include <stdlib.h> // for malloc and other things
#include <pthread.h> // just for the lock!

#include "uthread.h"


static int counter = 0;
static int max_counter = 12;

static pthread_mutex_t mutex; // Used to guard counter

void incr(void *arg){
	unsigned int tid = uthread_get_id();

	while(1){
		pthread_mutex_lock(&mutex);
		if(counter >= max_counter){
			pthread_mutex_unlock(&mutex);
			break;
		}
		sleep(1);
		counter = counter + 1;
		printf("Thread id %-2u (I was create when i=%ld).  Incrementing counter to %d\n", tid, (long)arg, counter);
		pthread_mutex_unlock(&mutex);
		uthread_yield(); //pthread_yield on linux
	}
}

int main(void){
	printf("Main thread starting now!  Counting to: %d\n", max_counter);

	pthread_mutex_init(&mutex, NULL);
	uthread_init();
	int num_threads = 3;

	uthread_t *list = malloc(sizeof(uthread_t) * num_threads);
	int i = 0;
	for(i = 0; i < num_threads; i++){
		list[i] = *uthread_create((void *)incr, (void *)((long)i) );
		printf("Main, thread_list[%d].tid: %u\n", i, list[i].tid);
	}

	
	for(i = 0; i < num_threads; i++){
		printf("Waiting on %u to finish...\n", list[i].tid);
		uthread_join(list[i]);
	}
	

	printf("Final value: %d\n", counter);

	return 0;
}
