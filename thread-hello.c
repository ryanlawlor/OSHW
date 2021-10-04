/* thread-hello.c */
// Creates threads to print messages!

#include <stdio.h> //printf among other things
#include <unistd.h> // to sleep
#include <stdlib.h>

#include "uthread.h" // your own threading library, comment out line above

void foo2(void *argp){
	printf("T2 I am another thread!  Here is my input: %ld. \n", (long)argp);
}

void hello_printer(void *argp){
	int tid = uthread_get_id();
	printf("T1 Hello World! from thread: %d.\n", tid);
}

int main(void){
	printf("MAIN just starting now.\n");

	uthread_init();

	uthread_t *t1 = uthread_create((void *)hello_printer, NULL);
	printf("MAIN the TID of t1 is: %u.\n", t1->tid);

	// Comment out this line to test creating only one thread
	uthread_create((void *)foo2, (void *)3);

	sleep(2); // Hacky way to wait for threads to finish, but it works in this case
	printf("MAIN at the very end now.  Goodbye!\n");
	return 0;
}