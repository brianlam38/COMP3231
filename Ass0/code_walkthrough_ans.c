'Question 1: What is the vm system called that is configured for assignment 0?'

	The VM-system is called Dumb VM.

'Question 2. Which register number is used for the stack pointer (sp) in OS/161?'

	Register $29 is used for the Stack Pointer in OS/161.
	Refer to the regdefs.h macros.

'Question 3. What bus/busses does OS/161 support?'

	LAMEbus is the only bus on sys161.
	Refer to kern/arch/sys161/include/bus.h

'Question 4. Why do we use typedefs like uint32_t instead of simply saying "int"?'

	- We use uint32_t to specify that a value is an unsigned 32bit int.
	- Different architectures handle int as different sizes.
	- Specifying uint32_t allows us to make sure that it will be a consistent number of registers/memory
	each time - i.e. always 32bit, unsigned - thus allows for simplifying compatability across platforms.
	- It also allows us to get more mileage out of a variable that we know will never go negative.

'Question 5. What function is called when user-level code generates a fatal fault?'

	kill_curthread(vaddr_t epc, unsigned code, vaddr_t vaddr)
	- I am assuming that depending on the code value of the fatal fault, it will invoke
	  a respective signal i.e. SIGABRT / SIGSEV / SIGBUS / SIGTRAP / SIGILL / SIGFPE
    - Refer to arch/mips/locore/trap.c

	NEED MORE STUDY:
	- What is a Trap code? Or what are Traps in general in Operating Systems?

'Question 6. How frequently are hardclock interrupts generated?'

	- hardclock() is called on every CPU HZ times a second, possibly only when the CPU
	is not idle, for scheduling.
	- HZ is #define as 100
	- Refer to kern/include/kern/clock.h

'Question 7. How many characters are allowed in an SFS volume name?'

	- SFS is the Simple File System
	- The maximum is 32 chars including the \0 character, so only 31 chars are allowed.
	- Refer to kern/include/kern/sfs.h

'Question 8. How many direct blocks does an SFS file have?'

	- SFS file has 15 direct blocks
	- Specified by #define SFS_NDIRECT 15
	- Refer to kern/include/kern/sfs.h

	- inode: is a data structure in a UNIX environment that describes a filesystem object
	         such as a file or a directory. Each inode stores attributes and disk block
	         locations of the objects data.

'Question 9. What is the standard interface to a file system (i.e., what functions must you use to implement a new file system)?'

	- fsop_sync(struct fs *);
	- fsop_getvolname(struct fs *);
	- fsop_getroot()

Question 10. What function puts a thread to sleep? 
Question 11. How large are OS/161 pids? 
Question 12. What operations can you do on a vnode? 
Question 13. What is the maximum path length in OS/161? 
Question 14. What is the system call number for a reboot? 
Question 15. Where is STDIN_FILENO defined?






