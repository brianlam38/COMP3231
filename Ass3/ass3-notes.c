One of the components of ass3:
- Writing something that manages availability of memory

Kernel System allocator needs to be implemented
- Allocator that runs on top of another allocator, that allocates 4k frames at a time.
- Part of this ass3 will be implementing the allocator that allocates 4k chunks at a time
  and implements something that frees those 4k chunks (frame table)

=============================================
POINTER TECHNIQUES
=============================================

Pre-cusor to ass3: Pointer techniques:
	char *c; // ptr to a char c
	c = 5; 	 // assign address of c a value of 5 (c address @ 5)
	*c = x;  // de-reference val and assign "x" to char c

	int *i;  // ptr to an int
	i = 4;   // assign address of i a value of 4 (i address @ 4)
	*i = 42; // de-reference val and assign "42" to int i

Convert index of entry -> physical address associated with the slot:
	frame_num = p_addr >> 12; // frame no. = physical addr  shifted 12 bits to the RIGHT.
	p_addr = frame_num << 12; // physical addr = frame no. shifted 12 bits to the LEFT.
							  		// physical address = [ 20bit frame number ][ 12bit offset ]

Indexing off pointers:
- You can declare a pointer, assign it to a location, then index it as an array without declaring it as an array.
- You can just index off any pointer.
- Amount in memory of the 'stride/step' is based off the type you declared.
	char *c;
	c = 5;
	c[0] = 'h'; c[1] = 'i';

	int *i;
	i = 4; i[0] = 42; i[2] = 7;	// i[0] addr = base address = #5
								// i[2] addr = base address + 2*4bits = #13

	Example of different sized 'strides': 16bit memory address (0 - 15)
	- char: 4bits (1byte) each
	- int: 32bits (4bytes) each

							  i[0]			 i[1] 			 i[2]
						|			   |			   |			   |
				        |      42      |               | 	  7	       |
	[0 ][1 ][2 ][3 ][4 ][5 ][6 ][7 ][8 ][9 ][10][11][12][13][14][15]
					         'h' 'i'
					        c[0]c[1]

Bump pointer allocator:
- Allocator that keeps track of location in memory of the highest thing allocated so far.

	char *c1, *c2, *c3;
	unsigned int next_free; // the bump pointer
- Example:
	Initial memory with nothing in it: bump pointer allocator = 0
	Allocate a 3-byte string:
	-> Bump the bump-ptr by 3 = location of the next thing that would be allocated.
- So everytime we call the allocator, we can always use where it points to at the moment as free memory.
  and we bump it by however much we allocate.
- Bump ptrs are nice efficient allocators, however there is a downside:
  -> 'You cannot free stuff'
  -> It only keeps track of where it is in memory that is currently free
  -> If you free stuff that is early on, it doesnt deal with holes at all
  ("THIS IS THE CURRENT ALLOCATOR INSIDE OS-161")
- In OS-161, if kfree() notices that a whole frame is free, it will give it back to the bump allocator
  that throws it away entirely.
  -> As long as there is something allocated, it stays working well. But as soon as you free a whole page,
  it gives it back to the lower level layer = the bump allocator.

^This is an example of being able to produce a magic number that can satisfy a request for a ptr to memory to store a type in.
- E.g. sizeof(int) 
	-> pass on to the bump allocator i.e. b_alloc(sizeof(int))
	-> bump allocator returns where it currently points to and increments bump ptr by size of the type you passed in.
	"THIS IS A SIMPLE MEMORY ALLOCATOR! W/ CONSTRAINT OF THROWING EVERYTHING AWAY"

=============================================
ASSIGNMENT 3
=============================================
Two parts:

'FRAME TABLE'
- Keeps track of 'what is free' and 'what is allocated'
- Try to test it as much as possible to ensure whatever you created works well, because
  if a lower level allocator (the thing youre building) that gives memory to a higher-level allocator
  which the entire OS-161 depends on is not working well, then OS-161 will fail miserably.
- An allocator would:
  -> NOT double/triple allocate the same memory unless it is FREEd.
  (IMPORTANT: Ensure no memory overlaps occur)
  -> NOT to have any memory leaks, where FREE() loses things.

'PAGE TABLE + REGION SUPPORT'
- Provides virtual memory for applications
- When the page table works, you will have a working system with shell and commands such as ls() cat() etc.
  (relatively simple UNIX clone that works)

R3000 (OS-161) 'Look at slide: Real R3000 Address Space Layout'
- Lives up in 0x80000000
- Applications live from 0x000000000 to 0x80000000
- This is the area that is translated by the TLB that your page table needs to store the mappings for.
- The OS-161 address space + kseg1 (address space 0x80000000 -> 0xA0000000) is directly translated to
  physical memory without involving the TLB. 
- Your page table has to provide data that the TLB / handler needs to populate the TLB to have the 'kuseg'
  area translated appropriately for the application that is running.

For each application that runs, it will have its own address space.
- Switching from one process to another involves:
  'changing address translations from one process address translations to another'
  -> You dont need to chany any of the context switching code to do this.
  -> There is a framework that you can use that the rest of OS-161 calls to achieve this.
- OS-161 code will call the appropriate underlying code in the right locaitons to achieve this switch from
  one view of virtual memory to another view of virtual memory when it switches one of the processes.

'kseg0' is where OS-161 lives:
- Hardware helps translates directly to physical memory (you can think of it as being hardcoded)
- 'OS-161' inside 'kseg0' starts from the base = '2GB' mark and consumes some of memory
- Above 'OS-161' is the rest of 'FREE RAM' up to however much RAM is configured for the machine.
  -> The max 'SYS-161' supports is 16mb.
- There are macros that allow you to convert 'to' and 'from' 'physical addresses' to 'kernel virtual addresses'
  in the area that allows you to access physical memory.
  -> Imagine you built an allocator that allocates physical memory and work in addresses from range of 0 to up to 16mb.
     To access that memory, you cant issue addresses between 0-16mb because there are virtual memory addreses down in the application.
  -> For the OS to manipulate RAM directly, you have to write to it through this 'FREE RAM' WINDOW.
     We can take a phsical address and convert it to the corresponding address in the window using this macro:
     
     #define PADDR_TO_KVADDR(paddr) ((paddr)+MIPS_KSEG0)
     '^adding or subtracting the appropriate offset of that address'

     This also works the other way around:
     If we have a kernel virtual address and we want the physical address associated with it, we can use the macro
     that goes in the other direction.

- top = ram_getsize()
  -> find out how much RAM is in the machine
  -> the mentioned configuration will return 16 mb.
  -> you need to size your data structure not assuming that RAM will always be 16 mb.
     It has to be based on how much RAM is configured to the machine.
     You can assume it can be '2' '4' '8' '16' (a power of 2 to keep things simple)
     ^we may test a combination of these to see if your code works.

- free_base = ram_getfirstfree()
  -> tells you how much memory OS-161 has used thus far in its bump allocator.
  -> return effectively where the bump allocator location is.
  -> Once you have this value, the idea is that you take over the allocation by having your own allocator.
  -> You need to know how much is used thus far, in order to initialise your allocator.

========================================================================

You will be implementing alloc_kpage() / free_kpage()
- These are the lower lvl functions that kmalloc() / kfree() use to allocate/free memory in its memory pool.
- alloc_kpage(): takes a number of pages, returns a ptr (pointing to a free range of pages)
- free_kpage(): takes that pointer and puts it back into the memory pool.
- 'For simplicity, we are assuming that we are only dealing with one page at a time'
- Will get a request for 1 page, and for all other requests (0 or >1) you can deny it by simply returning NULL
  -> NULL is the signal that you run out of memory
  -> For debugging, put a kassert() in there because if you get a request for >1 pages from the rest of OS-161
     some things will occur that shouldnt.
- Addresses that come out of the allocator need to be within 'kseg0 range'.
- If you are managing physical memory using addresses between 0 - 16 mb, just before you hand back the ptr,
  you should run the 'MACRO' that adds 2 GB to it to convert the 'physical address' of what your allocator is
  managing into a 'kernel virtual address' that then can be used by the rest of OS-161.

DUMBVMs simple bump allocator will return the 1st free address in physical memory.
- Corresponds to in the 'kernel virtual memory' whatever that is above the 2 GB mark.
- So any address accesses to 'kernel virtual memory' corresponds to access directly in 'physical memory',
  except with the 2 GB offset, so that 'applications can run in that address range'.
- Any allocation using the bump allocator that results in more allocations occurring, indirectly result in
  those allocations occurring in the corresponding location in the window in 'kseg0'.

To manage this, build a 'Frame Table' that has an entry in it for every Frame that is in physical memory
(1 to 1 relationship)
- Once you have a Frame Table, you need to initialise it with what is used / what is currently free.
- We can mark a few entries at the bottom that is already used by OS-161.
- The remainder can be initialised in a way that represents memory being free.
- Another trick for the Frame Table to have efficient allocaton is to form a 'linked-list' in the table itself
  using a linked-list of indexes of the free entries.
  -> i.e. chain the entries to refer to the next free entry
  -> i.e. to keep track of free memory, all you need to do is have an index that refers to the 1st slot that is empty.
  
  To allocate:
  1. take that index that is keeping track of the 1st free slot
  2. Then simply use that slot as the free frae that is just allocated
  3. Update the head of the index tracking node to point to the next free slot in the chian.
  
  To free (reverse):
  1. Take whatever slot the head is currently pointing to
  2. Take the empty slot and point it there
  3. Update the tracker to point to that location.

  'Now we have an O(1) allocate and free.'

How to place my Frame Table?

SOLUTION #1: At the 'top of RAM' (refer to slide)
- To do this, we need to know:
  1. how big the table is
  2. where the base address i
  so I can assign it to my pointer to my base table then I can index off it as though it is a normal array.
  '^i.e find the appropriate address to start using it like a normal array'

  struct frame_table_entry *frame_table;
  -> declare frame table pointer.

  location = top_of_ram - (top_of_ram / PAGE_SIZE * sizeof(page_table_entry));
  -> 1. take top of RAM, from fn that tells us how much RAM it is: 'top_of_ram'
  -> 2. figure out how many entries are in our frame table '(top_of_ram / PAGE_SIZE * sizeof(page_table_entry))'

  frame_table = (struct frame_table_entry *) location;
  -> assign to ptr the location that has been calculated
  -> now we can index off this location

  'NOTE: this is all in physical addresses, we must convert it up to kernel virtual addresses
   		 in order to access the Frame Table'

SOLUTION #2: At bottom
  1. Use existing bump allocator to allocate size of the Frame Table
  2. Get ram_getfirstfree() to figure out how much OS-161 has used incl. frame table.
  3. Initialize Frame Table then start allocator to use the Frame-Table based allocator
     and no longer the bump ptr.

  '2nd approach is the preferred approach, because you must allocate the HASH TALE
   for the hashed page table in memory, because there is only one of those.'
   It will be easier to just use OS-161 to allocate two of those with the bump ptr than to
   do this computation twice to figure out where things should live.

   Easiest thing is to use the Bump Allocator at the start, get everything allocated / initialise them.

How can my allocator work before and after it is initialised?
	
	struct frame_table_Entry *frame_table = 0; // start w/ frame_table ptr initialised to 0

	alloc_kpages() {
		if (frame_table == 0) {
			use ram_stealmem; // name of bump ptr allocator inside OS-161
		} else {
			use my own allocator as frame table is now initialised.
		}
	}

	Another way is to: very first time allocate page is called -> initialise the whole table itself
					   on the first time alloc_kpages() is called.
	'^approach not as good'

'KUseg layout'
- Typical layout of an ELF file.
- See load_elf()
- Stack at the top
- Code at the bottom
- Data section higher
- Other regions: whre they are located depends on location in the ELF file.
  'Compiler is choosing where things are'
- Use 'cs161-objdump -p testbin/huge' to print headers that describe the regions expect to exist inside
  the address space. The number of regions can vary, based off number of regions in the ELF file.
- Permissions are also specified in the ELF file.

'../bin/true'
- Another object dump tool, to tell us:
1. Which regions need to exist in memory to execute this particular file.
2. Which parts of the file need to be loaded into which locations in order to execute the file.
3. Need to 0-fill if the sizeof() the region that we are setting up, if the file part < region size, then
   0-filling is expected from the file-size up to the total size of the region.

- What we want to do inside OS-161 is keep track of these regions, i.e. valid ranges.
  ^Any references outside of these ranges are illegal memory references and can return a fault.

================

Process Layout and how it maps to Virtual Memory:
- Slide shows the layout of an individual program running on OS-161 at various memory locations.
- 'CODE' is a region that gets loaded
- 'DATA' lives a bit higher
- 'OTHER REGIONS' depending on what is in the ELF file
- What OS-161 does is load the ELF file, inspects the header then calls some functions that define
  the regions within this address space.
- Each Address Space function takes an 'address space struct ptr' as an argument, AS-defined region will
  get called by the loader component for each of the regions that will exist inside this program.
  -> You need to call that address space struct, do some bookkeeping that records the fact that these regions exist
  -> You dont have to load them, just to know that they exist so that if we get a VMFAULT at some point in
     the future, you can quickly determine whether this is:
     1. out of bounds of all the legal regions (VMFAULT return EFAULT)
     2. if it does exist then your VMFAULT will load the TLB (TLB MISS) from your Page Table
     3. or allocate 0-filled memory, put it in the Page Table and return
     4. or run out of memory, return NOMEM
  -> as_define_region(): gets called with bounds and permissions, can be implemented as a 'linked-list' hanging off the AS-structure
  -> as_define_stack(): returns the location of the stack ptr that your code shows. There is a constant defined somewhere that
  						sets the stack ptr to be at the top, inside region and the region size is reasonable to have something like 16 pages.

============================================================================

- ELF loader: defines the regions then loads the contents of parts of the regions that come from the file that we will start executing
  -> eventually, each region will be marked as read/write or readonly. 'CODE region is readonly'
  -> however, in order for loader to laod the code, it needes to be temporarily 'read/write'
  -> In the ELF loader, before it starts to load the code into that location, it calls:
     as_prepare_load(): Your vm subsystem wont honour the permissions of readonly of the CODE, just for the duration until
     as_complete_load(): is called which will go back to enforcing the readonly permissions on the segments that were readonly.
     It allows the ELF loader to load those parts of the file.

     For debugging purposes, make as_prepare_load() and as_complete_load() nothing as simply load everything read/write all the time.
     That way, your programs iwll be up and running initially so the page table / vm subsystem is debugged then for the last part, go back
     and do the permission enforcement properly.

- Need to keep a Page Table translation table for translating faults from one region to memory.
  -> Idea is that on VMFAULT, you will consult the page table to figure out / obtain the appropriate TLB entry to load into the TLB to allow the mapping
     to be established for these particular regions.

================================================================================

- as_create() and as_destroy() are two pieces of code that you provide that gets called by the OS everytiem it wants a new address space.
  -> e.g. everytime the loader wants to start a new process or fork() wants to fork a child, it will call as_create() at the appropriate point
    and you need to create the bookkeeping that keeps track of your address spaces i.e. the application address spaces.
    ^Keeping track of the regions associated with that address space.
    ^A struct address space that you can augment to contain any bookkeeping that you think you need.

    What can you use as a Process ID (PID) in the Hash Page Table?
    struct addrspace *as;	// use this ptr for matching your Hash Page Table which entries are associated with the current address space.
    pid = (uint32_t) as;    // cast the addrspace ptr into a 32bit value and use that as the PID for matching in your Hash Page Table.
    					    // gives you an ID that you can distinguish between which entries in your Hash Page TAble are related to
    							// the current address space or another address space.
    							// You can always get the current addr space easily in your code by calling proc current addr space for matching various points in your code

   -> as_destroy() will walkthrough the bookkeeping that youre creating and free up any entries that have been allocated for this particular addr space
      also de-allocates Frames that is allocated to the address space.

      This gets called during exit() or kill() current process by OS-161. It destroys the address space.

================================================

as_copy()
- Gets called by fork() to copy an address space.
- Creates a new copy of the addr space that has all the same regions as previously.
- However, each frame that is mapped needs to be copied such that the new child while sees pages that have the same content of the parent,
  they are independent copies so that the child modifies things independently and the parent modifies things independently.
- I.e. '1. Walking through your Page Table
        2. Identifying entries that are related to the current addr space
        3. Inserting an entry for the new addr space pointing to a newly allocated Frame with a copy of the old content'
        A BIG LOOP

as_activate() / as_deactive()
- TLB flush routine.
- In the OS, if we update the page tables, there is potential for stale entries to still be in the TLB.
- Everytime we 'downgrade access rights' OR 'switch from one address space to another', we need to ensure that we dont have stale entries in the TLB
  that we have used.
- In OS-161 nearly all of the appropriate places, calls as_active() whenever it thinks the TLB should be flushed.
- 'You can rip off this code from DUMBVM'
- You can just make as_deactive() call as_active()

================================================

VM Fault Approximate Flow Chart
- AS part of assignment is just getting data structures in place so that VM FAULT can do its business.
- VM FAULT gets called everytime TLB misses.

For the base assignment, there are some different fualt types to deal with.

- VM FAULT readonly: attempt to write to a readonly page => for base assignment return EFAULT
- if !VMFAULT readonly: look up the Page table to see if there is a valid translation for this fault.
  -> 'look at DUMBVM, how the bitmasking works / general framework for TLB refill handler: you can cut/paste code and gut'
  -> if valid translation, load TLB
  		-> for TLB loading, use 'write random'.
  -> if no valid entry in page table, look up region list and cant find a range in our region list that is legal for this fault, return EFAULT
     else we must have a VMFAULT on a logically existing page that we havent put in the page table yet. Only occurs in base assignment when we havent allocated a frame in the page yet.
     	we can allocate a frame: 0-fill it and insert a Page Table entry in our Page Table for that newly established page->frame mapping and load the TLB.

     In Load ELF: it will use virtual addresses within the user-space, so when it tries to write part of the ELF file into memory, it will cause these VMFAULTS.
     -> Load ELF will only break out of memory if you return EFAULT or ENOMEM from VMFAULT otherwise it will transparently continue
        because you are just doing a nested exception here in the TLB fault handler and it will replace the entry that has caused the fault
        and return back into the ELF loader to where it was copying before.
     -> We can call kmalloc()

================================================
VMFAULT DEBUGGING

kprintf()
- Do not use kprintf() in vm_fault()
- It blocks current processes while printing. You will be stuck in an endless loop.
  -> kprintf() switches to another process -> context switch flushes TLB / flushes what you just inserted

USE TRACE-161
- A verson of sys161 that prints out events that happen at the hardware level.
- A lot of noise comes out, so you need to filter for what you want to find.

================================================
HASH PAGE TABLE (move to proper notes)
================================================












