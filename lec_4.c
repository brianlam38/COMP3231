============================================================================================
LOCKS REVIEW
============================================================================================

increment/decrement problem
- synchronise using allocated lock to guarantee mutual exclusion.

============================================================================================
CONDITION VARIABLES
============================================================================================

Locks dont provide a mechanism for waiting for a particular event, other than the release of a lock itself.

Evoke wake operation when condition variable is TRUE.
- cv_wait(struct cv *cv, struct lock *lock)
- lock is released
- when cv_wait() returns, lock is acquired again

Two 'wake operations':
1. cv_signal(struct cv *cv, struct lock *lock)  -> wait on one thread
2. cv_broadcast(struct cv *cv, struct lock *lock) -> wakes everything that is waiting

Condition Vars and Bounded Buffers (slide 59):
- 'Non-Solution'
  -> Simple Lock without CV: Going to sleep while holding the lock, leading to deadlock
- 'Solution'
  -> Using cv_wait() primitive which releases the lock when we call it.
  -> Why use a while() instead of if()?
     > no guarantee that another thread hasnt consumed what you were originally woken up to consume
     > this is double checking that the condition is still TRUE
     > 'always should test condition with WHILE for condition variables + locks'

Producer-Consumer Problem:
- while (count == N) { cv_wait(full, 1 ) }   -> while buffer is full, wait on full CV and pass in lock
- while (count == 0) { cv_wait(empty, 1) }   -> while buffer is empty, wait on empty CV

'Dining Philosophers Synchronisation Problem'
SOLUTION with Semaphores:
- 'MUTEX': Mutual exclusion object created so that multiple threads can take turns sharing the same resource.
- Use semaphore to implement mutual exclusion. (semaphore to guarantee mutual exclusion)

'Readers and Writers problem'
- Models access to a database. e.g. airline reservation system
- Can have more than one concurrent reader.
-Idea: use a mutex to guanratee mutual exclusion of state update of a count variable.

============================================================================================
DEADLOCKS
============================================================================================

Resources
- Allocation of resources needs to be in a reasonable order.
- 'Pre-emptable resources': can be taken away frmo a process with no ill effects.
  'Non-preemptable resources': will cause problems if taken away

Deadlock
- Thread #1 holding resource A and requiring B
  Thread #2 holding resource B and requiring A
  ^'Circular Dependency' will occur

Two example resource usage pattern:
- RHS is deadlock prone because both resources are waiting on each other for a resource.
- LHS ordering of requests is NOT deadlock prone -> 'resource ordering' to prevent deadlock prevention

Formal Definition: 'Set of processes is deadlocked if each process in the set is waiting
                    for an event that only another process in the set can cause'
- None of the resources in the SET can RUN, RELEASE RESOURCES or BE AWAKENED.

Conditions for Deadlock to occur:
1. 'Mutual exclusions condition'
   -
2. 'Hold and wait'
   - Deadlock only occurs if I need more than one resource
   - hold the resource you have thus far, and wait for the one I dont have yet
3. 'No preemption condition'
   -
4. 'Circular Wait Condition'
   - Must be a circular chain of 2 process or more.

Deadlocks arent easy to identify.
A lot of problems may seem like it is fault of a deadlock, but not.

Annoying thing about deadlocks is that it does not always happen.
- i.e. if a dealock occurs only 1/10000 chance, then it will be hard to reproduce the problem.

Strategies for dealing with Deadlock:
1. Ignore the problem entirely.
2. 'Prevention': negate one of the four necessary conditions, then deadlock wouldnt occur
3. Detection and Recovery
4. 'Dynamic Avoidance': careful resource allocation

Approach 1: The Ostrich Algorithm
- Pretend there is no problem
- Mathematician vs. Engineer
  -> Deadlocks are reasonable if deadlocks occur rarely + cost of prevention is high
- UNIX / WINDOW takes this approach for some of the more complex resource relationships to manage
- Trade off between 'Convenience (engineering)' vs 'Correctness (mathematical)'

Approach 2: Deadlock Prevention
- Prevent:
  -> 'Mutual Exclusion': not feasible in general - some devices/resources are instrinsically not shareable.
  -> 'Hold and Wait': Can kind of tackle, but not ideal of the four to prevent.
      > Tie up resources for longer than we might like.
      > Prone to 'Livelock': never get resources I need, because always am unlucky and find one that is always held by someone else.
            Always changing state doing something, but never make any progress to complete.
    
    |'Livelock': set of processes that are not blocked, run regularly, but never make progress.
    | -> E.g. two people passing each other that attempt to step out of each others way in the same direction.
    |         BOTH are changing state + BOTH never pass each other. They are in livelock.

	while (TRUE) {
		flag[i] := TRUE;
		while (flag[j]) { wait };
		// critical section
		flag[i] := FALSE;
	}

	^Livelock occurs above as both are changing state constantly but never making any progress

   -> 'No Preemption': Not a viable option
      > Causing failure to important operation, definitely not an option. e.g. printer halfway printing and then cut short.
   -> 'Circular Wait': RESOURCE SORDERING IS A COMMON TECHNIQUE IN PRACTICE
      > You should understand the resource ordering expectations of the system
      > Making sure you dont 'VIOLATE RESOURCE ORDERING' to prevent deadlocks.

Approach 3: Detection and Recovery
- Let it happen and come up with a mechanism + come up with recovery mechanism if issue comes up.
- Detection with Multiple Resources of each type:
  
  Example for deadlock detection algorithm:

  How can we tell if the system will deadlock by looking at the Matrix? YES.
  -> STEP #1: Mark rows/processes which are '=' or '<' what is available
  -> STEP #2: Update resources + Repeat process above
  -> STEP #3: If all rows/processes can be marked off, the system is NOT deadlocked, else the system is deadlocked.




















