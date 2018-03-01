
Sketches of How to Do Interesting Stuff in ADL
==============================================

Overview
--------

We need to model not just multiple heterogeneous processors, but also things
that are less clever than a processor, and features in processors such as
threadswitching. We introduce one new feature to ADL to make this happen: the
instruction fetch hook. This is a function called by the generated ISS every
time an instruction is about to be fetched and executed. It has the ability to
do any semantic action needed, with the smae constraints as the 'action' for any
instruction.

1. How to describe a pure-hardware state machine, such as a serial interface, in
   ADL:

   * ADL allows us to have multiple "cores" running interleaved (one
     "instruction" from one processor, and then one from the next, ...) The
     processors can be of the same type or of different types.  They can share
     registers.

   * We will represent the RISC core with one ADL core and the serial interface
     state machine with another.

   * One would create a new architecture and a core to instantiate this
     architecture. The architecture will have registers to hold state. At least
     one register will be shared with the RISC core in the overall
     system. There will be no instruction set but there will be an
     instruction-fetch hook.

   * The instruction fetch hook will pol a register shared with the RISC
     core. When the value in that register changes to a 'start' value, the state
     hook function will start executing code which implements the state
     machine. Through use of an un-architected private register, the state
     machine can preserve state as the ISS cycles through the simulated cores,
     so that any amount of state transitions can be done per instruction
     executed on all the other cores.

     For example::

       define (core=Serial) {

         define (reg=Control) {
           width = 32;
           define (field=Active) {
             bits = 0;
           }
         }

         define (reg=State) {
           width = 32;
         }

         fetch_hook = func {
           if (Control.Active == 1) {
             if (State == 1) {
               ...
             } else if (State == 2) {
               ...
             } else {
               ...
             }
           }
         };
       }

   * As the state machine does work which the core needs to know about, the
     state machine will update appropriate shared registers which the RISC core
     will eventually poll. If the core is an ordinary core, writing the shared
     register might have an action hook which generates an interrupt; or the
     value may just sit there waiting for a program to read it via an
     instruction. (The choice depends on the architecture of the system, not on
     ADL)

   * Eventually, the state machine finishes the task, tells the RISC core that
     it's done by writing to a shared register, and resets its input register
     ready to be tasked again.

2. How to describe something like a DMA engine in ADL:

   * A DMA engine in this example isn't just a simple state machine as in
     example 1. It's able to execute a channel program.

   * The same approach is taken. The DMA engine is another core, with its own
     registers, its own ISA (in this case, the ISA is the channel program
     "ISA"), some registers shared with the RISC core, and an instruction fetch
     hook.

   * To start a DMA, an instruction on the RISC core writes a value to a shared
     register, which causes the DMA's hook function to start fetching DMA
     commands from memory at the address specified by the value written into the
     shared register.

   * The DMA "processor" simply executes the channel program, writing back
     values to registers shared with the RISC core as appropriate. Again,
     presumably these writes can update status information to be polled, or can
     cause an interrupt to be generated.

   * Describing a multichannel DMA engine should be straightforward

3. How to describe something like a thread scheduler in ADL:

   * In this example we assume that the RISC core has a scheduler which can at
     any moment between instruction executions choose to run a separate "thread"
     of code. The core will have (in this example) support for some number of
     threads, each with its own set of GPRs.  This is implemented by describing
     the RISC core as a system with each thread described as a core.

     The core will want to switch threads when some external event occurs. The
     external event is described using a register shared between the core and
     the outside world. The scheduler behaviour is described again using the
     instruction fetch hook.

   * Every instruction fetch hook reads the shared register. If it holds a value
     indicating that the relevant thread is active, then it proceeds with normal
     execution of the instruction.  If the thread is not active, then the fetch
     hook does nothing and yields control.

Current Status of ADL
---------------------

Support for systems of cores, with the sharing of registers and register files,
already exists within ADL.  The main features that need to be added in order to
be able to implement what is described above are:

1. The addition of a fetch-hook function.

2. More flexible sharing of registers, and potentially other types of resources,
   between cores within a system.  Currently, the sharing is limited to cores of
   the same type.  This should not be a major change, but some issues will need
   to be resolved, such as whether a shared register's hook functions (if it has
   any) are also shared.  For example, a shared register between a DMA and a
   RISC core might need to have a write hook on the RISC side which generates an
   interrupt.  Alternatively, this logic might be better located somewhere else.

3. The instruction-encoding features of ADL may need to be expanded in order to
   support a DMA channel program.
