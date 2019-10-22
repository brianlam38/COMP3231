"""
TUTORIAL 4 (week 5)
"""

NOTES:
- Learn the specific definitions of 'Deadlock', 'Livelock', 'Starvation'.

Question 1.
-----------
Dining Philosophers problem.
'Deadlock': A set of processes in deadlock waiting for an event to occur

This is a LIVELOCK problem.
They cant ever finish their meal.

Question 2.
-----------
'Starvation' occurs when a process cannot attain a resource because the schedular always allocates
resources to other processes, denying the starved process from progressing.


Question 3.
-----------
This is a 'Livelock' problem described, where a set of processes are NOT BLOCKED, RUN REGULARLY
but NEVER MAKE ANY PROGRESS.

Implement 'Resource Ordering', where one process can only read() after they attain seek().

Example flow:

1. Proc1 attains read() resource
2. Proc2 is blocked from read() as it is busy, hence blocked from seek() resources.
3. Proc1 finishes read() + attains seek() resource
4. Proc2 attains read() resource + blocked from seek() as it is busy
5. Proc1 finishes seek() resource.
6. Proc2 attains seek() resource

Question 4.
-----------
Conditions required for a deadlock:
1. Mutual Exclusion Condition:
2. Hold and Wait Condition:
3. No Pre-Emption Condition:
4. Circular Wait Condition:

Attacks on each Condition for a deadlock:

Question 5.
-----------

(a)
'Maximum Demand': 

CALCULATE MATRIX: MAX_DEMAND - CURRENT_ALLOC

STILL NEEDS
-----------
p1 0  0  0  0
p2 0  7  5  0
p3 6  6  2  2
p4 2  0  0  2
p5 0  3  2  0

(b)
AVAILABLE
-------------
   2  1  1  2

STILL NEEDS
-------------
p1 0  0  0  0 -> X
p2 0  7  5  0 -> X
p3 6  6  2  2 -> X
p4 2  0  0  2 -> X
p5 0  3  2  0 -> X

'Scheduling Order': P1, P4, P5, P2, P3

(c)
In the example, it is NOT deadlocked.

(d)
None will be deadlocked.

(e)
i:   No it cannot be safely granted immediately.
ii:  
iii:



------
KAHOOT
------
'Can multiple resources utilise a resource but not have deadlocks or get into a race condition?'

Shared resource that are only ever read by multiple processes, there is no need to put
a lock on resources, because that resource will never change.

Example: Buffer with an array of chars.
- Multiple threads can concurrently read from the buffer at the same time.
- Each will be given a timeslices / execution slices to read things out of the buffer.
- Since no process is 
- MUTUAL EXCLUSION ISNT ALWAYS NEEDED, but sometimes is needed.

Deadlock has four conditions and one of them is MUTUAL EXCLUSION.

'Hold and Wait QN'
There will never be a situation when a process will hold on resource then wait for additional resources.

If a process ever needs resources, it will request them all at the same time.

'Pre-Emption Condition'
Pre-Emption is NOT a feasible way to prevent deadlock.

Example: Each print job is its own process.
- Forcibly removing a process from using the printer halfway through the printing process
  will cause a half-printed document.

'Circular Wait'
It is feasible to remove the Circular Wait condition using 'RESOURCE ORDERING' as the solution.

'Resource Releasing'
You have to release in the 'REVERSE ORDER'.



