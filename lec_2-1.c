====================================================================================================
PROCESSES AND THREADS INTRODUCTION
====================================================================================================

'Processes (Task/Job)': Execution of an individual program.
- Encompasses one or more threads.
'Threads': A unit of execution that belongs to a program
- Can be traced: list the sequence of instructions that execute

'Dispatcher': is a module that gives control of the CPU to the process selected by the short-term schedular.
			  It is an important component that is involved in the CPU-scheduling function.
	 		  It receives control in kernel mode as the result of an interrupt or system call.
	 		  The functions of a dispatcher involve:
- Context Switches: in which the dispatcher saves the state of the process/thread that was previously running.
                    The dispatcher then loads the initial or previously saved state of the new process.
- Switching to user mode.
- Jumping to the proper location in the user program to restart that program indicated by its new state.

Process and Thread models of selected OSes:
- Single Process, Single-thread: MSDOS
- Single Process, Multi-thread: OS/161
- Multi-Process, Single-thread: Traditional Unix
- Multi-Process, Multi-thread: Modern Unix (Linux), Microsoft

====================================================================================================
PROCESSES CREATION + TERMINATION
====================================================================================================

Principle events that cause process creation.
1. 'System Initialisation': Foreground (interactive) and Background processes (e.g. Email Server, Web Server)
2. 'Execution of process creation syscall by a running process': New login shell for incoming ssh connection.
3. 'User request to create a new process'
4. 'Initiation of a batch job'
^All these cases use the same system mechanism to create new processes.

Process Termination:
1. 'Normal Exit': voluntary
2. 'Error Exit': voluntary
3. 'Fatal Error': involuntary
4. 'Killed by another process': involuntary

====================================================================================================
PROCESSES/THREAD STATES + TRANSITION
====================================================================================================

Process/Thread States - Three states are either 'RUNNING' 'BLOCKED' 'READY'
The transition between states is shown below:
1. [RUNNING]->[BLOCKED]: Process blocks input
2. [RUNNING]->[READY]: Schedular picks another process
3. [READY]->[RUNNING]: Schedule picks this process
4. [BLOCKED]->[READY]: Input becomes available to process

Transition causing events:
- [RUNNING]->[READY]: Voluntary yield(), end of timeslice.
   -> 'Timeslice': period of time which a process is allowed to run.
- [RUNNING]->[BLOCKED]:
   -> Waiting for 'INPUT' (e.g. file, network)
   -> waiting for 'TIMER'
   -> waiting for 'RESOURCE' to be available

====================================================================================================
PROCESSES SCHEDULAR/DISPATCHER + QUEUES
====================================================================================================

'Schedular / Dispatcher': has to choose a [READY] process to run.
- How does it choose a process to run?
- It is inefficient to search through all processes.
- 'THE READY QUEUE':

 				   QUEUE
ENTER -----> [ ][ ][ ][ ][ ][ ] -------> DISPATCH ------> |===========| -------> EXIT
  ^ 													  |	PROCESSER | 
  | 													  |===========| ---
  | 							PAUSE		 				  			  |
  ------------------------------------------------------------------------|

- Process goes through queue, is dispatched, gets control of processor, EXITS/COMPLETES.
- Process goes through queue, is dispatched, gets control of processor, PAUSES/WAITS FOR THINGS, RE-ENTER QUEUE.

- What about 'Blocked Processes'?
  -> When an 'Unblocking Event' occurs, we also wish to avoid scanning all processes to select one to make '[READY]'

- Using 'TWO QUEUES':

						"=== SINGLE BLOCKED QUEUE ==="

 				   QUEUE
ENTER -----> [ ][ ][ ][ ][ ][ ] -------> DISPATCH ------> |===========| -------> EXIT
  ^ 													  |	PROCESSER | 
  | 													  |===========| ---
  | 							TIMEOUT		 				  			  |
  ------------------------------------------------------------------------|
  |     																  |
  |             BLOCKED QUEUE 				  EVENT WAIT                  |
  |---------- [ ][ ][ ][ ][ ][ ] <----------------------------------------|
EVENT
OCCURS

						"=== MULTIPLE BLOCKED QUEUES ==="

 				   QUEUE
ENTER -----> [ ][ ][ ][ ][ ][ ] -------> DISPATCH ------> |===========| -------> EXIT
  ^ 													  |	PROCESSER | 
  | 													  |===========| ---
  | 							TIMEOUT		 				  			  |
  ------------------------------------------------------------------------|
  |     																  |
  |             EVENT 1 QUEUE 				  EVENT 1 WAIT                |
  |---------- [ ][ ][ ][ ][ ][ ] <----------------------------------------|
EVENT 1																	  |
OCCURS 																	  |
  |     																  |
  |             EVENT 2 QUEUE 				  EVENT 2 WAIT                |
  |---------- [ ][ ][ ][ ][ ][ ] <----------------------------------------|
EVENT 2
OCCURS
  |     																  |
  |             EVENT N QUEUE 				  EVENT N WAIT                |
  |---------- [ ][ ][ ][ ][ ][ ] <----------------------------------------|
EVENT N
OCCURS


====================================================================================================
IMPLEMENTATION OF PROCESSES
====================================================================================================

'Process Control Block (PCB)' stores a processes information.
- The PCB is a data structure maintained by the OS for every process, identified by a int process_ID (PID).
- A PCB keeps all the info needed to keep track of a process.
- PCBs form a 'Process Table'.
  -> Sometimes the Kernel stack for each process is in the PCB.
  -> Sometimes process info is on the Kernel stack. e.g. registers in trapframe in OS/161

Example fields of a 'Process Table' entry:
(i.e. each PCB would have this sort of info to keep track of a process)

PROCESS MANAGEMENT   |  MEMORY MANAGEMENT    |  FILE MANAGEMENT
-------------------------------------------------------------------
registers               Ptr to text segment     Root dir
program counter			Ptr to data segment     Working dir
stack ptr 				Ptr to stack segment    File descriptors
process state 									User ID
priority 										Group ID
parent process
.
.
and more.

====================================================================================================
THREADS
====================================================================================================

'Thread': The smallest sequence of programmed instructions that can be managed independently by a schedular.
- Is a component of a process
- Multiple threads can exist in a process, executing 'concurrently' + 'sharing resources'
- Threads of a process share its executable code + values of its variables at any given time.

MODEL 1: 'Thread Model' Hamburger restaurant analogy
- 'Single-Threaded':
['WAIT FOR CUSTOMER'] -> [CUSTOMER ARRIVES] -> [TAKE ORDER] -> [START FRIES] -> ['FRIES COOK'] -> [FRIES FIN] -> [START BURGER] -> ['BURGER COOK'] -> [BURGER FIN] -> [ASSEMBLE ORDER] -> [SERVE CUSTOMER]
- 'Multi-THreaded':
THREAD #1: ['WAIT FOR CUSTOMER'] -> [CUSTOMER ARRIVES] -> [TAKE ORDER] -> [ASSEMBLE ORDER] -> [SERVE CUSTOMER]
THREAD #2: [START FRIES] -> ['FRIES COOK'] -> [FRIES FINISH]
THREAD #3: [START BURGER] -> ['BURGER COOKS'] -> [BURGER FINISHED]

MODEL 2: 'Finite State Machine Model' (FSM) (event-based model):
- An abstract machine that can be exactly one of a finite number of states at a given time.
- Changes from one state to another state in response to input events = 'Transition'
- A FSM is defined by a list of states, its initial state and conditions for each transition.

THREAD MODEL vs. FINITE STATE MACHINE MODEL:
- 'Thread Model': State implicitly stored on the stack
- 'Finite State Model': State explicity managed by the program

====================================================================================================
THE THREAD MODEL
====================================================================================================

In the thread model, each thread has its own stack.
- 'Local vars' are per thread.
  -> Allocated on the stack
- 'Global vars' are shared between all threads.
  -> Allocated in .data segment
  -> Concurrency control is an issue
- 'Dynamically allocated memory' (malloc) can be global OR local
  -> Defined by the program (ptr can be global or local)

Thread usage example: A word processor with three threads
- THREAD #1: Keyboard
- THREAD #2: Text render
- THREAD #3: Disk

Thread usage example: A multi-threaded web server
- THREAD #1: Dispatcher thread, to manage the work for a thread.
- THREAD #2: Worker 2 thread executing instructions for Web Server process.
- THREAD #N: Worker N thread executing instructions for Web Server process.

^Rough outline of code for multi-threaded web server:
 Web Server to return page requested by user.
// Dispatcher Thread
while (TRUE) {
	get_next_request(&buf);
	handoff_work(&buf);
}
// Worker Thread - can overlap disk I/O with execution of other threads
while (TRUE) {
	wait_for_work(&buf);
	look_for_page_in_cache(&buf, &page);
	if (page_not_in_cache(&page)) {
		read_page_from_disk(&buf, &page);
	}
	return_page(&page);
}

Three ways to construct the Web Server:
1. 'Threads Model': Parallelism, blocking system calls
2. 'Single-threaded process': No parallelism, blocking system calls
3. 'Finite-State Machine Model': Parallelism, nonblocking system calls, interrupts

Why Threads? Summary:
- 'Simpler to program' than a state machine.
- 'Less resources' are associated with them than a complete process.
  -> Cheaper to create and destroy threads
  -> Shares resources (especially memory) between threads
- 'Performance': Threads waiting for I/O can be overlapped with computing threads
  -> NOTE: If all threads are 'compute bound', then there is no performance improvement
           i.e Threads on a computer with a SINGLE CPU
- Threads can 'take advantage of parallelism' available on machines with more than on CPU e.g. QUADCORE CPU.




















