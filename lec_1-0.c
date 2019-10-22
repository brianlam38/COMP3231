Make sure to do assert() based programming.
- NULL checks
- Checks in the code for returns code that might fail.
- E.g. did kernel allocator actually return memory? OR are you assuming this?

Need to be more systematic in programming.

'ROLES OF AN OPERATING SYSTEM'
Role 1: The OS is an abstract machine
Role 2: The OS is a resource manager
- Ensure no starvation
- Ensure all programs make process
- Resource allocation is according to a desired policy i.e. FIFO, Fair Share, Weighted Fair Share, Limits (quotas) etc.
- Overall ensuring that the system is efficiently used

OS and Security
- The OS should be responsible for security
- Fundamental security checks against applications should be inside the OS
- 'Privilege Escalation': bypassing OS by manipulating software that has privileged access

Operating System Kernel (e.g. Linux Kernel) Can be called 'Nucleus / Supervisor / Kernel'
- The portion of the OS that is running in privileged mode.
- Usually stays in main memory.
- Contains fundamental functionality:
  -> Whatever that is required to implement other services
  -> Whatever is required to provide security
- Contains most-frequently used functions

The Operating System runs as the 'privileged' part of the machine.
- It runs as privileged because it should be able to use hardware to restrict what apps do.
- Not only 'implements' high level abstraction for apps but 'enforces' apps.
- Applications should NOT be able to interfere or bypass the OS.
- OS can enforce resource allocation policies.
- OS can prevent apps from interfering with each other.

The structure of a computer system:
- 'USER MODE': Applications, System Libraries w/ tools such as strcmp() etc.
  -> Apps interact with themselves via. function calls to library procedures.
  -> Apps interact via. 'SYSTEM CALLS' ('transition me into privileged mode and give OS control')
     OS gets control of app to see what app has requested so then it can perform the operation.
  -> EXAMPLE: Read from network socket operation from an App
     1. App will call the read() function in the Systems library - part of C library
     2. Read would set up arguments, trap into the OS
     3. OS gets control of app + understands the App requested operation
     4. OS gets required information, puts it inside the application
     5. Return back into USER MODE
     6. Application continues with the information that it needed from the system call
  -> EXAMPLE: printf() non-system call example.
     1. Takes specification + args that App user crafted.
     2. OS builds buffer containing what you want to print inside the App.
     3. OS calls the system call pointing to that buffer + send output to wherever.
     printf() is basically a wrapper around the write() system call.
- 'KERNEL MODE': Hardware devices, Operating System, Memory etc.
  -> OS would manage hardware devices on behalf of apps to provide abstraction.
  -> Not safe to allow apps to interact with devices directly, we want OS to sanity check what apps want to do.
  -> OS interacts via. load and store instructions to all Memory, CPU and Device Registers and Interrupts.

'PRIVILEGE-LESS OPERATING SYSTEM'
- Some embedded OSs have no privileged component
- Can implement OS functionality but cannot enforce it
  -> All software runs together
  -> No isolation
  -> One fault potentially brings down entire system
- OS cant protect itself from malicious software, can be a fragile environment
- OS essentially trusts every app that it runs
- Examples: e.g. car OS

'SYSTEMS LIBRARIES'
- Are just libraries of support functions that make programming your apps easier.
- Only a subset of library functions are actually system calls.
  -> strcmp() memcpy() are pure library functions. They are simply 'Convenience Functions'.
     Manipulates memory within the app or perform computation.
  -> open() close() read() write() are system calls.
     They cross the user-kernel boundary e.g. read from disk device.
     Implementation mainly focused on passing request to OS and returning result to app.
- System call functions are in the library for convenience.

Operating System Software.
- OS functions the same way as normal software
- It is a program that is executed + has more privileges
- OS relinquishes control of the processor to execute other programs
- OS re-establishes control after System Calls or Interrupts (e.g. 'timer interrupts')
  -> OS can get control through timers

'HIGH-LEVEL OS CONCEPTS'
Processes:
- A program in execution, the entity that can be assigned to and executed on a processor, a unit of resource ownership.
- Consists of three segments:
  1. 'Text' contains the code (instructions)
  2. 'Data' contains global variables
  3. 'Stack' contains local variables + activation records of procedure/function/methods
- Note: Data can dynamically grow up e.g. malloc()
        Stack can dynamically grow down e.g. increasing function call depth or recursion.
- A process state consists of three segments:
  1. 'Text' executable program code
  2. 'Data and Stack' associated data needed by the program
  3. 'Execution context of the program' Registers, program ctr, stack ptr,
      info that the OS needs to manage the process i.e. OS-internal bookkeeping, files open etc.
Concurrency issues with multiple processes:
- 'Deadlocks'
Memory Management:
- 'Process Isolation' prevent processes from accessing each others data
- 'Automatic allocation and management'
   -> users want to deal with data structures
   -> users dont want to deal with physical memory directly
- 'Protection and access control'
   -> Still want controlled sharing
OS Services:
- 'Virtual memory'
- 'File system'
     
Operating Systems dont have the privilege of writing bad code unlike devs. They need to
keep track of all the memory that has been given to apps in order to ensure that there
are no memory leaks at the Operating System level, forcing a user to reboot.










