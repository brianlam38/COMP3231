====================================================================================================
CONCURRENCY AND SYNCHRONISATION
====================================================================================================

'Race Condition': Occurs when two or more threads access shared data and try to change it at the same time.
                  Because thread scheduling can swap between threads at any time, you dont know the order
                  in which threads attempt to access the shared data. Therefore, result of change in data
                  is dependent on the thread scheduling algorithm.
                  'i.e. both threads are racing to access/change the data'


Problems often occur when one thread does a "check-then-act"
(e.g. "check" if the value is X, then "act" to do something that depends on the value being X)
and another thread does something to the value in between the "check" and the "act".

Example Race Condition:

	if (x == 5) // The "Check"
	{
	   y = x * 2; // The "Act"

	   // If another thread changed x in between "if (x == 5)" and "y = x * 2" above,
	   // y will not be equal to 10.
	}

The point being, y could be 10, or it could be anything, depending on whether another
thread changed x in between the check and act. You have no real way of knowing.

In order to prevent race conditions from occurring, you would typically put a lock around the shared
data to ensure only one thread can access the data at a time. This would mean something like this:

	// Obtain lock for x
	if (x == 5)
	{
	   y = x * 2; // Now, nothing can change x until the lock is released. 
	              // Therefore y = 10
	}
	// release lock for x

====================================================================================================
CRITICAL REGION
====================================================================================================

'Critical Region' is a region of code where shared resources are accessed.
- Correctness relies on the shared resource NOT being concurrently modified by another thread/process.
- Incorrect access results in a race condition: incorrect behaviour, deadlock, lost work etc.

Critical Region Solutions:
- Conditions required of any solution to the critical region problem:
  1. 'Mutual Exclusion': No two processes simultaneously in a critical region.
  2. 'No assumption made about speeds or numbers of CPUs'
  3. 'Progress': No process running outside the critical region may block another interested process from entering when the critical region is free.
  4. 'Bounded': No process should wait for a resource / enter its critical region for infinite amount of time.
     -> 'Bounded Waiting': There is a limit for no. times other processes can enter into the critical region. After the limit
     					   is reached, the system must grant the requesting process permission to get into the critical region.
     					   The purpose is to make sure every process gets a chance to actually enter the critical region.
     					   Prevents process 'starvation'.

Possible solution to CR #1: 'LOCK VARIABLE'.
  -> lock = 1: someone is in the critical region and we must wait.
  -> lock = 1: nobody is in the critical region and we are free to enter.
- Example lock implementation:

  while(TRUE) {
  	while(lock == 1);
  	lock = 1;
  	critical();
  	lock = 0;
  	non_critical();
  }

  ^Problem with solution: 'REFER TO LEC VIDEO'

- Unfortunately is easier to show that something doesnt work, rather than prove it does work
  We would like to prove that our solutions work.

Possible solution to CR #2: 'MUTUAL EXCLUSION BY TAKING TURNS'
- Each process takes turns to access the CR.
  Example: Process 0 and Process 1

  // Process 0
  while (TRUE) {
  	while (turn != 0);		// not process 0s turn
  	critical_region();
  	turn = 1; 				// pass token to next process turn
  	noncritical_region();
  }

  // Process 1
  while (TRUE) {
  	while (turn != 1);		// not process 1s turn
  	critical_region();
  	turn = 0;				// pass token to next process turn
  	noncritical_region();
  }


- Works due to 'strict alteration': each process takes turns.
- Cons:
  -> busy waiting
  -> processes must wait its turn even while the other process is doing something else.
     > With many processes, must wait for everyone to have a turn.
       > Does not guarantee progress if a process no longer needs a turn.
     > Poor solution when processes require the CR section at differing rates.

- See 'Petersons Solution' (textbook)

Possible solution to CR #3: 'MUTUAL EXCLUSION BY DISABLING INTERRUPTS'
- Before entering CR, disable interrupts | After leaving CR, enable interrupts
- Pros:
  -> Simple
- Cons:
  -> Only available in the kernel.
  -> Blocks everybody else, even with no 'resource contention' (multiple processes attempt to use the same shared resource)
     > Slows interrupt response time.
  -> Does not work on a multiprocessor.

Hardware support for mutual exclusion:
- 'Test and Set Instruction'











