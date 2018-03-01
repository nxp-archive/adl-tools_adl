=================================
Software Design Methodology Notes
=================================

This document contains notes on the methodology used to develop ADL, including
specific processes for doing various things.

:Author:
  Brian Kahne
:Contact:
  bkahne@freescale.com

.. contents:: Table of Contents
   :backlinks: entry

.. sectnum::

Required Software
=================

ADL is primarily developed on Linux.  A run-time portion of the project is
supported on Windows.  Please refer to the README file for required software and
the associated packges on the GAIN/CDE network.

Build Environment
=================

ADL uses the GNU build environment, consisting of autoconf, automake, GNU make,
and libtool.  If you are not familiar with these tools, you should refer to
their online documentation (just google for the tool and the first link is
usually its homepage).

Here are some common tasks and how to do them:

Building From Subversion
------------------------

The ADL project can be built after a fresh checkout from Subversion via two
methods:

1. Invoking ``./configure`` directly.  This requires fewer tools (autoconf and
   automake are not required) but does not allow the user to rebuild the
   Makefiles, so it is best for users who simply wish to build and install ADL,
   rather than do development.

2. Invoking ``./autogen.sh``.  This requires the complete set of GNU build tools
   but means that Makefiles can be regenerated.  The default options configure
   the project to be built without optimization, statically linked, and with a
   cache file.

The ADL project's makefiles have been written so that a parallel build will
work.  On a multi-core system, you can significantly decrease build type by
using an option of ``-j4`` or ``-j8``.

Adding A New Directory
----------------------

1. Create the directory using ``mkdir``, then add it to Subversion using ``svn add``.

2. Add a ``Makefile.am`` file (assuming that this directory will contain something
   which needs to be built, distributed, or installed.

3. Add the name of the subdirectory to the **SUBDIRS** variable in the parent
   ``Makefile.am``.

4. Edit ``configure.ac`` in the top-level directory and add a path to the new
   ``Makefile`` (note: *not* Makefile.am) to the list of files processed by
   AC_CONFIG_FILES.  Note the need for a backslash for line-continuation
   purposes.

5. Run make.  This will rerun configure and automake, assuming that you
   configured with ``--enable-maintainer-mode``.  This flag is set if you
   invoked ``configure`` via ``./autogen.sh``.

Adding A New External Dependency
--------------------------------

Whenever an external binary, library, or other resource is added to the project,
its existence must be checked for by ``configure.ac``.  This way, it allows the
user to specify that the file exists in another location and it provides much
better error detection than a "file not found" error.

For example, if you need to use an external program called ``prest``, you might
add a check to ``configure.ac`` such as::

  DOCDIR=doc
  ac_prest=$PATH
  AC_PATH_PROG(PREST, [prest], no, $ac_prest)

  if [[ $PREST = "no" ]] ; then
    AC_MSG_WARN([Could not find prest in the path.  Please modify your path.])
    PREST=
    DOCDIR=
  fi
  AC_SUBST(DOCDIR) 
  AC_SUBST(PREST) 

This will check the current PATH variable for the program and store its location
in the variable PREST, or the value "no" if not found.  If the value is "no", a
warning message is generated and the PREST and DOCDIR variable are cleared.  In
this case, the DOCDIR variable is used in a Makefile.am's SUBDIRS variable, so
if it is cleared, it means that the document directory (doc), will not be
built.

After the test, the variables must be registered for substitution using
AC_SUBST.  This means that they may be used in Makefile.am files or any other
files listed in the AC_CONFIG_FILES line.

More complex tests might run the program with sample input to check that
everything works or to make sure that it is possible to use a library with the
version of the compiler that was selected.

For anything other than a trivial check, please move the check into its own
macro and store this macro in a file with a ``.m4`` extension in the ``/macros``
subdirectory.

Building And Installing Documentation
-------------------------------------

The primary directory for documentation is the ``doc`` directory in the
Subversion repository::

           file:///proj/.ppc_43/vc_noec/svn/adl/doc

The main ``doc`` directory contains documentation suitable for release as
open-source.  The ``internal-doc`` directory exists to store documentation which
should remain internal to Freescale.

Documentation ending with the ``.rst`` suffix uses `reStructured Text`__, which
can be transformed into HTML using ``prest`` (contained within the
`Text::Restructured`__ CPAN package.).  The configure script will automatically
search for this program and and store a path to it- you just need to make sure
that it is accessible in your PATH.

.. __: http://docutils.sourceforge.net/rst.html

.. __: http://search.cpan.org/~nodine/Text-Restructured-0.003040/lib/Text/Restructured.pm

To build the documentation, just run ``make``.  To install it, run ``make
docinstall``- this will write it to the website directory.

If you are not in the GAIN environment, or are using /pkg tools, then you will
need to install the ``Text::Restructured`` package from CPAN
(``http://www.cpan.org``).  In short, you invoke the CPAN shell thusly::

   perl -MCPAN -e shell 

and install a module, with the ``install`` command::

  install Text::Restructured
  install LWP
  install Term::ReadKey

In order to set up your own CPAN repository, you will need to modify your
``~/.cpan/CPAN/MyConfig.pm`` file to point to the desired installation
directory.  The important keys to change are::

    'build_dir' => q[<build dir>],                      # Path to use for building libraries.
    'makepl_arg' => q[PREFIX=<install dir>],            # Installation directory.
    'mbuildpl_arg' => q[--install_base <install dir>],  # Installation directory.

reStructured Text can also be used to create PDFs via Latex.  This requires that
a modern version of the Latex package be present.  Configure will check to see
if this is the case.  Specifically, we use the `pdflatex` program to generate
PDF files from .tex files.

Committing Code
===============

Before a developer commits any code to the Subversion repository, the following
steps must be taken:

1.  The regression test must pass.  To run the regression, issue the following
    command from the top-level directory::

      make check

    Regressions may be run in individual directories by running ``make check``
    in a relevant subdirectory.

2.  The regression should pass from a distribution.  To do this, issue the
    following command from the top-level directory::
    
      make distcheck

    This creates a distribution tarball, then unpacks it, builds an optimized
    ADL from this distribution, and runs the regression.

3.  Update the ``ChangeLog`` file.  If you use emacs or xemacs, you can issue
    the ``add-change-log-entry`` command to automatically open the file and add
    a new entry.  You should add a brief description of what was added and
    mention any DDTS bug numbers fixed by this change.

4.  Now you can do the commit.  Make sure to commit at the top-level.  It is
    acceptable to simply use your ``ChangeLog`` entry as the CVS commit message.

Revision Control
================

ADL currently uses Subversion for revision control.  The repository location
is::

    file:///proj/.ppc_43/vc_noec/svn/adl

To check out the tree::

    svn co file:///proj/.ppc_43/vc_noec/svn/adl/trunk adl

You may also use the http protocol to check out the tree.  The advantage to this
approach is that you do not need a local account on the TX32 CDE network; just a
valid CoreID and access permissions (explained later).

To check out with this approach::

    svn co http://cad.freescale.net/svn/adl/trunk adl

Repository Access Control
-------------------------

When using the file/NFS-based Subversion protocol, e.g. ``file://...``, a user
must have a local account on the TX32 CDE network and be a member of the UNIX
group ``tools``.

To use http-based access, the user need only have a valid CoreID.  By default,
anyone who authenticates with their CoreID may check out the ADL tree.  Commits,
however, are currently restricted to ADL developers.

Access to the tree via http is controlled by the file ``svnaccess`` located at
the root of the tree, e.g. ``file:///proj/.ppc_43/vc_noec/svn/adl/svnaccess``.
Refer to `Subversion Access Control`_ for more information about the syntax of
this file.

.. _Subversion Access Control: http://svnbook.red-bean.com/en/1.4/svn-book.html#svn.serverconfig.pathbasedauthz

Testing and Nightly Regressions
-------------------------------

To ensure that a developer hasn't forgotten to check in a file, that there isn't
a makefile error associated with producing a distribution, etc., ADL should be
checked using the ``test-build`` script located in the ``scripts`` directory on
a regular basis.  Currently, the ``test-build`` script is run nightly using the
``remote-test`` driver, which is run via a cronjob.

To set up your own use of this script:

1. Change to an empty directory which will be used for build testing.  A
   directory that is local to the machine is a good choice because it will be
   faster than a network drive and we don't care if this data is lost.

2. Copy the ``test-build`` script to this directory and write-protect it.  This
   ensures that the user doesn't accidentally modify the script.

3. Run ``test-build``.  This will delete an existing build in the current
   directory, check the project out from Subversion, build it, run the
   regression, create a distribution, then build the project and run the
   regression from this distribution.  If it completes with a 0 return code,
   everything has worked.  If there is an error, fix it.

Bug Tracking and Home Page
==========================

ADL currently uses Trac for bug tracking and as a wiki.  The homepage is `here`_

.. _here:  http://cad.freescale.net/adl

To submit a bug, simply click on the *New Ticket* tab.

The wiki should be used for documentation which is not tied to a specific
release.  So, for example, proposals, discussions, etc. are suitable for the
wiki, but APIs and reference guides should be written as reStructured Text and
version controlled in the ``doc`` directory.

The Trac environment is located at::

    /proj/.ppc_43/vc_noec/trac/adl

Static documentation is stored into the ``htdocs`` directory (this is the
default ``webdir`` location that configure will use).

In general, all configuration of Trac can be done via the web-admin interface,
but if necessary, the configuration file ``conf/trac.ini`` may be edited.  This
may require a restart of the server, however.

The web server itself is running on the host ``vs58-tx32`` in the directory
``/var/httpd/trac``.  Refer to http://cad.freescale.net/Trac/wiki/ToolsApache for
information about managing this server.

Regression Testing
==================

If a feature is added and a regression test does not exercise that feature, then
the feature *does* *not* *exist*.  In other words, if you add a feature, or find a
bug, make sure to add a regression!

Regressions should be placed in the *adl/tests* directory and should be grouped
into appropriate subdirectorioes.  For example, front-end tests are found in
*adl/tests/fe* and ISS tests are found in *adl/tests/iss*.  When creating a
brand-new regression subdirectory, follow the steps in `Adding A New
Directory`_.  Then, edit ``Makefile.am`` and add the name of the test to be run
to the ``TESTS`` variable.  Programs that should be built for regression testing
purposes, but not distributed, should be entered in the ``check_PROGRAMS``
variable.

A common regression-testing framework is *adl/scripts/rdriver.pm*.  Refer to
this file for documentation.  In most regression subdirectories, a ``regress``
script exists.  New tests can be added by simply adding a new line to the
``@Tests`` array located at the beginning of the file.

Some helper routines are provided for easing the task of adding tests.  Refer to
*adl/scripts/regress_utils.pm* for more information.  For example, to run a
model and then use the ``tracediff`` tool to compare the input file to the
output file, add an entry such as the following to the ``@Tests`` array::

  my @Tests = (
    ...
    model_test("./model","$src/test.dat"),
    ..
  );

Notice the use of the ``$src`` variable.  This variable is set to the location
of the ``regress`` script and allows the regression to be able to find its files
even if ADL is configured and built in a separate directory from the source
tree.  Running ``make distcheck`` tests this capability.

Releases
========

First, increment the version number of ADL.  The version number is found at the
top of configure.ac.  The line looks like this::

  AC_INIT([adl],[1.6.9])

Where the version is ``1.6.9``.  Increment the micro number (9 in this case) for
most releases.  We increment the minor number (6 in this case) quarterly.  Make
the edit, save the file, then rebuild the project and check everything in to
Subversion.

Before making a release, make sure that the nightly regression (``test-build``)
has run through to completion.  This checks lots of corner cases, such as making
sure that a minimal-build configuration will work.  It is OK to proceed with a
release directly after incrementing the version number, without an intervening
nightly regression.

Next, you must make sure that the project builds on Windows.  The complete
process is outlined in ADL's README file.  The basic steps are:

1. Check out ADL, via TortoiseSVN, into Windows.

2. Load the ``adl.sln`` file in Micrsoft Visual C++ 2005 (or newer).

3. Copy ``adl/tests/iss/mod23.cc`` (generated file) to ``model\model.cc``.  This
   subdirectory should be at the same level as the ``adl`` project directory.

4. Copy ``adl/tests/uadl/dlx_cache_split_safe{-iss.cc,.h,.cc}`` (generated
   files) to ``uadl-model``.  This subdirectory should be at the same level as
   the ``adl`` project directory.  Rename ``dlx_cache_split_safe.cc`` to
   ``uadl-model.c``.

5. Clean, then build the solution in both the **Debug** and **Release**
   configurations.  After each build, run ``model.exe`` and ``uadl-model.exe``
   with ``in4.dat`` to make sure that they work.

Then, you should review the generated release notes and make sure that
everything looks good.  To generate and view the release notes, issue the
following command in the main adl directory::

  make show-release-notes

This will generate the release notes and then invoke a browser (specified by the
``Browser`` make variable; defaults to ``firefox``) to display the file.

Remember that the release note bullet points are extracted automatically by
looking for items in ChangeLog which start with ``**``.  These are then inserted
as HTML, so that users may include fragments of HTML in the ChangeLog.  You may
need to edit the ChangeLog in order to fix formatting issues, etc.  Also, make
sure that all appropriate defects have been closed in Trac.  Defects and
enhancements closed since the last release will automatically be added to the
release notes, so make sure that everything is up to date.

Now you can start the release.  On the CDE/GAIN network, for releasing into
_TOOLS_ and /pkg, we use the ``cadinstall`` tool to make releases.  The policy
for this tool is ``fs-cadinstall-/3.3.11`` in /pkg.  The tool is usually invoked
thusly::

  cadinstall file:///proj/.ppc_43/vc_noec/svn/adl --pkg | tee cadinstall.log

This will install adl into /pkg using the package name ``fs-adl-/<version>``.

You do not need to have checked out ADL in order to use this tool; all work is
done by the tool in its own sandbox.  This will update the ``ChangeLog`` file
with a release banner, tag Subversion with a version number, then build ADL, run
the regression, install it in ``/pkg``, and post the documentation and a
distribution tarball to our Trac environment.

Various build parameters used by ``cadinstall`` can be set within a resource
file named ``~/.circ``.  For example::

  build=/tmp/.bkahne-build
  noprompt
  makeopts=-j4  

This will instruct ``cadinstall`` to build in /tmp and to invoke make with the
option to spawn itself four times.

If, during the build process, an error occurs which is transient, the build
process can be resumed without starting from scratch by using the ``--resume``
option.

Currently, we three versions of ADL: 32 and 64-bit for gcc 4.1.1 and 32-bit for
gcc 3.4.3.  All of these builds can be performed from a 64-bit workstation in
the CDE/GAIN environment via various options to cadinstall and the existence of
a proper environment.  

.. _install-proj:

The following bash function helps to simplify this step:

.. code_block:: sh
   :color:

   function install-proj ()
   {
    if [ -z "$1" ]; then
      echo "Usage:  install-prog <project> [--resume]"
      return 1
    fi
     if [ -z $svn ]; then
      echo "The svn variable was not set.  This must point to the base URI to use"
       echo "for checking out a project."
      return 1
     fi
     cadopts=""
    resume=no
    if [ ! -z "$2" ]; then
      case $2 in
        -resume)
               resume=yes
               ;;
               --resume)
               resume=yes
               ;;
           *)
        ;;
      esac
    fi
    if [ $resume = yes ]; then
      cadopts="${cadopts} --resume"
    fi
    case $(gcc -dumpversion) in
     3.4*)
         cadopts="${cadopts} --num-archs=1"
         ;;
    esac
    case $(gcc -dumpmachine) in
     *i686-pc-linux*)
       cadopts="${cadopts} --plat=i686-linux"
    ;;
    esac
    cadopts="$svn/$1 --pkg ${cadopts}"
    echo "Running cadinstall $cadopts"
    cadinstall $cadopts
   }

On the CDE/GAIN environment, the release process is this:

1. You may use your own workstation or a Linux compute node.  However, each
   installation, if performed in parallel, **must** be on its own machine in
   order to avoid conflicts with the ``/pkg`` installation process.

   I recommend opening three terminal windows and logging into three different
   compute nodes so that your own workstation is not bogged down.

2. Source the relevant environment files so that you have the following:

   1.  An environment in which ``gcc`` refers to a 64-bit version of gcc 4.1.1.
       For example, the following policy file for ``collect`` identifies the
       relevant gcc-specific tools.  You would then include the ``/run`` path to
       this collection in your PATH::

         gcc-/4.1.1 { add -aslink bin/*  }
         bzip2-/1.0.5 { add -aslink bin/*  }
         tcltk-/8.5.2
         perl-/CCE-5.8.8-Tk804.027
         bigloo-/3.0c-4
         flex-/2.5.33
         bison-/2.3
         fs-rnumber-/2.1.24-gcc_4.1.1
         fs-plasma-/0.8.47-gcc_4.1.1

    2. An environment in which ``gcc`` refers to a 32-bit version of gcc 4.1.1.
       You may use the collection described above, but include the ``/run32``
       path to this collection in your PATH.

    3. An environment in which ``gcc`` refes to a 32-bit version of gcc 3.4.3.
       You would then include the ``/run`` path to this collection in your PATH::

         gnu-gcc-/3.4.3-binutils-2.15 { add -aslink bin/*  }
         bzip2-/1.0.5 { add -aslink bin/*  }
         tcltk-/8.5.2
         perl-/CCE-5.8.8-Tk804.027
         bigloo-/3.0c-4
         flex-/2.5.33
         bison-/2.3
         fs-rnumber-/2.1.24-gcc_3.4.3
         fs-plasma-/0.8.47-gcc_3.4.3

3. In each window, run the following command::

     install-proj adl

   The `install-proj`_ bash function is shown above.

4. Change to an empty directory and copy the script ``make-oss-release`` (from
   the ADL scripts directory) into this directory.  Then run the script with an
   argument of the release tag just created by cadinstall.  For example, if the
   version of ADL installed was 1.2.3, then the release tag will be `v1_2_3`.
   So, run the following command::

        ./make-oss-release v1_2_3

   This will create a tarball suitable for release outside of Freescale and will
   copy it into a copy of the open-source release directory.  This script
   requires the following environment variables to be set:

   ${oss}:
     Specifies the open-source release tree destination directory.  You will
     need to rsync this from the server in order to start for the first time.

   ${svn}:
     Set to the base URL for obtaining the ADL project.  Usually it has the
     value ``file:///proj/.ppc_43/vc_noec/svn`` if you are in TX32 on the
     CDE/GAIN environment.

   ${RSYNC_PASSWORD}:
     Set to the password required by the external server.  You may obtain this
     by contacting `Larry Gwinn`_.

.. _Larry Gwinn:  Larry.Gwinn@freescale.com

   This script removes everything else in this directory, so you can simply
   keep this script in a standard location and run it whenever you make a new
   release.

5. Check to make sure that everything has worked:

    1. Go to the ADL homepage:  http://cad.freescale.net/adl

    2. Click on ``Download`` and make sure that the release tarball and release
       notes are present.

    3. Click on ``Open Source`` and then click on the ADL link.  Make sure that
       the target file exists and is the tarball of the newest release.

    4. Check to make sure that the documentation linked to on the home page is
       up-to-date.

6. Send out the release notes.  A release notes file is automatically generated
   and placed into the ``src`` directory of each release, e.g. for version
   1.2.3, this would be::

     /run/pkg/fs-adl-/1.6.9-gcc_4.1.1/src/readme-1.6.9.html

   Create an email with a suitable subject, e.g. "ADL Release: 1.2.3" and
   attach this html file.  Then send to ADLLIST@freescale.com.

If you do not wish to release to the /pkg location, then you must perform the
release manually.  The general steps to perform are:

1. Tag ADL remotely::

     svn cp file:///proj/.ppc_43/vc_noec/svn/adl/trunk file:///proj/.ppc_43/vc_noec/svn/tags/<tag> adl

2. Check out a fresh copy of ADL from Subversion using this tag::

     svn co  file:///proj/.ppc_43/vc_noec/svn/tags/<tag> adl

3. Configure, specifying the destination location, build, run the regression,
   and install::

     cd adl && ./configure --prefix=<install path> && make all check install

4. Create and distribute release notes.  You can use the ``make-release-notes``
   script to automatically query Trac for bug information and to extract feature
   notes from the ChangeLog file.

C++ Code Development
====================

Coding Standards
----------------

The ADL project does not have strict coding standards, but a few basic rules to
apply.

Garbage Collection
++++++++++++++++++

ADL libraries use the Boehm-Weiser garbage collector, so in general, the developer
should not explicitly delete memory.  This is a great way to reduce the chances
for memory leaks and double deletions, but unfortunately there are a few gotchas
that have to be kept in mind in order to avoid problems:

1.  To create a class which will be allocated from the collector, inherit from
    the class ``gc``.  This is defined in the header "gc/gc_cpp.h".  This class
    will be collected when no pointers reference it, but the class's destructor
    will not be called.  Usually, this isn't a problem, since the primary reason
    to use a destructor is to deallocate constituent elements, which is no
    longer necessary if they are also collected.  However, it may be necessary
    to execute a destructor if a member variable references a system resource,
    such as a file stream.  In this case, the class should inherit from
    ``gc_cleanup``.

2.  To allocate memory which is not a class, or cannot inherit from a
    collectible base class, use the placement allocator as follows::

      new (GC) <object>;

    For example::

      new (GC) char[BufSize];

    When allocating large blocks of memory which contain no internal pointers,
    it is much more efficient to allocate this from the collector's atomic
    heap.  To do this, use the placement allocator with the ``PointerFreeGC``
    symbol::

      new (PointerFreeGC) char[BigBufSize];

3.  By default, the garbage collector cannot see memory allocated by the default
    allocator of the STL container classes.  This means that placing a pointer
    to a collected class into a standard STL container can cause random segfaults
    because the collector may try to collect these objects, since it does not
    know that they are still referenced.

    To avoid this problem, use the ``gc_`` containers declared in the
    ``helpers`` subdirectory.  These are just partial specializations of the STL
    containers which use an allocator which allocates from the garbage
    collector.  Their use is otherwise identical to the default STL containers::

      #include "helpers/gc_vector.h"

      typedef gc_vector<Foo> FooVect;

    If you still want to use the standard containers, then make sure to *not*
    put any collectable objects into them.  You must also ensure that any
    objects which contain these default containers execute their destructors.
    So, if the parent class is collected, it must inherit from ``gc_cleanup``.

Naming Scheme
+++++++++++++

The following are loose guidelines for naming conventions used within the ADL project:

1. It is acceptable to use either camel-case or "_" separated lower-case names for
   method names.  Method names should start with a lower-case letter.

2. Class names should start with a capital letter.

3. Member variables of a class should start with "_", e.g. ``_foo``, to
   differentiate them from local variables, which should start with a lower-case
   letter.

4. Globals should start with an uppercase letter.  Exceptions are allowed for
   when this would conflict with a type name.

5. Preprocessor defines should generally be fully-capitalized.

Integer Types
+++++++++++++

If there is a need to describe an integer of a specific width, e.g. an unsigned
32-bit integer, the standard C99 types declared in ``inttypes.h`` should be
used.  These take the form of uint<n>_t for unsigned integers and int<n>_t for
signed integers.  For example, ``uint32_t`` denotes an unsigned 32-bit integer.

This header is included in ``helpers/BasicTypes.h``.

Perl Code Development
=====================

This section contains some notes about coding standards for Perl.

Script Locations
----------------

In general, scripts belong in the *adl/scripts* directly.  Exceptions are
regression scripts (which should be named *regress*) and should be placed into a
subdirectory within *adl/tests*, and scripts which are to be installed for
customer use; these maybe placed either in *adl/scripts* or within their own
subdirectory.

Coding Standards
----------------

Basic Perl coding standards used within this project are:

1. The ``perl`` interpreter should be located via ``/usr/bin/env``.  In other
   words, the shbang line should look like this::

      #!/usr/bin/env perl 

   This will invoke the version of perl specified by the user's PATH variable.

1. Every script should ``use strict``.  Avoid creating globals except at the
   global scope; the use of ``strict`` generally enforces this behavior.

2. If the script uses libraries which are contained within this package, it
   should make use of ``FindBin`` to locate its dependencies, e.g.::

      use FindBin;
      use lib "$FindBin::RealBin";

3. Scripts, in general, should use POD documentation for help information, then
   use ``Pod::Usage`` for displaying this information.

4. Command-line option processing should be done using ``use Getopt::Long`` for
   consistent behavior.

5. Try to write your Perl code in a clean, consistent manner, avoiding clever
   syntactic tricks.  Aim for code that can be understood by another
   programmer!
