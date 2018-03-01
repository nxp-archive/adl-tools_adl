==============
 ADL Home Page
==============

This is the home page for the Architectural Description Language (ADL) project.
The goal of this project is to provide a straight-forward means for specifying
the architecture and micro-architecture of a microprocessor core such that
various tools, such as simulators, assemblers, etc., may be generated from this
single description.  The idea is that a user does not need to be a software
expert in order to create high quality models and the various tools needed to
develop software and conduct trade-off analysis on a proposed new design.

The ADL project is broken up into two main parts: Architecture description via
the ADL language and micro-architecture via the uADL language.  The first
concentrates on describing the functional aspects of a processor, such as
instructions, registers, the MMU, caches, etc.  This generally corresponds to
the programmer's view of a processor.  The uADL language describes the
underlying implementation of a particular processor; its pipeline, reservation
stations, etc.

.. contents:: Table of Contents
   :backlinks: entry

Contacts
========

* `Brian Kahne (team lead)`__

__ http://directory.freescale.net/default.asp?numParms=1&hasSubmitted=1&selFormat=html&selSort=5&selAttrib1=motCoreID&selLogic1=is&txtValue1=bkahne

* `Hangsheng Wang`__

__ http://directory.freescale.net/default.asp?numParms=1&hasSubmitted=1&selFormat=html&selSort=5&selAttrib1=motCoreID&selLogic1=is&txtValue1=R00095

* `Zigu Haras`__

__ http://directory.freescale.net/default.asp?numParms=1&hasSubmitted=1&selFormat=html&selSort=5&selAttrib1=motCoreID&selLogic1=is&txtValue1=R54472

* `Sergey Khristo`__

__ http://directory.freescale.net/default.asp?numParms=1&hasSubmitted=1&selFormat=html&selSort=5&selAttrib1=motCoreID&selLogic1=is&txtValue1=B08077


Language Information
====================

`ADL Overview Presentation`__:  Some slides discussing the reason for developing
ADL (pdf).

__ ADL-Overview.pdf

`ADL Language Reference Manual`__: The main reference manual describing the ADL
architectural description language.  (a PDF version is also `available`__).

__ adl-language-reference.html
__ adl-language-reference.pdf

`uADL Language Reference Manual`__:  The main reference manual describing the
uADL micro-architectural description language.

__ uADL_manual.pdf


Downloads
=========

ADL distributions may be obtained from here__.

__ distributions

Related Links
=============

The open-source web site for ADL is here__.

__ http://opensource.freescale.com/fsl-oss-projects/

The main mailing list for ADL releases and user support is
`ADLLIST@freescale.com`__

__ ADLLIST@freescale.com

Developers' Corner
==================

Documentation
-------------

`ADL software methodology guide`__

__ methodology-notes.html

`ADL model developers' guide`__

__ developers_guide.html

`ADL requirements guide`__

__ adl-requirements.html

`Front-end design documentation`__

__ adl-fe-design.html

`Generated ISS design documentation`__

__ adl-iss-design.html

`Generated ISS scripting interface documentation`__ (a PDF version is also `available`__).
                                                                                                                                                                              
__ adl-iss-cli.html

__ adl-iss-cli.pdf

`Generated ISS plugin interface`__ (a PDF version is also `available`__).

__ adl-iss-plugins.html

__ adl-iss-plugins.pdf

`Documentation on the ADL DAT trace format`__ (a PDF version is also `available`__).

__ dat-format.html

__ dat-format.pdf

`Documentation on the ADL command-line/scripting interface`__ (a PDF version is
also `available`__).

__ adl-iss-cli.html

__ adl-iss-cli.pdf

`Using an ADL ISS with GDB`__ (a PDF version is also `available`__).

__ adl-gdb-howto.html

__ adl-gdb-howto.pdf

`Integrating an ADL ISS into another application, such as a system model`__ (a
PDF version is also `available`__).

__ adl-iss-integration.html

__ adl-iss-integration.pdf

Software Development
--------------------

Defects and enhancement requests for the ADL project may be submitted `here`__.

__ http://www.somerset.sps.mot.com/cgi-bin/projects/tools/bugtrack/custom_ddts.cgi?project=adl&class=CADTools

Open defects may be viewed `here`__.

__ http://tlv.sps.mot.com/cgi-bin/ddts-view/defect.scm?project=adl

A discussion Twiki for ADL and uADL may be found `here`__.

__ http://twiki.freescale.net/bin/view.cgi/Projects/AdlDiscussion

.. note:: The Twiki site is currently under export control.  If this is your
   first time visiting the site, hit ``Cancel`` when prompted for a password and
   then fill out the export-control questionnaire.
