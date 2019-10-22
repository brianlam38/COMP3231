========================================================================
VIRTUAL MEMORY
========================================================================

Memory Management Unit / Translation Lookaside Buffer 'TLB':
1. The CPU sends Virtual Addresses to the MMU
2. MMU sends physical addresses to the memory.

Virtual Memory: Divided into equal-sized 'Pages' vs. Physical Memory: Divided into equal-sized 'Frames'
- 'Mapping' is a translation between a page<->frame / page<->NULL
- Mappings are defined at runtime

Typical Address Space Layout:

	['K']['T']['S'][ ][ ]['L'][ ][ ][ ]['F']['E']['D']['C']['B']['A'][ ]

	'K': Kernel		'L':Shared Libraries   	 'F,E': BSS (HEAP)
	'T,S': Stack							 'D,C': Data
											 'B,A': Text (CODE)

A process may only be partially resident in memory:
- Allows OS to store individual pages on disk.
- Saves memory for infrequently used data & code.

'#### PAGE FAULTS ####': referencing an invalid page triggers a page fault
- It is an exception handled by the OS
- Two types of page faults:
  1. 'Illegal Address': Signal or kill the process
  2. 'Page not Resident':
     -> Get an empty Frame -> Load page from disk
     -> Update page translation table (enter frame #, set valid bit etc.)
     -> Restart the faulting instruction

'#### SHARED PAGES ####'
- Private Code and Data: each process has its own copy of code and data
  -> Can appear anywhere in the addr space.
VS.
- Shared Code: Single copy of code shared between all processese executing it
  -> Code must not be self modifying
  -> Code must appear at the same addr space in all processes

'#### PAGE-TABLE STRUCTURE ####'
- A Page-Table is an array of Frame Numbers, indexed by Page Number.
- Each entry has other bits.

	[ ][C][R][M][P][V]['PAGE FRAME NUMBER']

	'C': Caching Bit - indicates if processor should bypass cache when accessing memory
	'R': Reference Bit - indicates if page has been accessed
	'M': Modified Bit - indicates if page has been modified in memory
	'P': Protection Bit - RWX permissions
	'V': Valid Bit - indicates a valid mapping for the page

'#### Address Translation ####'
- Every memory address issued by CPU must be translated to physical memory. (needs Translation Hardware)
- In a paging system, translation involves replacing the Page Number with Frame Number
- Scenario:

	1. Program uses virtual address

		[ 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 1 ][ 0 0 0 0 0 0 0 1 0 1 1 0]
		<-------------20BIT VIRTUAL PAGE------------><-----12BIT OFFSET------->
		<---------------------------32BIT VIRTUAL ADDRESS--------------------->

	2. Virtual address is mapped to physical by MMU/TLB
	   
		   Go to Page-Table:
		   4[ ][   ]
		   3[1][110] -> Check if valid bit is set / if page exists -> (YES) address 110 in page table form the MSB in physical address
		   2[ ][   ]

		   Physical Address:
		   ['1 1 0' 0 0 0 0 0 0 0 0 0 0 0 0 0 '1 0 1 1 0' ]
		  <--------> 						 <------------>
	From Page Frame Number  				From 12bit offset

'#### Calculating Page Table Size ####'

Assume a 32bit virtual address [ address space has 2^32 combination of values ], with 4kB page size.

	'-- Convert Page size to bits --' 				 4kB = 4,000 bytes = ~2^12 bits/page

	'-- Divide total addr space by Page size -- ' 	 2^23 / 2^12 = 2^20 # of Pages/Frames possible [ page size = frame size ]

	'-- Calculate size of each Page Table Entry --'  20 bits required for Frame # (as 2^20 # of Pages/Frames possible)
												    + 1 valid bit
												    + 1 modified bit
												    + 1 reference bit
												    + 1 protection bit
												    + 1 caching bit
												    = 25 bits = 4 bytes

	'-- Calculate Page-Table size --'				4 bytes * 2^20 pages
													= 2^2 * 2^20
													= 2^22 bytes (4,194,304 bytes)
													= '~4MB PAGE TABLE'

Page-Table is very large, access has to fast.
- Lookup occurs for every memory reference.
- Most processes do not use the full 4GB address space, so we need a compact representation that does not waste space
  but is still fast to search.

'#### TWO-LEVEL PAGE TABLE ####'
Virtual addresses have 3 parts: (1) MASTER PAGE # | (2) SECONDARY PAGE # | (3) OFFSET

	Assuming a 32-bit virtual address space:
	'Master Page-Table' 	(10 bits) => maps Master Page # to Secondary Page-Table | 2nd PT start addr = Master Page # + Master PT Pointer
	'Secondary Page-Table'	(10 bits) => maps Secondary Page # to Frame #           | Frame # = 2nd PT start addr + 2nd Page #
	'Frame No. + Offset'   	(12 bits) =  Physical Address 							| Physical Addr = Frame # + Offset

	VIRTUAL ADDRESS:
	 _________________________________________________
	[ Master Page No. ][ Secondary Page No. ][ Offset ] -----------------------------|
			|				|													   |
			v 				|													   |
	 'MASTER PAGE TABLE'	|	'SECONDARY PAGE TABLE'							   |
     [ - - - - - - - -]		|	__________________								   |
     [ 2nd Page Table ] ------->[ - - - - - - - -]    			'PHYSICAL ADDRESS' |
     [ - - - - - - - -]		|	[ - - - - - - - -]	 							   v
     [ - - - - - - - -]		|-->[ Page Frame No. ]--------->[ Page Frame No. ][ Offset ]
     							[ - - - - - - - -]

'#### INVERTED PAGE TABLE ####'
A global page-table maintained by the OS for all the processes. (1 page table in the entire system)

	Algorithm:
	1. Compute Hash(Virtual Page Number)
	2. Extract IPT Index from Hash Table
	3. Match PID + Page Number in IPT entry
	4. if YES_MATCH: Use 'index value' has Frame # for translation
	   if NO_MATCH: Goto 'next' candidate IPT entry from chain field
	   if NULL chain entry: 'page fault'


	'-- VIRTUAL ADDRESS --':
	[ PID ][ Virtual Page # ][ Offset ]
	[  0  ][   0x5    		][  0x123 ]
				|	
				|	 	'Hash Table'			 'Inverted Page Table'	
				v  		 [ - - - - - ]		    PID   VPN    ctrl  next
		   'Hash(VPN)'-->[IPT_INDEX=2]------> 2[ 1 ][ 0x1A ][    ][ 0x40C ]		 '-- PHYSICAL ADDRESS --':
		   				 [ - - - - - ]        .[   ][      ][    ][       ]		 [ Physical Page # ][ Offset ]
  									  	  0x40C[ 0 ][ 0x5  ][    ][  0x0  ]----->[ 		0x40C      ][  0x123 ]
  									      0x40D[   ][      ][    ][       ]

Properties of Inverted Page-Tables:
- Decreases memory required to store each page-table but:
- Lookup is significantly higher O(n) as entire table must be searched to finding matching PID|VPN in the IPT.

'#### HASHED PAGE TABLE ####'

	Algorithm:
	1. OS grabs VPN 'p' from CPU and performs hash(p) => 'key'
	2. OS looksup first entry in Hashed-Table with 'key = same_key' and checks if 'p=q'. NO MATCH.
	3. OS uses the 'next' ptr in first entry to find second entry. OS checks if 'p=p'. MATCH.
	4. It grabs the Page Frame Number 'r' from the second entry.
	   -> 'r' is the physical frame that corresponds with the virtual page number 'p'
	5. OS uses 'r' to find physical frame in memory + adds offset 'o' to find the exact location of data requested by process.

	'VIRTUAL ADDRESS':
	[ PID ][ Virtual Page # ][ Offset ]		  PID   VPN	   PFN    ctrl  next 		  'PHYSICAL ADDRESS'
	[  0  ][      0x5    	][  0x123 ]		1[	 ][     ][      ][    ][     ]			PPN	   OFFSET
				   |						2[	 ][     ][      ][    ][     ]		  [ 0x42 ][ 0x123 ]
				   | 						3[ 0 ][ 0x5 ][ 0x42 ][    ][ 0x0 ]\
			       |						4[ 	 ][     ][      ][    ][     ] \ 'chained to second entry'
			       v						5[	 ][     ][      ][    ][     ] /
			 	Hash(p)------------------>  6[ 1 ][ 0x1A][ 0x13 ][    ][ 0x3 ]/

Hashed Page Table Properties:
- Size is based on physical memory size
- Enables 'Sharing' as each Frame can have more than one Page Table Entry.
- More Sharing = increases number of slots used, but also increases 'chance of hash collisions'
- We can tune the Hashed Page-Table size based on:
  -> Physical memory size
  -> Expected sharing
  -> Hash collision avoidance
- 'Hash Page-Table size = x2 of the no. of physical memory frames'

'#### TRANSLATION LOOKASIDE BUFFER (TLB) ####'
TLB is a high-speed cache for Page-Table Entries, containing recently used page table Entries
- Solves issue where each Virtual Memory reference can cause two physical memory accesses:
  1. One to fetch the Page Table Entry
  2. One to fetch/store the data

  Algorithm
  #1 Given a virtual address, processor examines the TLB
  #2 'TLB HIT' if matching PTE:
  			address is translated to physical address
     'TLB MISS' if !matching PTE:
     		page number is used to index the process'' Page-Table.
  #3 if Page-Table contains valid entry:
  			reload TLB and restart.
  	 else 'PAGE FAULT':
  	 		check if page is on disk.
  	 		if on disk: swap it in.
  	 		else: allocate new page or raise an exception.

TLB Properties:
- Page-Table is an array of Frame Numbers
- TLB holds a recently used subset of Page-Table entries.
  -> Each TLB entry must be identified with the page # it translates.
- Access is by lookup: All TLB entry`s tags are concurrently compared to the page #
- TLB Size = Typically 64-128 entries.
- Can have separate TLBs for instruction fetch and data access

'Hardware Loaded TLB'
- On miss, hardware performs PT lookup + reloads TLB
'Software Loaded TLB'
- On miss, hardware generates a TLB miss exception and exception handler reloads TLB

TLB and Context Switching:
- TLB is a shared piece of hardware.
- TLB entries are process-specific:
  -> On context switch need to flush the TLB (invalidate all entries) = high context-switching overhead
  -> Or tag entries with 'Address-Space ID'
  'TLB ENTRY: [ ASID | PAGE # | FRAME # | VALID BIT | WRITE PROTECT BITS ]'

TLB effects
- Without TLB: Average no. of physical memory references per virtual reference = 2
- With TLB: (Assuming 99% hit ratio) = 0.99 * 1 + 0.01 * 2 = '1.01'

For MIPS R3000 (software-loaded TLB):
- On miss, hardware generates a TLB miss exception -> exception handler reloads TLB.

MIPS R3000 Address Space.

'ASSUMING 32-BIT ADDRESS SPACE'
================= 0x'F'FFFFFFF = 4,294,967,295 uint32_t max-size
				|				
				|				
	Kseg2		|				
				|				
				|				
================= 0x'C'000000 
				|	: SIZE: 512mb
	Kseg1		| 	: Where devices are accessed. Accessible in Kernel-mode only.		
				|	: Virtual -> Physical has a fixed translation.
				|	: Cache-able
================= 0x'A'000000
				|	: SIZE: 512mb
    Kseg0		|	: Usually where Kernel code is placed. Accessible in Kernel-mode only.				
				|	: Virtual -> Physical has a fixed translation.
				|   : NOT Cache-able
================= 0x'8'0000000 = 2,147,483,648 uint32_t half-way
				|	: SIZE: 2gb
				|	: Where the Process Address Space is.
				|	: TLB translated/mapped.
	KUSEG		|	: Accessible in User and Kernel mode.
				|	: Page size = 4K	
				|				
				|				
=============== 0x'0'0000000 = 0



















