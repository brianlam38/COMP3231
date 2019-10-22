==============
QUESTION 1
==============

-----
Page Table

'F - smaller page size will lead to a smaller page table.'

	Example: 32bit address space mapped to 2Kb sized pages
	2Kb = 2000 bytes = 2^11 bytes
	no. of pages required = 2^32 / 2^11 = 2^21 pages
	1 page = 25bits (page no. | valid | dirty | protect| ref | caching) = 4 bytes per entry.
	Page Table Size = 4bytes * 2^21 pages = 2^2 * 2^21 = 2^23 bytes = 8,388,608 bytes = ~8MB PAGE TABLE
	'Smaller Page Size = More Entries Needed = Larger Page Table'

'T - smaller page size will lead to more TLB misses.'

	Smaller pages mean that it is less likely for a Page to have the address we are looking for,
	therefore leading to more TLB misses.

'T - smaller page size will lead to fewer page faults.'

	Page Faults = an exception where a there is no valid entry found for a virtual memory
				  in the Page Table, so must be retrieved from DISK and loaded into the Page Table
				  rather than simply retrieved from RAM.

	'READ QUORA ANSWER "WHAT IS A PAGE FAULT IN AN OPERATING SYSTEM"'

	ANSWER:
	In practice, initial faulting-in is usually less important than page faults generated once the app is up and running.
	Once the app is up and running, reducing page size results in a more accurate determination of work set.

	Having smaller page sizes means that there is more granularity, allowing the OS to perform more targeted swapping.

'T - smaller pages should reduce I/O throughput'

	Smaller pages should reduce I/O throughput as it requires additional access times to read()/write() memory
	to more pages.

------
Threads

'T - threads are cheaper to create than processes'

	Yes they are cheaper.

'F - Kernel-implemented threads are cheaper to create than user-level implemented threads'

	User-Level implemented threads are cheaper to create, as trap() to Kernel is not required.

'F - A blocking Kernel-implemented thread blocks all threads in the process'

	No, only a blocking User-implemented thread will block all threads in the process.

'T - Threads are cheaper to context switch than processes'

	Yes. We dont have to save the address space with a Thread context switch.

	Thread Context Switch: virtual memory address space remains the same
	Process Context Switch: virtual memory address space does NOT remain the same

	When changing virtual memory address spaces, the TLB must be flushed making memory accesses more expensive.

'F - Different User-level threads of the same process can have different scheduling priorities in the Kernel'

	No.

	User-Level threads arent scheduled, they yield() the CPU. Only Kernel-Level threads are scheduled.

'T' - All kernel-scheduled threads of a process share the same virtual address space.

		(Refer back to Thread / Process slides)

---------
Page Replacement Algorithms

'F - The optimal page replacement algorithm is the best choice in practice'

	No. It is impossible to implement in practice.

---------
OS Resource Allocation

'F - The OS is NOT responsible for resource allocation between competing processes'

	No.

	One of the OS main responsibilities is to be a resource manager: to ensure no starvation,
											progress for all processes, allocation according to desired policy.

---------
Syscalls

'F - Syscalls do not change the privilege mode of the processor'

	No.

	Syscalls trap() into the Kernel so we do change the privilege mode of the processor.

---------
I/O Management or Scheduling

'T - A scheduler favouring I/O bound processes usually does not significantly delay the completion of CPU-bound processes'

	The I/O processes get the I/O and then yield() the CPU again.

==============
QUESTION 2
==============

Addr_Translation() {
	// TLB hit
	if Page in TLB:
		return get_physaddr(TLB[page])
	// TLB miss -> Page Walk required
	else
		if Page in PT:
			updateTLB()
			return get_physaddr(PT[page])
		// Page fault -> Access to Disk required
		else
			readpage_from_disk(Disk[page])
			transferpage_from_disk_to_mem(Disk[page])
			// Page replacement
			if memory is FULL:
				page_replacement()
			updatePT()
			// Re-read TLB entry
			Addr_Translation()
}

'PART 1 - Effective Memory Access Time'

Paging Disk:
- Ave access time & transfer time = 5 ms / page

Page Table (within main memory):
- Access time = 100 ns / memory access
- Each memory reference through Page Table = 2 accesses

48-entry TLB to speed up accesses.

99% memory accesses = TLB hit
1% -> 5% (0.05% of total) = page faults

What is the Effective Memory Access Time?

Effective Memory Access Time = (HitRate Mem1) * (Access Time Memory 1) + (1 - HitRate Mem2) * (Access Time Memory 2)

	H = 0.99 | T1 = 100 ns * 2 acceses = 0.0002 ms
	H = 0.99 | T2 = 5 ms

	T(eff) = (0.99 * 0.0002) + ((1 - 0.99) * 5)
		   = 0.00019800000000000002 + 0.05
		   = 0.050198


	PAST PAPER ANSWER:
	- Paging Disk: 5 ms per page access/transfer
	- Page Table: 100 ns per access (each ref = 2 accesses)
	- 99% TLB HIT | 1% TLB MISS | 0.05% PAGE FAULT
	- EFFECTIVE MEMORY ACCESS TIME = (0.99 * 100 ns) + (0.0095 * 2 * 100 ns) + (0.0005(3 * 100 ns + 5ms))
									'TLB hit'	 'TLB miss -> Page Table'  'Page Fault -> Disk Read'

	'ADD TLB->PT->DISK DIAGRAM TO NOTES'

	PAGE FAULT -> DISK READ SCENARIO:
	A memory reference to update the page table + memory reference for the hardware to re-read the same entry to refill the TLB.

'PART 2 - Files'

	// SUGGESTED ANSWER #1
	Many applications read the first few bytes of a file, for instance to determine the file type.
	If bytes were not in the same disk block as the inode, then the application would need to:
	1. Read inode block
	2. Find address of block within data
	3. Send another request to disk to read start of file.
	VS.
	One request to get the starting data as needed.

	// SUGGESTED ANSWER #2
	A significant fraction of files are small, so most of them can be stored in the inode entirely, reducing the need to
	seek() to the inode and then seek() to the data. It is all in the inode.

==============
QUESTION 3
==============

'PART 1 - Disk-Scheduling Alogrithms'

	192 tracks: 0 - 191 -> serving a request at track #80. Finished #62.

	Queue: 119, 58, 114, 28, 111, 55, 103, 30, 75

'PART 2 - Threads / Scheduling'

	Co-operative scheduling is a system of scheduling where the threads yield() to allow other threads to have a go.
	- Commonly used because user-level code normally has no access to timers, which means there
	  is no way to interrupt the thread when it runs for too long, so it relies on cooperation from other threads to switch.
	- Sometimes, user-level threads have access to timers, but the units of time are not small enough to give illusion of parallelism.

==============
QUESTION 4
==============

'Virtual Memory'

	32 bit virtual address in a 2-level page table:

	[10 bit master page #][10 bit secondary page #][12 bit offset]
	'need more clear explanation of 2-level page table - outlne this in notes'

==============
QUESTION 5
==============

'PART 1 - EXTERNAL vs. INTERNAL FRAGMENTATION'

	Internal Fragmentation:
	-> Fragmentation caused by processes being allocated more memory than needed. This is due to
	   memory being divided up into fixed-size blocks.
	-> Internal fragmentation can be addressed by using dynamic allocation, where memory is allocated
	   for the exact size needed by the process.

	External Fragmentation:
	-> Fragmentation caused by processes being allocated in a non-contiguous (adjacent) manner, where the combination
	   of free memory may be able to service a process but cannot be serviced as the free memory is split up by used memory.
	-> This is due to memory being allocated dynamically but no memory-compact method is used or more focused allocation towards
	   a specific area of memory is used.
	-> External Fragmentation can be addressed 'Compaction', 'Paging' and 'Segmentation'.

	'SEE QUORA RESPONSE'

	// SUGGESTED ANSWER:
	Internal Fragmentation is when space INSIDE an allocated region is wasted.
	External Fragmentation is when space OUTSIDE an allocated region is wasted.

'PART 2 - Synchronisation Primitives'

	NFI.

	Test-and-Set Spinlock vs.
	Read-Before-Test-and-Set-Spinock

	NO NEED FOR THIS.

==============
QUESTION 6
==============

Four conditions for deadlock to occur:

1. Mutual Exclusion: resource cannot be shared with another thread/process

2. Hold and Wait: am holding the resource while waiting for another one / wont release until it can acquire another resource.

3. No Pre-Emption: cannot be pre-empted by another thread/process

4. Circular Wait: a chain of processes where each process is waiting for a resource that is being held by another process.
                  ^could be all processes just waiting on one process who is holding on a resource.

Preventing the Circular Wait Condition is a common method to prevent deadlocks.

'Require that each process must request resources in an increasing order.'

For example, Dining Philosophers: 5 chopsticks 1,2,3,4,5

Philosophers require 2 chopsticks to eat.
- Mutual Exclusion: Each chopstick can only be used by 1 philosopher at a time.
- No Pre-emption: Philosophers cannot take chopsticks from another philosopher.
- Hold-and-Wait condition: Philosopher has 1 chopstick, but doesnt have second so holds onto the first while it waits for the second.



	  'P5'    
	[5]  [1]
 'P4'  O    'P1'
	[4]	 [2]
	  [3]
   'P3'	 'P2'

'RESOURCE ORDERING SOLUTION:'
- Anytime a process requests a resource, it has to request it in a global (applying to all processes) specified order.








