=============================
ADL ISS Model Design Document
=============================

This document describes the architecture of a generated ISS model.

:Author:
  Brian Kahne
:Contact:
  bkahne@freescale.com

.. contents:: Table of Contents
   :backlinks: entry

.. sectnum::

Overview
========

An ISS model is produced by the ``make-iss`` program, or the ``adl2iss`` wrapper
script, from an ADL description.  Usage of these programs is covered by the `ADL
Design Notes`__ document.  This document covers the structure of the generated
models.  Two types of ISSs can be currently generated: Untimed and time-tagged.
An untimed ISS, as you might imagine, has no concept of time.  Instructions are
simply executed sequentially.  A time-tagged ISS executes instructions in the
same manner, except that each resource (currently just registers) has a time
associated with it.  Current time, and the time that a resource is available, is
set by looking at the instructions operands' times and adding the instruction's
latency.  Thus, this allows the user to approximate the behavior of a pipeline
without the overhead of actually modeling the pipeline.

__ adl-language-reference.html

The model can be separated into three main parts:

1. The generated code.  This is created by ``make-iss`` from the ADL
   description.

2. The support library.  This code is used by all models and contains structures
   such as the sparse memory model.

3. The stand-alone execution framework.  This library is only needed if the model
   is going to be used as a stand-alone ISS and contains various file readers and
   writers for initializing the model and storing logged results.

Generated Code and Support Library
==================================

Header Files
------------

The following header files are referenced throughout this document:

* ``ModuleInterface.h``: This header file defines the interface for the model
  and should be used by anything that wants to interact with it.  For example,
  the stand-alone-ISS framework includes this.

* ``ModuleSupport.h``: This file is *only* included by the generated model.  It
  contains the memory model and various support routines.  This is a private
  header and can only be included once due to various global declarations
  contained within.

Class Structure
---------------

The model generator, ``make-iss``, produces a single C++ file from an ADL
description.  Each core is transformed into a class which publicly inherits from
``IssCore``.  The base class provides a small number of services, such as the
ability to set a register, display all registers, set the starting address, etc.
This base class is declared in ``ModuleInterface.h``.  For a complex ISS that
contains systems of cores, each system is represented as a structure which
declares any needed child systems or cores, then instantiates them.

Each core object registers itself by calling ``registerCore``, which places a
reference to the core object, indexed by its path in the system hierarchy, into
a hash.  These cores can then be retrieved by calling ``getCore``.  This allows
an external framework to retrieve all cores in the system for the purpose of
initializing them, receiving logging information, etc.

The main interface to the system or core are the following routines:

* ``simulate``:  Cycles each core, or in the case of a system, each child
  object, once.  In other words, each core executes one instruction.

* ``resetSimulation``:  Resets each core to its initial state.

These methods are called by generated functions called ``simulate`` and
``resetSimulation``.  These are the main interface functions for executing and
reseting the simulation.  The ``simulate`` function currently loops until all
cores are inactive.

Instructions
------------

Instructions are decoded in the ``simulate`` method.  The method is gated by a
boolean, called ``_active``.  If the core is ever halted, this flag is set to
false.  If the core is active, it assigns the next-instruction-address register (NIA)
to the current-instruction-address register (CIA), then uses the CIA's address
to read from memory.  Note that the names for the CIA and NIA are not fixed:
They are determined by the model builder by looking for registers that have the
``nia`` and ``cia`` instruction class.

The data read from memory is then decoded by using a nested switch statement.
For PowerPC, the outer-most switch statement corresponds to the primary opcode
and the next level of switch corresponds to the secondary opcode.  However, the
model does not know that a field is a "primary-opcode"- this is derived from the
instruction encoding and what fields are given hard-coded values.

At a leaf in the switch statement, the NIA is updated based upon the
instruction's size, ``log_instr`` is called if tracing is enabled (logging is
described in Logging_), then the instruction handler function is called.  The
arguments to this function are each of the operands that the instruction has.
These are extracted from the instruction using generated extractor functions.
For example, for ``mtspr``, the ``get_SPRN`` function creates a 10-bit ``intbv``
by extracting bits 16 through 20 and 11 through 15, then permuting them.

The instruction handler function is basically just the code specified in the
``action`` key in the ADL function definition.  Only minor changes are made:
Register references are modified so that the function-call operator is called,
e.g. ``CTR`` is changed to ``CTR()``.  Also, the various language extensions,
such as ``var`` are handled.  In this case, ``var`` is converted to using gcc's
``typeof`` operator.

Once the instruction handler returns, any exceptions are handled, and the
function returns.  Note that most functions within the model are inlined,
including all of the argument extractors and instruction handlers.

Registers
---------

Each register in the core is declared as a class within the core's class.  A
basic register is just a wrapper around an ``intbv``.  More complicated
registers, such as aliases, might not actually contain an ``intbv`` but will
have the same interface.  A register's interface is:

* ``operator=``:  Handles assignment to the register.  This calls the
  register's write-hook, if one exists, and is the interface used by
  instruction-handler code.

* ``operator()``:  Handles reading from a register.  Each instruction-handler
  has all register references transformed into calls to this operator.  If a
  register has a read-hook, it is called here.

* ``get_value()``:  Handles reading from a register with no side-effects.
  This is used by initialization logic, such as the test writer in a
  stand-alone ISS.

* ``set_value(intbv<N>)``:  Handles writing to a register with no
  side-effects.  This is used by initialization logic, such as by the test
  reader in a stand-alone ISS.

The actual data object within the register is either a ``Tag`` (for time-tagged 
ISSs) or ``NoTag`` (for untimed ISSs).  The ``NoTag`` class has no functionality-
it simply presents the same interface as ``Tag`` and is pretty much inlined out
by the compiler.  The ``Tag`` object will be discussed in the `Time Tagged ISS`_
section.

Register files are handled in an analogous fashion to registers, except that
they inherit from ``RegFile``, a simple class which contains an array of objects
(either ``Tag`` or ``NoTag``).  The interface to each register-file class is
basically the same as for registers.  The main difference is that ``operator[]``
is defined, which returns a reference object which refers to the particular
register being referenced.

Memory
------

The memory model is declared in ``Memory.h``.  The memory representation class
is called ``Node``.  It is a short, wide tree.  Internal nodes are pages of
pointers to child nodes.  Leaf nodes are pages of bytes.  The depth of the tree
is determined by the template parameter Total.  Through the use of recursive
template types, it generates the internal node types, stopping at level 0, which
is the leaf node (page of bytes).  The ``MemTree`` type is what is actually
instantiated and is a wrapper which instantiates the ``Node`` template with the
proper parameters.

So, for example, ``MemTree<64,4>`` will create a tree of four levels of 16 bits
each; three internal nodes and the addressing within the 64K page.
``MemTree<64,3>`` creates a three-level tree: 22 bits for the internal nodes (4
Meg) and 20 bits for the page size (1 Meg).  For performance, the ``MemTree``
class cache the last page accessed.

The ``Memory`` object wraps the ``MemTree`` object and provides support for
reading and writing 32-bit, 16-bit, and 8-bit quantities which may be
misaligned.  This class is defined in ``ModuleSupport.C`` and is currently
hardcoded to create a memory model with 64 address bits and four levels of
lookup, i.e. four total levels to the tree.

The routines ``mem_read32``, ``mem_write32`` and equivalent functions for
reading/writing 16 and 8 bits are the interface used to access the ``Memory``
object.  These are then wrapped by the routines ``mem_log_read32``,
``mem_log_write32``, etc.  The logging routines call the appropriate read/write
routine and then call a logging template (``logMem32``, etc.) with a template
parameter of either ``LogRead`` or ``LogWrite``.  These objects call the actual
tracing routines of ``log_mem_read`` or ``log_mem_write`` if the ``Tracing`` and
``TraceMode`` flags are set to true.  The first flag, ``Tracing``, is a constant
and is declared by the generator at the top of the generated file.  Thus, the
compiler is able to eliminate the conditional, and thus entirely remove all
logging logic if tracing is not enabled.  The second flag, ``TraceMode``, is set
at run-time.  In a stand-alone ISS, this would be set by the value of
command-line parameters.

Finally, a class called ``MemoryModel`` wraps the ``log_mem_read32`` function
and others and is defined in the generated model as ``Mem``.  This is the memory
object used by instruction handlers.  Its interface methods, ``read`` and
``write``, are both templated on the size of the access in bits and call
structures ``read_internal`` and ``write_internal`` which have partial
specializations for handling different access sizes.  For example, a call to
``Mem.read<32>(addr)`` will call ``read_internal<4>``, which then directly calls
``log_mem_read32()``.  Partial specializations exist for other reading and
writing values of 8, 4, 3, 2, and 1 bytes.  Other values are handled by a
generic function which does 32-bit reads for as much as it can, then handles any
remaining bytes.

Logging
-------

The intermediate-results logging interface is declared in ``ModuleInterface.h``.
As noted earlier, code for it is only generated if the model generator is told
to create it.  The logging interface consists of the following functions:

* ``log_instr``: Called when an instruction is executed.  This is called with a
  pointer to the core that is executing the instruction.  All subsequent
  intermediate-result calls are relative to this specified core.

* ``log_instr_times``:  Called by a time-tagged ISS 

* ``log_reg_write``:  Called on a register write.

* ``log_regfile_write``:  Called on a write to a register file.

* ``log_mem_write``:  Called on a write to memory.

* ``log_mem_read``:  Called on a read from memory.

Stand-alone Framework
=====================

The stand-alone framework may be linked with a generated model to create an
executable ISS able to read in initial state from a variety of files and write
its intermediate and final results to a file in a variety of formats.  The
primary framework code is contained within ``ExecSupport.C``.  

It contains ``main``, parses command-line parameters, and instantiates the
necessary test reader and writer.  The type of reader and writer is generally
derived from the input or output filename extension, but this may be overridden
using the ``--input-format`` and ``--output-format`` command-line arguments.

The basic flow is:

* ``main`` resets the simulation by calling ``resetSimulation``.

* ``init`` is called, which processes command-line parameters and instantiates
  the necessary reader and writer.  The writer is stored in a global called
  ``writer`` and the current reader is stored in ``reader``.

* ``readInput`` is called.  This tells the test reader to read its input and
  initialize the model.

* ``simulate`` is called.  This executes the model.

* ``writeOutput`` is called.  This tells the test writer to generate its
  output.

* ``finalize`` is called.  This currently just prints some final output
  information, such as elapsed wall-clock time.

Test Readers
------------

All test readers are derived from ``Reader``, declared in ``Reader.h``.  

There are currently two classes which derive from Reader, ElfReader and DatReader.
Although we do not currently support an AvpReader nor a UvpReader, we do provide
two scripts for conversion to dat format: avp2dat and uvp2dat.

The reader is responsible for parsing the specified input and setting up the
model accordingly.  The resources that it may initialize are the mmu, registers
and system memory.  All communication with the model goes through the base
reader class using the following methods: 

  ``set_program_counter_internal`` which then calls set_program_counter in ModelInterface.
                                                                                                                                                                            
  ``init_reg_internal`` which then calls initialize_reg in ModelInterface.                                                                                                                                                                              
                                                                                                                                                                           
  ``init_mem_internal`` which then calls initialize_mem in ModelInterface.
                                                                                                                                                                            
  ``init_mmu_internal`` which then calls initialize_mmu in ModelInterface.

The public interface for a reader is:

  ``readFile`` - which then parses the input file and calls the previously
  described internal methods to initialize the model.
        

Test Writers
------------

All test writers are derived from ``Writer``, declared in ``Writer.h``.

The DatWriter is a streaming writer while the UvpWriter buffers the output.

All communication with the model goes through the base writer class using the
following methods:

  ``set_program_counter_internal`` which then calls set_program_counter in
  ModelInterface.  This method is used by the UvpWriter when it removes the
  final halt instruction from it's instruction list and sets the program counter
  back before it queries for all final register values.  

The public interface for a Writer is:

   ``pre_initialize`` - used to reset the testwriter.

   ``post_initialize`` - used to tell the testwriter initialization is complete so
   that it can capture the memory and register state for initial values. 

   ``pre_finalize`` - used to tell the testwriter the test is complete so that it can
   capture the memory and register state for final results. 

   ``post_finalize`` - used to tell the testwriter to clean up.
                                                                                                                                                                            
   ``log_used_addresses`` - used by initialization to let the testwriter know which
   addresses will be used in the testcase.  If a new format comes around that
   doesn't initialize all used memory in the input file, then this function may
   need to be called by the logging interface rather than the initialization.
   For that case an initial value will need to be assumed.
                                                                                                                                                                            
   the Logging_ methods described above                                                                                                                                                                             

   ``writeResults`` - called by writeOutput to tell the testwriter to write out the testcase
   
   ``regfile_reg``, ``reg``, ``mmu`` - these methods are part of the communication mechanism
   between the testwriter and the core through the ModelInterface.  Since the
   testwriter does not know what resources the core has, it calls getRegs and
   getMmu through the ModelInterface which then calls showRegs and showMmu
   directly on the core.  Those methods then call back through the
   ModelInterface to reg/regfile_reg and mmu which then calls the corresponding
   methods in the testwriter. 


                                                                                                                                                                            
   .
                                                                                                                                                                            



Time Tagged ISS
---------------

As mentioned earlier, the time-tagged ISS is an untimed ISS with the addition of
time tags on various resources (currently just registers).  This is implemented
by having each register (and each element in a register file) use a ``Tag``
object, which contains a time-tag.  The interface to ``Tag`` (which is the same
as ``NoTag``) is:

* ``get_data()``:  Return the data element.  In the non-constant case, this
  updates the max-time accumulator (explained later) with the current item's
  time tag.

* ``set_data()``:  Sets the data element.  This updates the max-time accumulator
  with the current item's time tag and sets this element's time to the max-time
  accumulator plus the instruction's latency.

The basic architecture of the system is identical to that of an untimed ISS,
except for the use of the ``Tag`` objects.  There is also a max-time accumulator
which keeps track of the times of the instruction's operands.  The main
simulation loop is also modified slightly:  Each core gains a member which
stores current time.  Upon entry to ``simulate``, the global accumulator
variable (AccumTime) is set to the core's current time and at the end of the
function, the core's time is updated with the accumulated value.

