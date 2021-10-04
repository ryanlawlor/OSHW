/* counter.c */

// Uses simple threads to increment a counter value incrementally.

#include <stdio.h> // for printf
#include <unistd.h> // for sleep
#include <stdlib.h> // for malloc and other things
#include <pthread.h> // just for the lock!
#include <sys/types.h>
#include <sys/socket.h>

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

		// CLIENT CODE
			int sock_fd;
			int conn_fd;

			struct sockaddr saddr = {AF_LOCAL, "incr-num-serv"};
			socklen_t saddrlen = sizeof(struct sockaddr) + 6;

			sock_fd = socket(AF_LOCAL, SOCK_STREAM, 0);
			conn_fd = connect(sock_fd, &saddr, saddrlen);

			char req[4];
			int BUFF[1];
			BUFF[0] = -1;
			sprintf(req, "%d", tid);

			write(sock_fd, req, 3);
			read(sock_fd, BUFF, 4);

			int val = BUFF[0];
			// if(val != -1)
			// {
			// 	printf("Got back %d\n", val);
			// }

			close(sock_fd);
			close(conn_fd);
		// CLIENT CODE

		if (val > 0)
		{
			counter = counter + val;
			printf("Thread id %-2u (I was created when i=%ld).  Incrementing counter to %d\n", tid, (long)arg, counter);
		}
		else
		{
			uthread_yield();
		}
		// counter = counter + 1;
		// printf("Thread id %-2u (I was created when i=%ld).  Incrementing counter to %d\n", tid, (long)arg, counter);
		pthread_mutex_unlock(&mutex);
		//uthread_yield(); //pthread_yield on linux
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
