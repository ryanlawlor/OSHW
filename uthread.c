// User-space thread library


// Of course, my header file!
#include "uthread.h"

#include <stdlib.h> // malloc and other things
#include <stdio.h> // some debug printing


// Number of bytes in a Mebibyte, used for thread stack allocation
#define MEM 1048567


// The threads are managed with a double-linked list
static ucontext_t *main_ctx; // original (main) context
static int num_threads = 0; // Used to count the number of linked list nodes and give thread_ids

// Inserts a node (at the back) into the linked-list
void ll_insert(node *new_node){
	if(head == NULL){
		head = new_node;
		tail = new_node;
		//printf("head tid: %u\n", head->t->tid);
	} else {
		tail->next = new_node;
		tail = new_node;
	}
}

// print all the threads in the linked list
void ll_print(){
	int counter = 0;
	printf("-- Linked List of Threads --\n");
	node *cur = head;
	while(cur != NULL){
		printf("Node %d with tid %u and state: %d\n", counter, cur->t->tid, cur->t->state);
		cur = cur->next;
		counter++;
	}
	printf("--Linked List of Threads --\n");
}

// for a given state, return the first thread node of matching state
node *ll_get_by_state(int state){
	node *cur = head;
	while( (cur != NULL) && (cur->t->state != state) ){
		//printf("thread tid: %u state %d\n", cur->t->tid, cur->t->state);
		cur = cur->next;
	}
	return cur;
}

// for a given tid, return the thread node of matching tid
node *ll_get_by_tid(unsigned int tid){
	node *cur = head;
	while( (cur != NULL) && (cur->t->tid != tid)){
		cur = cur->next;
	}
	return cur;
}









// Feel free to implement utility methods as you see fit.

/** These are the methods you need to implement! **/
void uthread_init(void){
	//printf("init called\n");

	// Implement the necessary structs and state so that the first call to uthread_create is successful
	// You should assume that this method is called by the program's main / original thread.
	// Set the value of static ucontext_t *main_ctx (declared above)

	// 1) Make main context  (and place into main_ctx)
	main_ctx = malloc(sizeof(ucontext_t));
	getcontext(main_ctx);
	// 2) Make uthread_t
	uthread_t *main_thread = malloc(sizeof(uthread_t)); 
	main_thread->state = T_ACTIVE; 
	main_thread->ctx = main_ctx;
	main_thread->tid = num_threads;
	// 3) Make node
	node *main_node = malloc(sizeof(node));
	main_node->t = main_thread;
	// 4) Insert into linked list
	ll_insert(main_node);
	num_threads++;
}

uthread_t *uthread_create(void *(*func)(void *), void *argp){
	//printf("create called\n");
	// Create a new thread which will call func
	// 1) Make new ucontext_t
	node *active_node = ll_get_by_state(T_ACTIVE);
	ucontext_t *new_ctx = malloc(sizeof(ucontext_t));
	getcontext(new_ctx);
	new_ctx->uc_link=active_node->t->ctx;
 	new_ctx->uc_stack.ss_sp=malloc(MEM);
 	new_ctx->uc_stack.ss_size=MEM;
 	new_ctx->uc_stack.ss_flags=0;
	// 2) Make a new uthread_t
	uthread_t *new_thread = malloc(sizeof(uthread_t));
	new_thread->state = T_READY;
	new_thread->ctx = new_ctx;
	new_thread->tid = num_threads;
	// 3) Make a new node
	node *new_node = malloc(sizeof(node));
	new_node->t = new_thread;
	// 4) Insert into linked list
	ll_insert(new_node);
	num_threads++;
	// 5) Schedule this thread to run!
	active_node->t->state = T_READY;
	new_thread->state = T_ACTIVE;
	makecontext(new_ctx, (void*) func, 1, argp);
	swapcontext(active_node->t->ctx, new_ctx);
	// 6) Return the new uthread_t that was created
	//ll_print();
	return new_thread;
}

int uthread_get_id(void){
	//printf("get_id called\n");
	// Search through queue (linked list) 
	// for the active thread and return it's tid

	node *active_node = ll_get_by_state(T_ACTIVE);
	return active_node->t->tid;
}

int uthread_yield(void){
	//printf("yield called\n");
	// Just schedule some other thread!
	node *active_node = ll_get_by_state(T_ACTIVE);
	node *new_node = ll_get_by_state(T_READY);
	active_node->t->state = T_READY;
	new_node->t->state = T_ACTIVE;
	swapcontext(active_node->t->ctx, new_node->t->ctx);
	// return 0 if there are no errors
	return 0;
}

int uthread_join (uthread_t thread){
	//printf("join called\n");
	// Example, main thread calls uthread_join(child_thread_1) 
	//
	// 1) Find the given thread in the queue (linked list) by it's TID
	node *new_node = ll_get_by_tid(thread.tid);
	uthread_t *new_thread = new_node->t;
	// 2) Tell the given thread that it is blocking the calling
	//    thread (i.e. the currently active thread), by setting 
	//    the joining_tid
	uthread_t *active_thread = ll_get_by_state(T_ACTIVE)->t;
	new_thread->joining_tid = active_thread->tid;
	// 3) Mark the calling (currently active) thread as blocked
	active_thread->state = T_BLOCKED;
	// 4) Mark the given thread (from the queue) as active
	new_thread->state = T_ACTIVE;
	// 5) swap the context to start the given thread (from the queue)
	swapcontext(active_thread->ctx, new_thread->ctx);
	// return 0 if there are no errors
	return 0;
}
/** These are the methods you need to implement! **/