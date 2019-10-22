####################################
====== 'OS INTRO AND OVERVIEW' =====
####################################

	TWO ROLES OF AN OS:
	#1 'Abstract Machine': hides hardware details, provied abstraction to programmers and apps. 
	#2 'Resource Manager': ensure no starvation, progress for all processes, allocation according to desired policy.

	OS COMPONENTS
	-> 'Kernel': runs in privileged mode, mediates access to resources (CPU, RAM, IO DEVICES)
	-> 'User'  : interacts with Kernel via. System Calls.

############################
====== 'SYSTEM CALLS' ======
############################

	SYSTEM CALL:
	-> A mechanism to cross between the 'User-Level' and 'Kernel-Level'.

	STEPS IN MAKING A SYSCALL: read() example
	----- /* User Program */ -----
	#1 Push parameters on the stack: push nbytes, &buffer, fd
	#2 Invoke the syscall read()
	----- /* Library Procedure read() */ -----
	#3 Put code for read() on the register so the OS knows what to look for + trap()/switch to the Kernel.
	#4 'Dispatcher/Schedular' receives control in Kernel Mode as a result of trap(), performs context switch.
	   'Context Switch': Dispatcher saves the state of prev process that was running then loads the
	   					 initial/previously saved state of the new process.


/* consume() is called by a consumer to request more data */
struct item consume(void) {
	// 1. DOWN full and mutex -> if num_full=0 OR mutex=0, block consumer thread
	P(num_full); P(mutex);
	// 2. consume item from buffer head
	struct item data = buffer[head]
	// 3. item consumed, UP empty and mutex
	V(num_empty); V(mutex);
	return data;
}
/* produce() is called by a producer to add more data */
void produce(struct item i) {
	// 1. DOWN empty and mutex -> if num_empty=0 OR mutex=0, block producer thread
	P(num_empty); P(mutex);
	// 2. add item to buffer
	buffer[tail] = i;
	// 3. item added, UP full and mutex
	V(num_full); V(mutex);
}






