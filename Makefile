all:
	gcc counter.c uthread.c -o counter
	gcc thread-hello.c uthread.c -o hello
	# gcc demo.c -o demo

debug:
	# gcc -g counter.c uthread.c -o counter
	gcc -g thread-hello.c uthread.c -o hello
	# gcc -g demo.c -o demo



