============================
 ADL Perl database interface
============================

.. contents:: Table of Contents
   :backlinks: entry




Command line
============

Syntax::

  adl-db --core=<core_name> --object=<object_name> --cond=<perl_cond> 
               --format=<format_string> --sort=<fields_list> --expand=<obj> 
	        --output=out.txt database_file.pl
                         
Describe options briefly, will be clarified below:

--output=<filename>, -o=<filename> 	 Output file name.
--core=<core name> 	 Optional, specifies the core of interest. In general, ADL database may consist of several cores and systems, systems may also consist of systems and cores. Core consists of registers, regfiles etc. In such a case core can be defined like this: ``--core='PSystem::PChip::Core'``. If only one core is present in the database then it is not necessary to specify this option.
--object=<object name>, -obj=<object name>		 Optional, specifies object to be queried, Possible options are: ``instrs, instrfields (or ifields) , regs, exceptions, contexts (or ctxs), caches,eventbuses ,memories (or mems), core-level-hooks (or hooks)``. Default is ``instrs``. Note that ``regs`` refers to registers and register files.
--cond=<expr>, -c=<expr>   Optional, specifies query that is interpreted as a filter, the program outputs entries that satisfy the condition specified, syntax resembles perl. Default is to '1', which means that all objects pass this test and will be printed out.
--format=<string>, -f=<string>    Optional, specify the format of output. Default is ``'name:40,syntax:40'``, where ``name`` is the ADL name of, say, instruction.
--sort=<string list>, -s=<string list>     Optional, specifies fields by which to sort output, default is object's name.
--multi-line, -M	Optional, specifies that output should be wrapped on multiple lines if not fitted to a single line.
--expand=<obj list>, -e=<obj list>   Optional, specifies object to be expanded. Only applicable to instructions and instruction fields. If object list is empty then in case of instructions every instructions every field to be expanded. When applied to instruction fields only empty list is allowed.   
--help -h    Will print help, In conjunction with ``--cond``,``--format``,``--object`` or ``--expand`` will provide some addtional information about this options.


Instruction example::

  adl-db --cond='name =~ /do/' --format='name:5,syntax:10,action:10, fields:5'
    --sort='block,type'  --expand='FFF,BBB,CCC' --output='aaa' file.pl

* *--core* is not specified, so only one core is present.
* *--object* is not specified, so instructions are assumed.
* *--cond='name =~ /do/'* : Take instructions which name matches *'do'*.
* *--format='name:5,syntax:10,action:10, fields:5'*: Means: print name in a 5 character's wide column, syntax in 10, action also in 10 and a list of fields in 5 character's wide column.
* *'--sort='block,type''*  : Sort according to a *block* and then according to a *type*, *name* is implicitly the last element of the sort priority.
* *'--expand='FFF,BBB,CCC'*: Let us assume that 
 
 * ``AAA`` is a nested field referencing to the register file ``D``, so it will be expanded according to its syntax (if present): ``"d0,d1","d0,d2"``, etc. 
 * For ``BBB`` let's assume that it is an enumerated field with the following possible values:   ``("r0)","r0)+","r0-)","r0)-", ... ,"r15)-")``. 
 * ``CCC`` will be expanded accordingly to the syntax of entries of the regfile it is referenced.
 * ``IMM`` cannot be not expanded.

Output::

	   Name     Syntax           Action   Fields
 	   sdo     do AAA,d0,d1       {...}    AAA,FFF
 	   sdo     do AAA,d0,d2       {...}    AAA,FFF
  	           ...
	   sdo     do AAA,d15,d0      {...}    AAA,FFF
   
	   do1     do AAA,(r0)        {...}    AAA,BBB
	   do1     do AAA,(r0+)       {...}    AAA,BBB
         	   ...
	   do1     do AAA,(r15)-      {...}    AAA,BBB
	   
	   do2     do AAA,r1.l        {...}    AAA,CCC
	   do2     do AAA,r2.h        {...}    AAA,CCC
	               ...
	   do2     do AAA,d5.e        {...}    AAA,CCC
	
	   do5     do AAA,-IMM        {...}    AAA,IMM

One more example of expand, now for an instruction fields (instruction fields
are from the previous example). Note, that when instruction field is expanded it
does not require *--format*::

   Options:
   --object=instrfields --cond='(name eq BBB) || (name eq FFF))' --expand 

   Output:   
           Field: BBB
         Entry        Value
         "r0)"          0
         "r0)+"         1
               ...         
         "r15)-"        35

           Field: FFF
         "d0,d1"        0
              ...
         "d1,r56"       3
       
    
Instruction database
=====================

Users are currently interested in instructions database, so we will go through
all fields that instruction entry has and describe how to query and how to
specify it in the output.
  
Instruction fields
----------------------

Pseudo instruction field have two representation: abstract definition and inst
implementations.

For example: 'DaDb' as an abstract field and a specific implementation is
defined in a specific instruction. This will not lead to confusion::

   --cond='fields =~/DaDb/' : all instructions with DaDb - clearly refers to abstract.
   --cond='fields->ref eq "D"' also  abstract (about sub-objects see below).

Format specificator *'bits'* will output bits and values of all instruction
field (shows implementation of DaDb)::

   Name       Action   Prefix     Fields     Bits
   sdo        {...}      0        AAA,DaDb   DaDb:[0,1],[5,5],[2,3],
 			                     BBB:[10,11],[15,15],[12,13]
 			                     OPCD1(30):[10,11],[15,15],[12,13]


Instruction fields are also can be also shown using the *'enc'* specificator in
*'--format'*, in this case encoding is shown as a bit vector - 'R' for
*reserved*, *0* and *1* for opcode and any other characters specify fields::

   --output='name:enc'
  Name          Encoding
  mac           FSS1001FFGG10033333

There should be special field in ADL description that defines which character
represents the field.

Opcodes are not considered as fields in *--cond* and in *--output*, 

Database representation of fields is an array of strings, so using 'fields'
keyword in the condition means to test each field individually.

Examples of queries::

  --cond='fields =~/FFF/' -- return instructions that have non opcode field which name matches 'FFF'.
  --format='fields:5'     -- print non-opcode field names (abstract)
  --format='enc:30'       -- print encoding

Syntax
----------

Database representation::

  "syntax" => "IMACLHUU jjj,JJJ,FFF",

Query: will handle it like a string, for example::

  --cond='syntax =~/FFF/' 

Output specified by *syntax*.

Alias
---------

Query:

* test if it is an  alias ::

   --cond='is_alias'

* test if is an alias to some specific instruction::

  --cond='is_alias eq 'bmclr4''

Output: 

* If *action* specificator is used then alias shown output::
   
    Name    Syntax            Action
    do1     do a,b      alias="do11 a(a),b(b),c(0)"

* The *alias* specificator can be also used, it is empty for usual instructions::

   --cond='is_alias && name =~ /do1/' --output='name:4,syntax:4,action:4'
      Name    Syntax            Action
      do1     do a,b      alias="do11 a(a),b(b),c(0)"
   --cond='name =~ /do1/' --output='name:4,syntax:4,alias:4"
      Name    Syntax            Alias
      do1     do a,b      alias="do11 a(a),b(b),c(0)"
      do11    do a,b,c      


Description string
----------------------

Description string is accessed using *desc* key-word::
  
  --cond="desc =~ /halt/"
  --output="name:43:desc"

Name
--------
In ADL resources have names, allow to ask about it.
Queries::

  --cond="name eq FFF" --object=field
  --cond="name != /add/"

Instructions will have additional field called *iname* which is the name deduced from syntax, i.e.::

  define (instr=add3) {
    syntax = ("add %f,%f",...);
  }

Here the *name* is *add3*, while the *iname* is *add*. Note, that the name is
unique among objects of the same type but the *iname* is not. Also, it only can
be used with instructions.


Attributes
--------------
Will handle like user-defined field in the database.

Unparameterized attributes will be handled like strings.

Query::

  --cond='attrs =~ /agu_/' 

will match instructions with attributes agu_l,agu_r etc.

Attributes are added as keys to the object's hash.  If the attribute has no
value, then it is still added with an empty value.  This allows the user to
query for an attribute using ``exists`` or to check the value using a normal
expression::

  --cond='exists first or second > 7'

This will match any items which have an attribute named **first** or an
attribute named **second** with a value greater than 7.

Attributes parametrized by integers will allow two types of questions: 

* Whether the object has some attribute, in this case "attrs" is used::

  --cond='attrs =~/cycles/'

* Whether the object has some attribute with some specific constraint on value,
  in this case name of attribute is used like a database field specificator::

  --cond='cycles > 3'

assuming that a resource has integer attribute 'cycles'.

Attributes parametrized by string will be handled in the similar way, only
parameter now is a string.  Suppose some instruction has attributes::

    attrs=(meminfo("read_d0"),cycles(3));

So queries::

  "attrs =~/mem_info/"    ask for instructions attributed with some mem_info
  "mem_info  =~ /^read/" - mem_info with read at the beginning (like in our example).

Parametrized by a list of integers or a list of strings will not be supported. 


Source/Targets
-------------------

In database represented as an array of strings:: 

  "inputs" => ["DRTable(HHHH)?",]
  "outputs" => ["DRTable(HHHH)?","L?",]

Query will apply to every string individually::

  --cond='inputs =~ /FFF/' means that one of inputs has FFF.

 Query:
 --cond='outputs =~ HHHH' --output='name:14,syntax:14,inputs:14,outputs:14'
 Output:
  Name   Syntax       Inputs   Outputs
  add  add FFF,HHHH   D(HHHH)  D(HHHH)
 
     
New fields (not present at QISET)
-------------------------------------

*  block: handle like attribute (i.e. parametrized by a string)     
*  prefix: boolean (true/false)  ( --cond='prefix=true and ...')
*  pseudo: boolean (true/false)  ( the same)
*  type: like attribute         ( --cond='type =~ /OneWord/')
*  width/fetch_width: numeric   ( --cond='width > 32')
*  helpers: array of strings     ( --helpers='helpers =~ /saturation32/)

Nested instructions
-------------------

Nested instructions will be interpreted as usual instructions, i.e not aliases::

  define (instr = add) {
   define (instr = add1) {
     alias = add ...
   }
   define (instr = add2) {
     alias = add ..
   }
   action = {
     ...
   };
  }
    
Instructions *add1* and *add2* should be queried like usual instructions.



Nested database objects
------------------------

By nested objects we understand nested database structures. For example instruction field has an optional reference::

  define(instrfield=DaDb) {
    ref=D;
  }

We will support the following syntax for some of objects::

 object->subobject

We know that field of any instructions should be described in "instrfields" database, so this can be used::

  --object=instr --cond='width==32 && "fields->ref eq D"

This means the following: choose instructions which have width equal to 32, and have at least one field referenced to "D".  

It is also possible to omit "fields" from the query, in this case every subobject will be queried::

  --object=instr --cond='width==32 && "valid_ranges"



Summary of queries and output
===============================

Every format specificator can be used in condition.
All above-mentioned fields can appear in format.

* *name*:       unique ADL name
* *iname*:        only for instructions: mnemonic name as written in syntax.
* *enc*:          encoding in binary format
* *fields*:      non-opcode abstract fields
* *bits*:        all fields with bits
* *syntax*:      syntax
* *action*:      action or alias definition
* *is_alias*:    non-empty in the case of alias, keeps name of the target instruction.
* *alias*:       if alias then alias definition, otherwise empty.
* *desc*:        description string
* *inputs*:      source fields
* *outputs*:     destinations fields
* *block*:       handle like attribute (i.e. parametrized by a string)     
* *prefix*:      if it is a prefix instruction 
* *pseudo:*      if it is a pseudo instruction
* *type*:        which type 
* *width*:       virtual width (including prefix bits)
* *fetch_width*: real width
* *helpers*:     name of helpers instruction uses 
* *attrs*:       list names of all attributes of this instructions
* *<attrname>*: For usual attribute print true or false, for functional attribute print its values. 



Query about other objects (not instructions)
===============================================

Specified by the option *--object*.  Fields of objects can be used in the
condition and in the output specification.  Every query should be correct
perl-like condition, i.e::

 --object=regfiles --cond='name ~/AA/'
 --object=regfiles --cond='size >= 32'
 --object=regfiles --cond='alias->name eq "D"'
  
Output:
Default output will show perl structure, i.e:: 
 
  --object=regfiles --cond='name eq C1Tablee' --format='size:7,width:10,sparse:20'

   Size     Width     Sparse
    8        32      "0" => { "name => "EMR"}
                     "1" => { "name => "D"}
                     "5" => { "read" => { "action" => " "}} 

We can define special output for some cases, if needed.   

Additional features to be implemented 
======================================

- Possibility to output a perl database filtered by some condition.

- Print header describing database used and query used.

- Multi-page printing, with header on each page.

- Possibility to explore DB and dynamically understand what can be asked and
  how, for example new integer key was added to ADL -> report that it can be
  queried as integer).

Future features (not implemented at the moment)
================================================
 
* Allow formating of the description string::

   """
   General::
     Adds two data registers .
   Remarks::
     Affects SPR
   """;

  If this formatting is used user can print out specific part of the description string in the output::
 
   --output="name:43:desc->General"

  The program will search description for the tag "General::" and print everything until the next tag (in this case "Remarks").
   
* Support for register fields in sources or destinations. So if, for example,
  only true bit of the status register is written then targe looks like::

 "outputs" => [ "SR.T ],

* Support for attributes parametrized by lists.


Query syntax
============

Perl database must reflect structure of ADL file.  Named define block is key of
anonymous hash referecing another hash, in which erever key (including unnamed
blocks) is a property.  Key implicitly accesseble as a property called "name"::

  ADL :
  
  define (obj=B) {
    prop1  = 8;
    prop2 = 32;
    define (prop3) {
      define (prop4=1) { reg  = ((8,15));
    }
    prefix = b;
  }
  
  PERL:

  "B" => {
    "width" => 32,
    "pseudo" => 1,
    "size" => 8,
    "prefix" => "b",
    "shared" => 0,
    "read" => {
       "alias" => {
          "name" => "R",
	  "map" => {
	    "0" => 8,
	    "1" => 9,
	    "2" => 10,
	    "3" => 11,
	    "4" => 12,
	    "5" => 13,
	    "6" => 14,
	    "7" => 15,
	  },
	},
      },
    	
Nice to have
============

* Automatic formatting: User will not specify width of some columns, program
  should adjust it automatically.


