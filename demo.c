
/* demo.c */

/*
The demo breifly demonstrates some of the core functionality of ucontext
Note: ucontext is deprecated on some platforms 
(because the decision makers for platforms are pedantic fools)
To compile without all of those annoying deprecated warnings:


gcc demo.c -o demo -Wno-deprecated
*/

// This define is necessary only on macOS.
#define _XOPEN_SOURCE 600

#include <stdio.h> 	// Provides printf
#include <stdlib.h>	// Provides malloc
#include <ucontext.h>

#define MEM 1048567 // Amount of memory required for thread stack
// If this number is too small it will cause some really weird errors.


// Main and T1 are a global and fully allocated
ucontext_t T1, Main;

int fn1() // The function for the first thread
{
 printf("this is from 1\n");
 setcontext(&Main);
 return 0; // This never executes because of the setcontext before it
}

void fn2() // The function for the second thread
{
 printf("this is from 2\n");
 setcontext(&Main);
}



int main(int argc, char *argv[])
{

 getcontext(&Main);
 
 getcontext(&T1);
 T1.uc_link=0;
 T1.uc_stack.ss_sp=malloc(MEM);
 T1.uc_stack.ss_size=MEM;
 T1.uc_stack.ss_flags=0;
 makecontext(&T1, (void*)&fn1, 0);
 swapcontext(&Main, &T1);

 // T2 is a pointer to a context,
 // in order to allocated the space for the actual ucontext_t struct
 // it is necessary to use malloc (no type-cast necessary)
 ucontext_t *T2 = malloc(sizeof(ucontext_t));
 getcontext(T2);
 T2->uc_link=0;
 T2->uc_stack.ss_sp=malloc(MEM);
 T2->uc_stack.ss_size=MEM;
 T2->uc_stack.ss_flags=0;
 makecontext(T2, (void*)&fn2, 0);
 swapcontext(&Main, T2);
 printf("completed\n");
 exit(0);
}


