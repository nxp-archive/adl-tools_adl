===============
ADL To-Do Items
===============

* Const expression evaluaton in sign-extend does not work

* N-bit support: support jit, transactional iss.


* Assembler

  * Handle PC in more general situations (now it is just line number,
    4-byte increment).

  * Support SC encoding.

  
* tracediff:

  * Print out standard file:line-number messages so we can use it within
	emacs.

* ISS:
  
  * Refactor walkers so that we have a generic ADL-aware walker, e.g. it knows
	about register-file writes and such and calls an appropriate translate
	function.

  * Create base ADL walker.

    * Modify VarWalker and InstrWalker to use this new class.

  * Use contexts to model an e300-type architecture with TGPRs.


  * Display command-line arguments to the test writers.

  * Add ability to toggle whether an aliased register/register-file reports
	itself or its target on a write.

  * Add logging of CIA and NIA back in.  Have testwriters suppress registers
	with the nia, cia attributes.

  * JIT technology:

    * Template based ala QEMU.

  * TCL scripting:

    * Hook up events so that users can register tcl event handlers.

  * Add enumeration to register/register files. Should be supported by Dat writer/reader
    introspection and action code. 

* Doc support:

  * Can we switch to DocBook?

* Language:

  * intbv:

    * Add ++ and -- operators to intbv.
    
    * Better support for large literal constants.  We should support the use of
      "_" within constants and we should convert large constants into const
      static globals transparently::

        bits<128> foo = 0x1234567_12345678_12345678_12345678;

      should be converted into something like:

        static const bits<128> sym2342_1("0x1234567_12345678_12345678_12345678");

        ...

        bits<128> foo = sym2342_1;

  * IPC:  We need to be able to define typed-channels and communicate.  

    * Support blocking and non-blocking (perhaps via a re-run capability?).

  * Use += for adding to keys within define/defmod blocks and have = always
    overwrite/replace a value.  For example, attributes are inconsistent, as are
    MMU lookup tests.

  * Make inheritance a general concept.  Modify MMU lookups to use this generic
    concept.

    For example, use "interface" as a new block type.  Then add syntax for using
    these interfaces.  The data model should remove all interfaces::

      interface (lookup=TlbBase) { }

      define (lookup=tlbCam) : TlbBase { }

    Allow for multiple inheritance.

  * Create a solution to allow action functions in defmod blocks to re-use the
    code from prior defines/defmods.

  * Need method for handling arbitrarily large concat operations.  Handle
	within RtlWalker?

  * Need to able to instantiate cores/systems with parameters.

  * We need a general macro-language solution for ADL.  We currently use the C
    preprocessor, but this is not general enough.  I want something that can
    handle looping, conditionals, etc.  Possibly use Perl as the preprocessor
    language.

* Infrastructure
   
  * Evaluate cmake: Should we switch away from auto*?

  * Convert to Subversion.
