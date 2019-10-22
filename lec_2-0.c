"""
LECTURE 2: System Calls and MIPS R3000 Overview
"""

============================================================================================================
SYSTEM CALL IMPLEMENTATION - CROSSING USER/KERNEL BOUNDARY (Continued)
============================================================================================================

Protect Operating System execution, two or more CPU modes of operation exist:
- 'Privileged Mode' (system, kernel mode): All instructions and registers are available
  -> MMU registers: manipulate memory layout
  -> Exception Type: tells OS what went wrong / why it got back in control again
  -> Interrupt Mask: what interrupts are on / off
  ^ these are all only accessible to the OS / in privileged mode.
- 'User Mode': Uses safe subset of the instruction set.
  -> It only affects the state of the application itself
  -> They cannot be used to uncontrollably interfere with the OS.

Examples of unsafe instructions:
- 'CLI' instruction on x86 architecture 
  -> Instruction disables interrupts
  -> Example exploit (Denial of Service) to disable interrupt and make app go into an endless loop:
     cli /* disable interrupts */
     while (true)
     	/* loop forever */
  -> If this instruction is run in 'user-mode', an exception will be triggered.
     Your program will be killed by the OS once it gets that exception.

Transition from User -> Kernel mode:
- Accessibility of memory changes
  -> Kernel Mode: Access everything
  -> User Mode: Access only a subset
  -> Exact memory ranges are configurable and vary between CPU architectures / Operating Systems

System Call - Timeline of Memory Usage:
- The System Call mechanism securely transfers from user execution to kernel execution and back.

'USER MODE'						              > 'Application'
------------------------------------------------------------------------------------------->
|||||||||||||||||||||||||||||||||||||||||||||| (1) Syscall Occurs - transition from app to OS
------------------------------------------------------------------------------------------->
'KERNEL MODE'                         ----------------------
                                      ||||||||||||||||||||||(2) Return control
                                      ----------------------
                                      'System Call Handler'

(1) System Call occurs
    -> The application passes control to the OS
    -> The OS needs to figure out why it has control again + what the app is asking me to do.
(2) Return control back to user mode.
    -> Continue where the Application left off.

To switch betwen these modes, some questions must be asked:
- There is only one register set. How is register use managed?
  -> 1. How to manage the register state that the app was using
  -> 2. How to set up OS register state
  -> 3. How to restore the app register state back to what it was
- 'Kernel Mode Trigger?': How is the transition to Kernel Mode triggered?
- 'OS Entry Point?': Where is the OS entry point? (system call handler)
- 'What to do?': How does the OS know what to do?

There is an explicit instruction to transfer from User -> Kernel Mode.
- 'User to Kernel': system call instruction
- 'Kernel to User': return from privileged mode instruction
- 'TRAP' instruction is an alternative term for syscall
  -> Effectively trap the exception into a well-known location and get control.

System Call Mechanisms - The things that need to happen when a syscall occurs:
- 'Processor Mode Needs to Switch': Switch from User -> Kernel | switched back when returning to User Mode
- 'Stack Pointers - Managing the User Register set':
  -> 1. Save User-level SP
  -> 2. Initialise Kernel-level SP
  -> 3. Restore User-level SP when returning to User-mode.
- 'Program Counter': Have to remember where the application called us from.
  -> Potential calls to the OS scattered through hundreds of positions in the code.
  -> We need to remember which one of those calls were made in order to return back to it.
  -> Therefore, we need to remember the program counter + set the counter to the entry point to the OS.
  -> Kernel Entry Point must be strictly enforced, otherwise bypass checks and execute unauthorised actions. 

System Call Mechanisms - How registers work
- Registers are set at user-level to indiciate syscall type and arguments.
- Some registers are preserved at user/kernel level in order to restart user-level execution.
- Result of syscall placed in registers when returning to user-level.

Why do we need System Calls?
- We need a controlled entry point that allows an OS to check whether the app is allowed to do what
  it is asking or not.
- Common Feature of all OS: controlled entry point at the hardware level that allows the OS to take control and
      						determine if instruction is allowed and perform checks.

'Example: 11 steps in making a system call read(fd, buffer, nbytes)':
-- USER PROGRAM CALLING READ --
Essentially pushing arguments of the syscall onto the stack of the application.
(1) Push 'nbytes'
(2) Push address of 'buffer'
(3) Push 'file descriptor (fd)'
(4) Call 'read()' function into the system library
-- LIBRARY PROCEDURE READ --
(5) Put code for read in a 'register' so that OS knows what to look at
(6) 'Trap' to the Kernel. (Perform the 'syscall' instruction: transfer User -> Kernel)
-- KERNEL SPACE ACTIONS --
(7) Dispatcher: the component that dispatches to all the handlers for all the different system calls.
    - Determine that is a read() system call + dispatches appropriate code for read().
    - Call the syscall handler
(8) System Call handler:
    - Syscall handler returns to User-mode to the point AFTER the Trap instruction.
(9) Return control to User-Mode
    - Increment Stack Pointer to undo the pushes for pushing args on the stack.


============================================================================================================
MIPS R3000 INTRODUCTION
============================================================================================================

Load/Store Architecture
- 'Store Word': sw   r4, (r5)
- 'Load Word' : lw   r3, (r7)
- 'Addition'  : add  r3, r2, r1 -> r3 = r2 + r1
- 'Move'	  : move r2, r1     -> r2 = r1

All instructions are 32-bit.
Immediate values are constants, encoded in the instruction itself and are only 16-bit.
- 'Add Immediate' : addi r2, r1, 2048 -> r2 = r1 + 2048
- 'Load Immediate': li   r2, 1234     -> r2 = 1234

Offsets
- lw  r4, 32(r29)  -> r4 = address of r29 + 32 bytes
- sw  r4, 32(r29)  -> address of r29 + 32 bytes = r4

User-mode accessible registers:
- '32 general purpose registers'
  -> 'r0' : hardwired to zero
  -> 'r31': the link register for jump-and-link JAL instruction
- 'HI/LO registers'
  -> 2^32bits for multiply and divide.
- 'Program Counter registers'
  -> Not directly visible and modified implicitly by jump and branch instructions.

Branching and Jumping:
- Branching/Jumping have a branch delay slot.
- The instruction after a jump instruction is always executed prior to the destination of jump.
- There is always a '1 instruction delay' for each instruction.
- The point where I notice that I have to perform a jump in the execution pipeline, by that time the
  following instruction for example would have been added to the pipeline already.
- Some micro-processors have logic to cancel a half-started instruction.
  Some micro-processors notice before the instruction and stall the pipeline.
  This micro-processor just lets it finish. It will force the compiler to ensure the instructions that come after
  the Jump dont matter.
- 'NOP' instructions will be filled in the space following the Jump if the processor knows what its doing.

Jump and Link Instruction - we need to know where to jump to + where to return:
- 'Return Address Register (RA)'is used to return from function call.
- Example:
  
main:
  jal   function	// jump to function
  nop
  lw    r3, (r6)    // continue with operation
  .
function:
  sw    r2, (r3)    // perform operation
  jr    r31			// return to main and continue

Compiler Register Conventions:
- The compiler writer gets to choose what register is used for what kind of operations.
- Typically, certain subsets of the register set will be dedicated to certain tasks e.g. managing local variables.
- In this architecture, registers are not enforced.
- 'Compiler Calling Convention': A low-level scheme for how functions receive params from the caller and how they
								 return a result.
  -> E.g. register 2-3 (v0-v1) are values returned by functions.
          register 4-7 (a0-a3) are the first four params by functions.
- Refer to online resources for Compiler Calling Conventions

============================================================================================================
FUNCTION STACK FRAMES
============================================================================================================

Each function call allocates a new stack frame for local variables, the return address, previous frame ptr etc.
- 'Frame Pointer': start/base of current stack frame for the function
- 'Stack Pointer': end of current stack frame for the function

Local vars are allocated on the stack.
The compiler manages the stack of the particular thread of execution that the code is generated for.
The compiler knows how many local vars are being used, if extra space is needed on the stack or not.
The compiler will keep track of a SP on all architectures in one of the registers.
It may keep track of a Frame Pointer, which is the base of the local stack frame for that function

// f(1) function
----------------- => Frame Pointer
||     f(1)    ||
|| stack frame ||
----------------- => Stack Pointer

// f(1) function calls f(2)
More space is generated on the stack by moving the SP down by amount needed for local vars of f(2).
Moves FP down to the base of f(2).
-----------------
||     f(1)    ||
|| stack frame ||
----------------- => Frame Pointer 
||     f(2)    ||
|| stack frame ||
----------------- => Stack Pointer

The same scenario will occur if f(2) calls f(3).
If f(2) returns, it will go back to the first state.

Args 1-4 of a function have space reserved for them.
- However, beyond 4 args of the function that we are calling, it stores
  remainder of them on the stack at offsets.

-----------------
||     arg 6   ||
----------------- 
||     arg 5   ||
----------------- 
||   args 1-4  ||
----------------- => '$fp'
||     saved   ||
||   registers ||  --> stores the $s registers
----------------- 
||     local   ||  --> stores local f() vars
||   variables ||
----------------- 
||    dynamic  ||  --> stores dynamically allocated memory i.e. alloc()
||      area   ||
----------------- => '$sp'

============================================================================================================
COPROCESSOR 0
============================================================================================================

The processor control registers are located in 'CP0'.
- Exception/Interrupt management registers
- Translation management registers

CP0 is manipulated using 'mtc0 (move to)' and 'mfc0 (move from)' instructions.
- mtc0 and mfc0 are only accessible in kernel mode.

CP0 Registers.
- Exception Management
  -> 'c0_cause' register: tells you what causes the exception
  -> 'c0_status' register: tells you what the status of the CPU is / was
  -> 'c0_epc' register: tells you the address of the instruction that triggered the exception
- There are more i.e. Miscellaneous, Memory Management but we will look at these later.

'c0_status' register:
--------------------------------------------------------------------------
|         IM  			|      0     | KUo | IEo | KUp | IEp | KUc | IEc |
--------------------------------------------------------------------------

	'IM'
	- individual interrupt mask bits. Allows you to turn particular interrupts on/off.
	- 6 external
	- 2 software

	'KU'
	- 0 = kernel mode:
	- 1 = user mode:

	'IE'
	- 0 = all interrupts masked
	- 1 = all interrupts enabled: masked is determined via. IM bits.

	'c, p, o' = current, old, previous


'c0_cause' register:
--------------------------------------------------------------
|  BD  |    0    | CE |  0  |  IP |  0  |    ExcCode   | IEc |
--------------------------------------------------------------

	'ExcCode'
	- Contains a code number after the exception that gives the OS an indication
	  of what it was that triggered the exception.
	- Example: ExcCode Value = 0
	  -> Mnemonic: Int
	  -> Description: Interrupt
	- Example: ExcCode Value = 8
	  -> Mnemonic: Syscall
	  -> Description: Generated unconditionally by a syscall instruction

'c0_epc' register: The Exception Program Counter
	- Points to the address of where to retart the instruction if something went wrong.
	- Assume 'sw  r3, (r4)' caused a restartable fault exception.
	  -> 'c0_epc' will contain the address of the instruction of where to return to.


'Exception Vectors'

Program Addr  |   segment   |   Physical Addr    |    Description       |
-------------------------------------------------------------------------
 0x8000 0080  |     kseg0   |    0x0000 0080     | All other exceptions |

- Majority of exceptions result in jumping to the above Program Addess and execution the code
  that lies there.

============================================================================================================
SIMPLE EXECUTION WALKTHROUGH WITH AN EXCEPTION - AT THE MACHINE / HARDWARE LEVEL
============================================================================================================

'USER MODE'						              > 'Application'
------------------------------------------------------------------------------------------->
|||||||||||||||||||||||||||||||||||||||||||||| (1) Interrupt Occurs ||||||||||||||||||||||||
------------------------------------------------------------------------------------------->
'KERNEL MODE'                         ----------------------
                                      ||||||||||||||||||||||(2) Return control
                                      ----------------------
                                      'System Call Handler'

1. Interrupt occurs as the previous instruction completed - right before start of a new instruction
2. Currently in USER MODE, with interrupts ON.
   -> 'c0_epc' stores the Program Counter so the OS can find it
   -> 'c0_status' bits |KUc|IEc| are set to 1.
   -> 'c0_status' bits |KUc|IEc| are shifted two bits to the left - |KUp|IEp| are now set to 1 + |KUc|IEc| are 0.
   Now we are in KERNEL MODE. with interrupts ON. 
2. 'c0_cause' is set with the ExcCode
   -> Since it is an interrupt exception, the ExcCode = 0.
3. 'Program Counter' gets set to 0x8000080 - 2gigs + 80 bytes - address of the general exception vector placed in PC
   -> Software is back in control.
   -> Software now knows:
    Where the application was previously.
    What state the system was in prior to the exception.
    What triggered the exception.
   -> Software has enough info to know how to handle the interrupt exception.

We will ignore for now how to deal with the interrupt exception.

'Returning from an exception':
- Prior to exiting back out, the last thing that the OS will do is to run:
  
  lw   r27, saved_epc  // Load contents of EPC - usually saved earlier to memory by the exception handler
  nop
  jr   r27			   // Jump back into user code - store the EPC back into the Program Counter
  rfe 				   // In the branch delay slot, execute a RESTORE FROM EXCEPTION instruction
                       // i.e. go through steps to unwind state of the hardware
                       //      shifts 'c0_status' bits back to the right so OUT of kernel mode
                       //      and back into USER MODE with interrupts turned back on.

  lw    r27, saved_epc  // load contents of EPC - saved earlier to memory by exception handler
  nop
  jr    r27             // jump back into USER CODE - store EPC back into the Program Counter
  rfe                   // execute RESTORE FROM EXCEPTION instruction (unwind state of hardware):
                        // -> shifts c0_status bits back to the right
                        // -> Current State: USER MODE + INTERRUPTS ON
- We are now back in the same state we were in when the exception happened.

============================================================================================================
MIPS SYSTEM CALLS
============================================================================================================

System calls are invoked via. a 'syscall' instruction.
- The syscall instruction causes an exception and transfers control to the general exception handler.
- A convention (agreement between KERNEL <-> APPS) is required as to how user-level software indicates:
  1. 'which system call is required'
  2. 'where its args are'
  3. 'where the result should go'

OS/161 System Calls
- OS/161 convention:
  -> Args are passed and returned via. normal C function calling convention
  -> '$v0' contains the syscall number
  -> On return, '$a3' contains:
     0: if success, v0 contains successful result
     !0: if failure, v0 has th errno.

============================================================================================================
USER LEVEL SYSTEM CALL WALKTHROUGH - CALLING READ()
============================================================================================================

int read(int filehandle, void *buffer, size_t size)
- Three args, one return value
- Code fragment calling the read() function:

	move   a0, s3					// a0 = filehandle
	addiu  a1, sp, 16				// a1 = sp for the buffer
	jal    40068c <read>            // jump and link to read() function
	li     a2, 1024                 // a2 = size of buffer
	move   s0, v0 				    // s0 = store return value
	blez   s0, 400194 <docat+0x94>  // test return value. If -1 then error occured.

Inside read() function:

  read:
    j      400640 <__syscall>       // Jump to syscall stub
    li     v0, 5 				    // v0 = 5 -> corresponds to the read() syscall
<__sycall>:
    syscall 						// generate a syscall exception
    beqz   a3, 40065c <__syscall+0x1c> // test success, if yes, branch to return (jr instruction at the bottom)
    nop                                // if it returns, the whole syscall is done i.e. transition -> modeswitch -> code -> entry -> copy to buffer etc.
    lui    at, 0x1000               // takes errno from OS and stores somewhere
    sw     v0, 0(at)				// in the C library to find later as the programmer
    li     v1, -1                   // returns -1 (error value)
    li     v0, -1
    jr     ra                       // jump back to where it was called.
    nop

Summary:
- From the app perspective read() looks like a normal function call.
- Most languages will have similar libraries that interface with the OS
  using the standard convention.

============================================================================================================
OS/161 Exception Handling
============================================================================================================

Exception:

    move   k1, sp 				// Save stack ptr that application was using
    mfc0   k0, c0_status 		// Load status register
    andi   k0, k0, CST_KUP      // Check the "we were in user mode" bit
    beq    k0, $0, 1f    		// If clear, from kernel, already have stack
    nop  						// Delay slot

    /* Coming from User Mode - load kernel stack into sp */
    la     k0, curkstack        // get address of curkstack
    lw     sp, 0(k0)            // get its value
    nop                         // delay slot for the load

1:
    mfc0   k0, c0_cause     // Now, load the exception cause
    j      common_exception // Skip to common code
    nop                     // delay slot

common_exception:

	 * At this point:
	 *    Interrupts are off.
	 *    k0 contains the exception cause value.
	 *    k1 cotains the old stack ptr.
	 *    sp points into the kernel stack.
	 *    All other registers are untouched.

	 addi  sp, sp, -164   // allocate 164 bytes

     * lots of memory shuffling

     sw   k1, 152(sp)     // saving the User SP onto the Kernel stack (saving of state)
     nop

     mfc0 k1, c0_epc 	  // grabbing the Exception Program Ctr (of where we have to return back to)
     sw   k1, 160(sp)     // storing the EPC value on the kernel stack at a particular offset

     * saving of all the other registers

     * save the remaining exception context information

     sw   k0, 24(s0)      // save k0 on the stack - k0 was loaded with the cause value earlier
     mfc0 t1, c0_status   // load and store status and put it on the stack
     sw   t1, 20(sp)
     mfc0 t2, c0_vaddr    // load and store another register and put it on the stack
     sw   t2, 16(sp)

     // WE ARE GENERALLY SAVING EVERYTHING ONTO THE KERNEL STACK

     /* Prepare to call mips_trap(struct trapframe *) */

     addiu a0, sp, 16     // Create a ptr to the base of the SAVED REGISTERS + STATE in the first arg register.
     jal   mips_trap      // Mips Trap is now C code.
     nop                  // delay slot

     /* Now there are all these registers saved on the Kernel stack */

struct trapframe {
     THIS WOULD BE A STRUCT IN C THAT HAS THE SAME
     LAYOUT HAS EVERYTHING THAT WE DUMPED ON THE STACK.
}

     /* Now we arrrive in the C kernel */

     This is the General Trap (exception) handling function for mips.

     void mips_trap(struct trapframe *tf) { }

What happens next?
- The kernel will deal with whatever caused the exception:
  -> syscall, interrupt, page fault
  -> potentially modifies trapframe 
- mips_trap() eventually returns back to assembly code when it exits.

exception_return:
    Largely a reverse of all the assembly code we did along the way in.
    Slowly un-stacking everythig off the Kernel stack.

    We get to a point where we have no room anymore, as we have put back all the content
    that the application is expecting to see.

    We load what we return to 'EPC'
    We load the 'SP' with the 'USER STACK PTR' that was saved on the way in.
    We jump to the EPC value.

    We are backin the APPLICATION.




















