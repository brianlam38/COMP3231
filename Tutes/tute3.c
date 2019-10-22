"""
TUTORIAL 3
"""

Question 1.
-----------
- Race condition may occur if two or more threads access the critical region at the same time.
  For example if x = 0, thread A was an increment() and thread B was a decrement(), A would return x = 1
  whereas B would return x = -1. i.e. both threads are racing to access/change the resource at the same time,
  resulting in an incorrect result.
- To synchronise this result, a developer could put a lock on the critical region.
  For example, if no threads are currently in the CR + thread A requests access to the CR, A would
  execute the increment() and the CR would be locked to other threads. B would then access the CR
  after A completes its execution and releases the lock. This way, we will not run into a race condition in
  the code.

TUTE NOTES:
- Multiple threads, shared resource, typically 'global vars'.
- Outcome dpeends on the schedular of the threads.
- 'Synchronisation Primitive': Software mechanisms provided by a platform (e.g. OS) to users for 

Solution:

Question 2.
-----------

int i;

void foo()
{
    int j;

    /* random stuff*/

    i = i + 1;		-> critical region that requires mutual exclusion as i is a 'global variable'
                       > shared variable, outcome depends on the scheduling order.
    j = j + 1;		-> not part of critical region because it is a 'local variable'
    				   > local vars live on the stack. Each thread of a process has a copy of the stack.

    /* more random stuff */
}


Question 3.
-----------

void inc_mem(int *iptr)
{
    *iptr = *iptr + 1;
}

Critical Section: if *iptr is pointing to a global var, which can be accessed by multiple threads, it is in a Critical Section.
Not Critical Section: if *iptr is pointing to a local var on the stack, each thread would have a copy of on its own stack

'The answer depends entirely on the variable that *iptr is pointing to. Even though *iptr is a local var, if it is
 pointing to a global var i.e. a shared resource, then it is a critical section.'

=================================

'Primitives': Things to use to build Mutual Exclusion.

We use Locks as a synchronisation primitive / mechanism to have one thread wait for another thread to update a state.
   - Allow one thread to hold a lock to access CS at a given time.
   - E.g. if lock has been acquired, goto sleep
          else acquire lock and enter CS.

          if (lock->lk_holder != NULL)
          	{ thread goto sleep }
          else
          	{ lock->lk_holder = curthread; grab lock }

SEMAPHORES: Can do what locks do + more.
- Locks are either TRUE/FALSE
- Semaphores count the no. of resource available. i.e. if semaphore = 3, 3 resources are available.
- Two things you can do with semaphores:
  -> I want to take a resource for myself (decrement counter of avail resources by 1)
  -> Done with resource im holding (incrememnt ctr of avail resource by 1)
  -> If semaphore = 0 and someone wants resource they will be blocked.
  -> 'If only N resources available, at most N threads are holding resources at a given time'
- Producer/Consumer Problem: implement solution with two semaphores + possibly additional locks
  -> One semaphore counting emtpy space in buffer
  -> One semaphore cunting full spaces in buffer
- Down empty spaces semaphore before putting something in buffer.
- If you want to takes something out of buffer, down number of full spaces semaphore before pulling something out.

CONDITION VARIABLES: Wait for something to come true, then do something.
- E.g. Restaurant, if someone finishes eating a meal, you want to clean up what they have eaten.
  -> As a waiter, you would want to wait until they are finished with meal before cleaning up.
  -> Thread for meal eater | Thread for waiter cleaning.
  -> Waiter thread needs to wait until customer thread is finished.
  -> CONDITION VAR would be used by Customer Thread to signal that they have finished their meal.
=====================================

Question 4.
-----------
It would NOT be a lock.

	waiter() {
		acquire(L);
		cleanTable();
		release(L);
	}

	// Single customer acquires lock L before waiter() is called.

	customer() {
		eatMeal();
		release(L);
	}

Actual Solution: Using a CONDITION VARIABLE.

	int tableNeedsCleaning = 0;
	int conditionVar = 0;

	waiter() {
		acquire(L);
		while (tableNeedsCleaning == 0) {
			wait(conditionVar, L); // put thread to sleep
		}							// releases the lock L, the customer needs to get pass the Lock point
		cleanTable();
		tableNeedsCleaning--;
		release(L);
	}

	// Single customer acquires lock L before waiter() is called
	customer() {
		eatMeal();
		acquire(L);
		tableNeedsCleaning++;
		signal(conditionVar); // wake up a waiter() thread from event queue
		release(L); // release Lock L to allow woken up thread to take lock and continue
	}

Alternative Solution: Using a SEMAPHORE.

	struct sempahore numTableNeedingCleaning = sem_create(0);

	waiter() {
		down(numTablesNeedingCleaning); // decrement count if table needs cleaning
		        // if no tables need cleaning and down() is executed, semaphore = -1 and waiter() thread goes to sleep
		cleanTable();
	}

	customer() {
		eatMeal();
		up(numTablesneedingCleaning);
	}

Question 5.
-----------

USE SEMAPHORES AS THE SOLUTION. (see above)


Question 6.
-----------

Wake up all threads at the same time.
- Why would you do this?
  -> All threads cant progress until buffer is filled up with info that is read from disk.
  -> They all are waiting on a condition var that is corresponding to the corresponding that buffer is ready to use.
  -> When OS finishes retrieving, the thread that it returns to may signal and wake up all the threads.
  -> A 'CV broadcast' will be performed rather than a 'CV signal'.
     > They will all wake up and continue at the same time rather than just one.

Question 7.

'Deadlock': thread is never woken up.






















