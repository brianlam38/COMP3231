Memory Management is managing memory in the machine to achieve:
1. 'Process Isolation' so that the machine can be shared between diff processes.
2. 'Allocation and De-allocation of memory for data structures'
   - OS must deal with the fact that not all apps are programmed well and book-keeping is required
     in order to ensure that things are cleaned up when programs execute.
   - Allow processes to share memory and collaborate in order to perform some kind of computation
     e.g. Web Servers may create a whole new process to service that request. Advantage is that the servicing
          of the request separately, if it leads to a malfunction, then the web server servicing that specific
          requests dies but the main one doesnt.
   - 'File Systems' are the main repo of data beyond the existence of a process or the particular start of a machine.

'Virtual Memory'
- Provides programmers a logical view of what is happening with the physical allocation of memory
  in the machine at the time.

'Virtual Memory Addressing Process'
1. CPU issues virtual addresses that your apps are using. For example virtual addresses such as:
  -> virtual addresses stored in pointers, data structures etc.
  -> return addresses stored on the stack etc.
2. Memory Management (exists in the processer)
  -> Translates every virtual address into the corresponding location in physical memory
  -> Transalation is under the control of the Operating System

'File Systems'
- It is a name repository for data.
- Example:
  -> Root -> Students -> Student Names -> eventually student files / data.
- Each OS has their own File System convention i.e. Windows has Pictures, Docs etc.

'Scheduling and Resource Management'
- Scheduling in regards to choosing which service we want to service next.
- Scheduling algorithms will be a recurring theme.
- Fairness e.g. fairness algorithms
- Differential Responsiveness e.g. Games vs. Background processes
- Efficiency e.g. throughput for weather simulations, making sure everything is fully maximised

'Operating System Structure'
- LAYERED APPROACH, where each layer depends on the inner layers.
  ^This is unrealistic because all these concepts end up becoming interdependent on each other.
- SPAGHETTI APPROACH, where everything is tangled
  ^Also unrealistic, but closer than the layered approach.
- Monolithic Operating System Structure
  -> 'Interprocess communication': mechanisms that allow processes to talk to each other and cooperate to achieve a goal
     e.g. sending signals or messages between each other
  -> There is a lot of interdependency on the different subsystems.

'Windows equivalent subsystem'
- 'User Applications' -> Win32 API (sys functions) -> NTDLL -> interfaces to system Kernel
- 'Inside Kernel' -> various runtime library functions + processes + threads + object manager (way of arranging things within the kernel)
  + virtual memory management + graphics + device drivers / IO managers -> interaction with hardware.
- 'Hardware' then performs operations

============================================================================================================
SYSTEM CALLS - INTERFACE
============================================================================================================

'System Calls - Interface and Implementation'
- A mechanism to cross between the USER LEVEL and OPERATING SYSTEM i.e. Kernel level

Logical Workflow:
1. App calls into system library, invoking a system function
2. System function executes a special hardware instruction
3. Special hardware instruction transitions system from running in USER mode to KERNEL mode and
   giving the OS control + enough info to understand what happened.

'System Calls'
- Are special function calls, called from your programming language that provide a controlled entry into the OS.
- Entry point needs to be controlled, because you cant trust Applications to do the right thing.
  There needs to be a carefully controlled entrypoint provided by the hardware that allows the OS to get control in
  a secure and safe way, to manage the transition between USER <-> KERNEL modes.
  THE APP SHOULDNT BYPASS THE BOUNDARY EVER.

- Inside Kernel mode, perform the operation + run the implementation of the abstraction. Manage the hardware to
  provde the result that were looking for. Then turn back to the caller in USER mode with the result.
- System Call is the only abstraction that the OS provides to Applications.
  ^'SYSTEM CALLS ARE THE ABSTRACTION'

Example: 'System Calls for Process Management'
- pid = fork()   | Create a child process identical to the parent
- pid = waitpid(pid, &statloc, options)  | Wait for child to terminate
- s = execve(name, argv, environp)  | Replace a processes core image
- exit(status)  | Terminate process execution and return status

Example: 'System Calls for File System Management'
- fd = open(file, read/write, ...) | Open a file for reading, writing or both
- s = close(fd)  | Close an open file
- n = read(fd, buffer, nbytes)  | Read data from a file into a buffer
- n = write(fd, buffer, nbytes) | Write data from a buffer into a file
- position = lseek(fd, offset, whence)  | Move the file pointer
  -> Choose where in the file you would like to write to.
  -> Seek the file pointer around the file.
- s = stat(name, &buf)  | Get a files status information
  -> Once file is written, you can query information about the file

Example: 'System Calls for Directory Management'
- s = mkdir(name, mode)  | Create a new dir
- s = rmdir(name)  | Remove an empty dir
- s = link(name1, name2)  | Create a new entry, name2, pointing to name1
- s = unlink(name)  | Similar to rm. If it is the last link, OS will garbage collect and clean up content of file.
- s = mount(special, name, flag)  | Mount a file system
- s = unmount(special)  | Unmount a file system

Example: 'System Calls for Miscellaneous Tasks'
- s = chdir(dirname)  | Change the working dir
  -> Keeping track of the current working directory
  -> e.g. using 'cd' which uses this syscall
- s = chmod(name, mode)  | Change a files protection bits
- s = kill(pid, signal)  | Send a signal to a process
- seconds = time(%seconds)  | Get the elapsed time since Jan. 1 1970

A stripped down Shell implementation:
while (TRUE) {
	type_prompt();         			// wait until they type
	read_command(command, params);  // take input and form command

	if (fork() != 0) {				// fork off child process
		/* Parent code */
		waitpid(-1, &Status, 0);    // wait for child to exit
	} else {
		/* Child code */
		execve(command, params, 0); // execute command
	}
}

The shell is nothing special. It just happens to be the initialisation program that the subsystem gives you when
you log in. There is no magic that you cant do. You can replace it, rewrite it or run something else and never
use it again. It is just a program that uses standard abstraction, syscall APIs to provide the functionality
that you are used to for navigation around a file system in a command line.

Same as Windows.

Windows Sys32 API calls: Look at slide table to see corresponding syscall functions to UNIX functions.

============================================================================================================
SYSTEM CALL IMPLEMENTATION - CROSSING USER/KERNEL BOUNDARY
============================================================================================================

A simple model of CPU computation:
- A program counter, containing the address of the next instruction that the machine will execute
- How the machine works:
  1. Fetch instruction at the address
  2. Executes that instruction
  3. Increments program ctr to point to the next instruction
  4. Goes back into the loop again

More descriptive model of CPU computation: 'REFER TO LEC SLIDES FOR DIAGRAM ON THE MODEL'
- Add some registers: 'General Purpose Registers 'used for computation of the application
  -> local vars on the CPU that fast computations are performed on
- A 'Stack Pointer' also exists, keeping track of where the stack has grown to for this sequence of execution.
  -> I.e. local vars in a programming language such as C being allocated on the Stack.
  -> Adjusting the Stack Pointer, increasing/decreasing based on how much memory storage is required for
 	 local vars for the programming environment.
- 'Status Registers' that keep track of the state of execution.
  -> Might contain info such as the `the last operation resulted in an error`
  -> Or system-level status registers e.g. `interrupts are turned off` or `mode the chip is running in`












