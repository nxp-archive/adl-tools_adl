#
# Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
#
# You may distribute under the terms of the Artistic License, as specified in
# the COPYING file.
#

# This is required setup information.
AC_INIT([adl],[3.17.2])
AC_CONFIG_SRCDIR([fe/Data.h])
AM_INIT_AUTOMAKE([no-define -Wno-portability])
m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])

AC_CONFIG_HEADERS(adl_config_internal.h)
AX_PREFIX_CONFIG_H(adl_config.h)

AM_MAINTAINER_MODE
AC_CONFIG_MACRO_DIR([./macros])

# Get any user-specified flags here.  We then ignore the default stuff that
# comes from Autoconf.
user_cxxflags="${CXXFLAGS}"

AC_PROG_CXX
AC_LANG([C++])

# These are various tests.  The first two
# make sure that there is a C compiler and
# the install program.
AC_PROG_CC
AC_PROG_INSTALL 

AM_CPP_SETUP(4,8)

# Silence warning: ar: 'u' modifier ignored since 'D' is the default
AC_SUBST(AR_FLAGS, [cr])

AM_CONDITIONAL([OPT_ENABLED],[test $OPT_ENABLED = yes]) 

MINGW=false
case `uname` in
	MINGW32*)
		MINGW=true
		;;
	*)
		MINGW=false
esac
AC_SUBST(MINGW)

# Check the version of Perl being used.  Ignore for MinGW for now, as we don't
# really support that stuff yet.
if [[ $MINGW = false ]]; then
	# Check Perl verison.
	AC_MSG_CHECKING([that Perl has the requisite version (5.8.8)]);
	if `perl -e " require 5.8.8; " 2>/dev/null`; then
		AC_MSG_RESULT([yes])
	else
		AC_MSG_RESULT([no])
		AC_MSG_ERROR([Perl 5.8.8 is required in order to build ADL.])
	fi
  # Make sure that Getopt::Long is new enough.  If it's not, then we don't get
	# the expected behavior for negated options, e.g. --no-<opt> doesn't work,
	# only --no<opt>.
	AC_MSG_CHECKING([that Perl Getopt::Long has the requisite version (2.33)]);
	if `perl -e " use Getopt::Long 2.33; " 2>/dev/null`; then
		AC_MSG_RESULT([yes])
	else
		AC_MSG_RESULT([no])
		AC_MSG_ERROR([Perl Getopt::Long module 2.33 is required in order to build ADL.])
	fi	
fi

# Make sure that a readlink program exists.
adl_readlink=no
ADL_READLINK( adl_readlink=yes , AC_MSG_WARN([Could not run readlink.  ADL requires the readlink command in order to resolve symbolic links properly.]))

# Some behavior is dependent upon compiler version.  This defines GCC3
# if the compiler is GCC 3.x.
gcc3=no
if [[ $GCC_MAJOR -eq 3 ]]; then
	gcc3=yes
fi
AM_CONDITIONAL([GCC3],[test $gcc3 = yes])

## Setup to use libtool.
enable_shared_orig=${enable_shared:=yes}
LT_INIT
enable_shared=${enable_shared_orig}

# Check to see if we want to create everything, or just the run-time.  If the front-end is disabled,
# then you can run models, but not create them.
AC_MSG_CHECKING([if the user wants to build the frontend..])
AC_ARG_ENABLE(frontend,AS_HELP_STRING([--enable-frontend],[Create the front-end support libraries and programs.]),
[case "${enableval}" in
  yes) 
	lang_frontend=true
	;;
  no)  
	lang_frontend=false
	;;
  *) 
	AC_MSG_ERROR(bad value ${enableval} for --enable-frontend)
	;;
esac],[lang_frontend=true])

AH_TEMPLATE([LANG_FRONTEND],[Defined and set to 1 if we're creating the front-end libraries and programs.])	
if [[ $lang_frontend = true ]] ; then
	AC_MSG_RESULT([yes]) 
	LANG_FRONTEND=1
	AC_DEFINE([LANG_FRONTEND],1)
else
	LANG_FRONTEND=0
	AC_MSG_RESULT([no]) 
fi

AC_SUBST([LANG_FRONTEND])
# Use this within Makefile.am to distinguish between building the front-end and not.
AM_CONDITIONAL([LANG_FRONTEND],[test x$lang_frontend = xtrue])

# Check to see if we want to create a minimal build.  This just builds the
# support libraries necessary for a minimal ADL ISS shared object (one generated
# with --target=base-so by adl2iss).  If so, then many of the dependencies are
# not required.
AC_MSG_CHECKING([if the user wants a minimal build])
AC_ARG_ENABLE(minimal,AS_HELP_STRING([--enable-minimal],[Create a minimal set of support libraries for a "thin" ADL ISS shared object.]),
[case "${enableval}" in
  yes) 
	minimal_build=true
	;;
  no)  
	minimal_build=false
	;;
  *) 
	AC_MSG_ERROR(bad value ${enableval} for --enable-minimal)
	;;
esac],[minimal_build=false])

AH_TEMPLATE([MINIMAL_BUILD],[Defined and set to 1 if we're creating a minimal build.])	
if [[ $minimal_build = true ]] ; then
	AC_MSG_RESULT([yes]) 
	AC_DEFINE([MINIMAL_BUILD],1)
else
	AC_MSG_RESULT([no]) 
fi

# Use this within Makefile.am to distinguish between a full vs. minimal build.
AM_CONDITIONAL([MINIMAL_BUILD],[test x$minimal_build = xtrue])

## Check for flex
FlexRequiredVersion=2.5.35
HaveFlex=no
ADL_FLEX($FlexRequiredVersion, HaveFlex=yes , AC_MSG_WARN([flex $FlexRequiredVersion is required in order to fully build this project.  Disabling tokenizer build rules.]))

AM_CONDITIONAL([HAVE_FLEX],[test x$HaveFlex = xyes])

## Check bison (version 3.0 or higher required)
BisonRequiredVersion=3.0
HaveBison=no
ADL_BISON($BisonRequiredVersion, HaveBison=yes, AC_MSG_WARN([bison $BisonRequiredVersion is required in order to fully build this project.  Disabling parser build rules.]))

# If we don't have bison, then use a dummy value for the bison command to avoid
# invoking the wrong one.
if [[ $HaveBison = no ]]; then
  YACC=true
fi

AM_CONDITIONAL([HAVE_BISON],[test x$HaveBison = xyes])

## Check for RNumber.
RNumberRequiredVersion=2.1.29
AM_PATH_RNUMBER($RNumberRequiredVersion, , AC_MSG_ERROR([RNumber $RNumberRequiredVersion is required in order to build this project.]))

## Check for LLVM.
LLVMRequiredVersion=3.1
#ADL_LLVM($LLVMRequiredVersion, , AC_MSG_WARN([llvm $LLVMRequiredVersion is required in order to build this project with fast-ISS support.]))
# Well, looks like the ADL JIT no longer works on RHEL6, for some reason, so I'm
# disabling it until I can get it ported to LLVM 3.9.
LLVM_EXISTS=false
AM_CONDITIONAL([LLVM],[test $LLVM_EXISTS = true])

## Do we have fork()?
AC_FUNC_FORK
AM_CONDITIONAL([HAVE_WORKING_FORK],[test x$ac_cv_func_fork_works = xyes])

## Configure coroutines.
##ADL_CORO

# These libraries are only required for a full build.
if [[ $minimal_build = false ]] ; then

	## Configure to use plasma.
	PlasmaRequiredVersion=0.8.47
	AM_PLASMA($PlasmaRequiredVersion,$lang_frontend, , AC_MSG_ERROR([Plasma $PlasmaRequiredVersion is required in order to build this project.])) 

	## We don't use the plasma library, and it may not exist for newer compilers, so remove it.
	PLASMA_LIBS=$(echo ${PLASMA_LIBS} | sed -e s/-lplasma// )

	## Check for tcl version 8.4.  Not supported for MinGW.
	if [[ $MINGW = false ]]; then
		TclRequiredVersion=8.4
		AM_PROG_TCL($TclRequiredVersion, , AC_MSG_ERROR([Tcl $TclRequiredVersion is required in order to build this project.])) 
	fi
	# Note:  We're not using swig right now for our TCL integration.
	## Configure to use swig
	#SwigRequiredVersion=1.3.28
	#AM_PROG_SWIG($SwigRequiredVersion, , AC_MSG_ERROR([Swig $SwigRequiredVersion is required in order to build this project.]))

fi

# We use LZ4 for compressing DAT traces.  This is an extremely fast compressor
# with fairly good results.
AH_TEMPLATE([HAVE_LZ4],[Defined and set to 1 if lz4 exists.])	
LZ4_LTLIBS=
AM_LZ4( , AC_MSG_WARN([lz4 is required in order to support compressed traces.]))
AM_CONDITIONAL([HAVE_LZ4],[test $LZ4_EXISTS = true])
if [[ $LZ4_EXISTS = true ]]; then
	 AC_DEFINE([HAVE_LZ4],1)
	 LZ4_LTLIBS="\$(top_builddir)/lz4_stream/liblz4stream.la ${LZ4_LDFLAGS}"
else
	 AC_DEFINE([HAVE_LZ4],0)
fi
AC_SUBST(LZ4_LTLIBS)

## Configure to use SystemC.  Not needed for minimal builds, but making the macro
## conditional can interfere with AM_CONDITIONAL.
SystemcRequiredVersion=2.2.0
AM_SYSTEMC($SystemcRequiredVersion, , AC_MSG_WARN([SystemC $SystemcRequiredVersion is needed for SystemC regressions to be run.])) 
AM_CONDITIONAL([SYSTEMC],[test $SYSTEMC_EXISTS = true])

## Check to see if we have RunSim for the DevTech RunSim integration.
AM_RUNSIM( , AC_MSG_WARN([Runsim not found; the Runsim integration will not be built.])) 
AM_CONDITIONAL([RUNSIM],[test $RUNSIM_EXISTS = true])

## Check to see if we have the DevTech assembler.
AM_DTASM( , AC_MSG_WARN([DevTech assembler not found; the DevTech assembler integration will not be tested.])) 
AM_CONDITIONAL([DTASM],[test $DTASM_EXISTS = true])

## Add suffix to bin and lib directories of installation 
AC_MSG_CHECKING([for architecure suffix])
ac_archsuffix_default=
archsuffix=
AC_ARG_WITH([archsuffix],AS_HELP_STRING([--with-archsuffix],[specify an arhcitecture suffix.  The default is $ac_archsuffix_default.]),archsuffix=$withval,archsuffix=$ac_archsuffix_default)

AC_MSG_RESULT([$archsuffix])
AC_SUBST(archsuffix)

## Check for web directory for storing info file.
AC_MSG_CHECKING([for info destination directory])
ac_infohome_default=/proj/ppctools/www/a-z/adl
InfoHome=
AC_ARG_WITH([infohome],AS_HELP_STRING([--with-infohome],[specify a web destination directory for the info.xml file.  The default is $ac_infohome_default.]),InfoHome=$withval,InfoHome=$ac_infohome_default)
if [[ -d $InfoHome ]]; then
  AC_MSG_RESULT([$InfoHome])
else
  AC_MSG_RESULT([Directory $InfoHome not found.  Documentation will not be installed.])
fi
AC_SUBST(InfoHome)

## If the user has supplied a directory as a documentation destination, use that
## instead.  If specified, then we install to this directory.  To always use a
## web directory, set WebDir to a default location.
WebDir=/proj/.ppc_43/vc_noec/trac/adl/htdocs
AC_MSG_CHECKING([if we should install to a documentation directory])
AC_ARG_WITH([webdir],AS_HELP_STRING([--with-webdir],[specify a web destination directory for documentation.]),WebDir=$withval,)
if [[ ! -z $WebDir ]]; then
	AC_MSG_RESULT([$WebDir])
else
	AC_MSG_RESULT([no])
fi

## If we're installing to a web directory, then see if a 'releases' subdirectory exists.  If so, use that as the 
## release directory.  If the user has supplied an override, then use that.  If all else fails, install directory to
## the web directory.
RelDir=
AC_MSG_CHECKING([for a release directory])
AC_ARG_WITH([reldir],AS_HELP_STRING([--with-reldir],[specify a release directory for the release tarball.  If not specified, and --with-webdir is used, then <webdir>/releases will be used.]),RelDir=$withval,)  
if [[ -z $RelDir ]]; then
  RelDir="${WebDir}/releases"
fi
AC_MSG_RESULT([$RelDir])

## If the user has supplied a specific open-source web URL, use that instead of the default.
DefaultOssUrl="http://opensource.freescale.com/fsl-oss-projects"
OssUrl=
AC_MSG_CHECKING([for alternate opens-source web URL location])
AC_ARG_WITH([ossurl],AS_HELP_STRING([--with-ossurl],[specify an open-source web URL for stylesheets, etc. (instead of using ${DefaultOssUrl}).]),OssUrl=$withval,)
if [[ ! -z $OssUrl ]]; then
	AC_MSG_RESULT([$OssUrl])
else
	AC_MSG_RESULT([no])
	OssUrl=${DefaultOssUrl}
fi

## This is the default stylesheet to be used for documentation generation.
AdlStyle="${OssUrl}/adldoc.css"

AC_SUBST(WebDir)
AC_SUBST(RelDir)
AC_SUBST(OssUrl)
AC_SUBST(AdlStyle)

# Check that we have prest and fop.

# Prest is required in order to build the documentation.
AC_ARG_WITH([prest],AS_HELP_STRING([--with-prest],[specify a path to use to find the Prest reStructured Text processing program.]),ac_prest=$withval)
if [[ x$ac_prest = x ]] ; then
  ac_prest=$PATH
fi
AC_PATH_PROG(PREST, [prest], no, $ac_prest)
build_docs=yes
if [[ $PREST = "no" ]] ; then
  AC_MSG_WARN([Could not find prest in the path.  Please modify your path or use the --with-prest option.  
Documentation will not be built.  You can obtain prest by installing the Text::Restructured package from cpan.org.])
  build_docs=no
  PREST=
else
  ADL_FULLPATH([PREST],prest,$adl_readlink,[for Prest's location],[Set to the path of the Restructured Text (prest) interpreter we're using.])
fi
AC_SUBST(PREST)

# Apache FOP is used to transform XHTML to PDF via XSL-FO.
AC_ARG_WITH([fop],AS_HELP_STRING([--with-fop],[specify a path to use to find the Apache FOP XSL-FO processing program.]),ac_fop=$withval)
if [[ x$ac_fop = x ]] ; then
  ac_fop=$PATH
fi
AC_PATH_PROG(FOP, [fop], no, $ac_fop)
if [[ $FOP = "no" ]] ; then
  AC_MSG_WARN([Could not find Apache FOP in the path.  Please modify your path or use the --with-fop option.])
  FOP=
else
  ADL_FULLPATH([FOP],fop,$adl_readlink,[for Apache Fop's location],[Set to the path of Apache FOP.])
fi
AC_SUBST(FOP)
AM_CONDITIONAL(FOPOK, test x$FOP != x)

# Convert is used for graphics conversion.
AC_ARG_WITH([convert],AS_HELP_STRING([--with-convert],[specify a path to use to find the convert program.]),ac_convert=$withval)
if [[ x$ac_convert = x ]] ; then
  ac_convert=$PATH
fi
AC_PATH_PROG(CONVERT, [convert], no, $ac_convert)

if [[ $CONVERT = "no" ]] ; then
  AC_MSG_WARN([Could not find convert in the path.  Please modify your path or use the --with-convert option.  Otherwise, documentation will not be built.])
  build_docs=no
  CONVERT=
fi
AC_SUBST(CONVERT)

## check fig2dev (required for uadl document generation)
AC_ARG_WITH([fig2dev], AS_HELP_STRING([--with-fig2dev],[specify a path to use to find the fig2dev program.]), ac_fig2dev=$withval)
if [[ x$ac_fig2dev = x ]] ; then
  ac_fig2dev=$PATH
fi
AC_PATH_PROG(FIG2DEV, [fig2dev], no, $ac_fig2dev)

if [[ $FIG2DEV = "no" ]] ; then
  AC_MSG_WARN([Could not find fig2dev in the path.  Please modify your path or use the --with-fig2dev option.  Otherwise, documentation will not be built.])
  build_docs=no
  FIG2DEV=
fi
AC_SUBST(FIG2DEV)

## check epstopdf (required for uadl document generation)
AC_ARG_WITH([epstopdf], AS_HELP_STRING([--with-epstopdf],[specify a path to use to find the epstopdf program.]), ac_eps2pdf=$withval)
if [[ x$ac_eps2pdf = x ]] ; then
  ac_eps2pdf=$PATH
fi
AC_PATH_PROG(EPS2PDF, [epstopdf], no, $ac_eps2pdf)

if [[ $EPS2PDF = "no" ]] ; then
  AC_MSG_WARN([Could not find epstopdf in the path.  Please modify your path or use the --with-epstopdf option.  Otherwise, documentation will not be built.])
  build_docs=no
  EPS2PDF=
fi
AC_SUBST(EPS2PDF)

AM_CONDITIONAL([BUILD_DOCS],[test x${build_docs} = xyes])

ADL_USAGE_LOG

# Check for rnv
AC_PATH_PROG(RNV, [rnv], no)

# Check to make sure that we can actually run rnv.
if [[ ! $RNV = "no" ]] ; then
  AC_MSG_CHECKING([to see if we can actually run rnv.])
  $RNV ${srcdir}/misc/adl.rnc ${srcdir}/doc/rnv-test.xml > /dev/null 2>&1
  if [[ $? != 0 ]]; then
    RNV=no
    AC_MSG_WARN([no.  Check to make sure that the executable 'rnv' will actually run.'])
  else
    AC_MSG_RESULT([yes])
  fi
fi

validate_xml=yes
if [[ $RNV = "no" ]] ; then
  AC_MSG_WARN([Could not find or run rnv in the path.  XML cannot be validated during regression without this program. This program is available at http://www.davidashen.net/rnv.html under BSD license.])
  validate_xml=no
  RNV=
fi

AC_SUBST(RNV)
AM_CONDITIONAL([VALIDATE_XML],[test x${validate_xml} = xyes])

## Get the full compiler path, if feature is enabled.  We normally want to do this, so that an installed version
## knows precisely what compiler to use.  For testing, howvever, especially when using distcc, we disable this to
## avoid possible confusion.
AC_MSG_CHECKING([if we want full compiler paths])
AC_ARG_ENABLE(full-paths,AS_HELP_STRING([--enable-full-paths],[Enable or disable use of full paths for compiler versions.]),
full_paths=${enableval},full_paths=yes)
AC_MSG_RESULT([$full_paths])

if [[ -z ${CXXPROG} ]]; then
  if [[ $full_paths = yes ]]; then
    ADL_FULLPATH([CXXPROG],$CXX,$adl_readlink,[for the C++ compiler's location],[Set to the path of the C++ compiler that we're using.])
  else
	  CXXPROG=$CXX
  fi
fi
if [[ -z ${CPROG} ]]; then
  if [[ $full_paths = yes ]]; then
    ADL_FULLPATH([CPROG],$CC,$adl_readlink,[for the C compiler's location],[Set to the path of the C compiler that we're using.])
  else
    CPROG=$CC
  fi
fi

## Perl always has to have a full path, since we use it in shbang lines, which require absolute paths.
ADL_FULLPATH([PERL],perl,$adl_readlink,[for Perl's location],[Set to the path of the Perl interpreter we're using.])

## If compiling with --enabled-shared, then JIT bitcode files go into .libs, so
## that they'll be in the same directory as their executable.
BCDIR="."
if [[ $enable_shared = yes ]]; then
	 BCDIR=".libs"
fi
AC_SUBST(BCDIR)


# If we're on an AMD64 machine, then we must use the -fPIC flag explicitly,
# since we want to be able to use static libraries when creating shared-object
# models.  Libtool only specifies -fPIC for shared objects, so we have to add it
# here in case the user builds with --disable-shared.  If shared libraries are
# not disabled, then we only need it for when creating a shared object from an
# ADL file.  This is only added in for AMD64.  Note that on this platform, you
# don't take a performance hit using it, as you do with IA32.
EXTRA_CFLAGS=
ADL_SO_CFLAGS=

AC_MSG_CHECKING([if -fPIC is required])
case $host_cpu in 
  x86_64)
    ADL_SO_CFLAGS="${ADL_SO_CFLAGS} -fPIC"
	if [[ $enable_shared = no ]]; then
	  EXTRA_CFLAGS="${EXTRA_CFLAGS} -fPIC"
          AC_MSG_RESULT([yes (for libraries and adl-config)])
	else
          AC_MSG_RESULT([yes (only for adl-config)])
	fi
	;;
  *)
	AC_MSG_RESULT([no])
	;;
esac
AC_SUBST(EXTRA_CFLAGS)
AC_SUBST(ADL_SO_CFLAGS)

# We only support the JIT if we have LLVM.
if [[ $LLVM_EXISTS = true ]]; then
  ADL_JIT="--jit"
fi

AC_MSG_CHECKING([if the JIT can be supported])
if [[ x$ADL_JIT = x ]]; then
  JIT_OK=0
  AC_MSG_RESULT([no])
else
  JIT_OK=1
  AC_MSG_RESULT([yes])
fi

AC_SUBST(ADL_JIT)
AC_SUBST(JIT_CCFLAGS)
AC_SUBST(JIT_OK)

# This will set WORDS_BIGENDIAN if we're on a big-endian platform.
AC_C_BIGENDIAN

# Are we creating a distribution for an external release?
AC_MSG_CHECKING([if the user wants an external distribution release])
AC_ARG_ENABLE(external-release,AS_HELP_STRING([--enable-external-release],[Prepare this distribution for an external (outside of Freescale) release]),
[case "${enableval}" in
  yes) 
	external_release=true
	;;
  no)  
	external_release=false
	;;
  *) 
	AC_MSG_ERROR(bad value ${enableval} for --enable-external-release)
	;;
esac],[external_release=false])

if [[ $external_release = true ]] ; then
	AC_MSG_RESULT([yes]) 
else
	AC_MSG_RESULT([no]) 
fi

EXTERNAL_RELEASE_FLAG=external-release-distribution

# This conditional is the switch used to differentiate between creating vs. not
# creating an external distribution.
AM_CONDITIONAL(CREATE_EXTERNAL_RELEASE, test x$external_release = xtrue -o -e ${srcdir}/designs/${EXTERNAL_RELEASE_FLAG} )

# This conditional is the switch used to tell whether we are in an external
# distribution vs. not being in one.
AM_CONDITIONAL(IS_EXTERNAL_RELEASE, test -e ${srcdir}/designs/${EXTERNAL_RELEASE_FLAG} )
AC_SUBST(EXTERNAL_RELEASE_FLAG)

# Check to see if we want to run the GDB regressions.  They can be kinda flaky
# for some reasons- we get timeouts on the sockets which can screw up the
# regressions.  Therefore, I recommend that we don't run these for the nightly
# regressions.  By default, it's on, so that the usual testing done in a user's
# working copy does run them.
AC_MSG_CHECKING([if we want to run gdb regressions])
AC_ARG_ENABLE(gdb-tests,AS_HELP_STRING([--enable-gdb-tests],[Enable or disable GDB regression tests.]),
[case "${enableval}" in
			yes)
			GDB_TESTS=1
			;;
			no)
			GDB_TESTS=0
			;;
			*)
			AC_MSG_ERROR(bad value ${enableval} for --enable-gdb-tests)
			;;
esac],GDB_TESTS=1)
if [[ $GDB_TESTS = 1 ]]; then
	 AC_MSG_RESULT([yes])
else
	 AC_MSG_RESULT([no])
fi
AC_SUBST(GDB_TESTS)


# If we're using a newer version of tar, then we have to tweak the options so
# that tar doesn't use V7 mode, which has a problem with the use of big UIDs.
# Unfortunately, newer verions of tar automatically switch into v7 mode if they
# see the -o option being used, which is automake's default.  We have to use the
# --no-same-user option, instead, which is the same flag, just the long-form.
# This doesn't cause the switch.
AC_MSG_CHECKING([if uid conversion will be done by tar])
tar_version=$(tar --version | grep tar | awk '{print $[4]}')
DPP_VERSION_CHECK(${tar_version},1.15,uid_conversion=no,uid_conversion=yes)
if [[ $uid_conversion = no ]]; then
	am__tar=$(echo $am__tar | sed -e 's/chof -/ch --no-same-owner -f -/')
	AC_MSG_RESULT([no.  New tar invocation:  ${am__tar}])
else
	AC_MSG_RESULT([yes.  No change required to tar arguments.])
fi

## For binutils, we enable optimization if the top-level project is optimized.
## We also add in -fpic so that we can use the libraries within a shared object.
## If we're on Cygwin, though, this causes a warning, which gets promoted to an
## error, so we don't use that flag.
BU_FLAGS="-g"
if [[ x$OPT_ENABLED = xyes ]] ; then
  BU_FLAGS="${BU_FLAGS} -O2"
fi

## If we've disabled shared mode, then we have to export our symbols or else
## plugins won't work.  We do this by communicating with adl-config.
so_support=true
if [[ $enable_shared = no ]]; then
	 so_support=false
fi
AC_SUBST(so_support)


##
## This here's a really specific issue...  With 32-bit gcc 4.7.4, there seems to
## be a bug with the loop tree vectorizer that can cause segfaults in our code.
## So, we disable that option for that situation.
##
AC_MSG_CHECKING([if we should disable gcc's tree vectorizer option])
disable_tree_vectorize=no
if [[ x$OPT_ENABLED = xyes -a x${GCC_VERSION} = "x4.7.4" ]]; then
	 #echo "(opt is on and version is correct)"
	 #echo "machine is $(${CXXPROG} -dumpmachine)"
	 case $(${CXXPROG} -dumpmachine) in
	 			i686*)
				disable_tree_vectorize=yes
				;;
	 esac
fi
if [[ x$disable_tree_vectorize = xyes ]]; then
  CPPSETUP="${CPPSETUP} -fno-tree-vectorize"
	AC_MSG_RESULT([yes])
else
	AC_MSG_RESULT([no])
fi

# Find the Boost library.  Bit of a hack here: If the user hasn't specified an
# override, and the version we need exists in /pkg, then force --with-boost to
# use that path.
platarg=
BoostPrefix=
BoostDefaultPath=
BoostReqVersion="1.54.0"
AC_ARG_VAR([BOOST_VERSION],    [Specify a Boost version using the default /pkg search mechanism.])
if [[ -z ${BOOST_FULL_VERSION} ]]; then
	 if [[ -z ${BOOST_VERSION} ]]; then
	 		BOOST_VERSION="${BoostReqVersion}"
	 fi
	 case $CXXPROG in
		 *i686-linux*)
		 BoostPath1=/pkg/OSS-boost-/${BOOST_VERSION}-OSS-gcc_$($CXXPROG -dumpversion)
		 BoostPath2=/pkg/boost-/${BOOST_VERSION}-gcc_$($CXXPROG -dumpversion)
		 platarg="-32"
		 CheckerType="32"
		 ;;
		 *)
		 BoostPath1=/pkg/OSS-boost-/${BOOST_VERSION}-OSS-gcc_$($CXXPROG -dumpversion)
		 BoostPath2=/pkg/boost-/${BOOST_VERSION}-gcc_$($CXXPROG -dumpversion)
		 CheckerType="64"
		 ;;
	 esac
else
	 case $CXXPROG in
		 *i686-linux*)
		 BoostPath1=/pkg/OSS-boost-/${BOOST_FULL_VERSION}
		 BoostPath2=/pkg/boost-/${BOOST_FULL_VERSION}
		 platarg="-32"
		 CheckerType="32"
		 ;;
		 *)
		 BoostPath1=/pkg/OSS-boost-/${BOOST_FULL_VERSION}
		 BoostPath2=/pkg/boost-/${BOOST_FULL_VERSION}
		 CheckerType="64"
		 ;;
	 esac
fi

if [[ -d ${BoostPath1} ]]; then
  BoostDefaultPath=${BoostPath1}
elif [[ -d ${BoostPath2} ]]; then
  BoostDefaultPath=${BoostPath2}
fi

# We have to see if Boost is installed using a full platform name or just the
# shortened name.  If neither exists, then skip this and use the system default.
platprog=/run/pkg/foundation-/frame/bin/platform
if [[ ! -z ${BoostDefaultPath} ]]; then
	fullplat=$(${platprog} -f ${platarg})
	shortplat=$(${platprog} ${platarg})
  if [[ -d ${BoostDefaultPath}/${fullplat} ]]; then
		 BoostDefaultPath=${BoostDefaultPath}/${fullplat}
	elif [[ -d ${BoostDefaultPath}/${shortplat} ]]; then
		 BoostDefaultPath=${BoostDefaultPath}/${shortplat}
	else
		 BoostDefaultPath=
	fi
fi

# Only look automatically in /pkg if no --with-boost or BOOST_ROOT was
# specified.
if [[ -z ${with_boost} ]]; then
	 if [[ -z ${BOOST_ROOT} ]]; then
	 		if [[ -d ${BoostDefaultPath} ]]; then
	 	 		 with_boost=${BoostDefaultPath}
	 	 		 echo "Using default Boost path ${with_boost}"
	 		fi
	 fi
fi

boost=no
AX_BOOST_BASE([${BoostReqVersion}],[boost=yes])
if [[ $boost = no ]]; then
	 AC_MSG_ERROR([Was not able to find the proper version of the Boost library (${BoostVersion})])
fi


## Compiler options for compiling the instruction handlers when creating a JIT model with clang.
JITFLAGS="${RNUMBER_CFLAGS} ${BOOST_CPPFLAGS}"
AC_SUBST(JITFLAGS)

## If this is Cygwin, then we must tell it to not define addr_t, since we use
## that type ourselves.
case ${build_os} in
cygwin*)
  DYNEXPORT="--export-all-symbols"
  EXTRA_CFLAGS="${EXTRA_CFLAGS} -D__addr_t_defined"
  JitFlags="${JitFlags} -D__addr_t_defined"
  ;;
*)
  BU_FLAGS="${BU_FLAGS} -fpic"
  DYNEXPORT="-export-dynamic"
  ;;
esac

AC_SUBST(DYNEXPORT)


## Find our source directory and export this so that Perl modules and such can
## be found.
adl_top_srcdir=$(cd ${srcdir} && pwd)
AC_SUBST(adl_top_srcdir)

## Just to make sure that the GC works nicely with threads.
GC_FLAGS="-D_REENTRANT -DGC_LINUX_THREADS"

## Do final substitions and generation.
CXXFLAGS=" --std=c++0x -ggdb -g3 -Wall -Wno-deprecated -Wno-unused-label -Wno-unused-but-set-variable -Wno-unused-local-typedefs ${GC_FLAGS} ${CPPSETUP} ${BOOST_CPPFLAGS} ${EXTRA_CFLAGS} ${user_cxxflags}"

# This is required in configure.ac.  It generates the output files.
AC_CONFIG_FILES([\
	Makefile \
	designs/Makefile \
	doc/Makefile \
	external/Makefile \
	tests/fe/Makefile \
	tests/iss/Makefile \
	tests/cli/Makefile \
	tests/scripts/Makefile \
	tests/docs/Makefile \
	tests/graphite/Makefile \
	tests/intbv/Makefile \
	tests/bits/Makefile \
	tests/designs/Makefile \
	tests/uadl-fe/Makefile \
	tests/uadl/Makefile \
	tests/adlapi2simapi/Makefile \
])

#if [[ ! -e ${srcdir}/designs/${EXTERNAL_RELEASE_FLAG} ] && [ $external_release = false ]] ; then
#	AC_CONFIG_FILES([designs/scbtb/Makefile]) 
#fi

AC_CONFIG_FILES([\
	scripts/adl-config \
],[chmod +x,-w scripts/adl-config])

AC_CONFIG_FILES([\
	scripts/adl_config.pm \
],[chmod -w scripts/adl_config.pm])

AC_CONFIG_FILES([\
	scripts/usagelog.pm \
],[chmod +x,-w scripts/usagelog.pm ])

AC_CONFIG_FILES([\
	scripts/adl2iss \
],[chmod +x,-w scripts/adl2iss ])

AC_CONFIG_FILES([\
	scripts/adl2asm \
],[chmod +x,-w scripts/adl2asm ])

AC_CONFIG_FILES([\
	scripts/adl2doc \
],[chmod +x,-w scripts/adl2doc ])

AC_CONFIG_FILES([\
	scripts/uadl2model \
],[chmod +x,-w scripts/uadl2model])

# AC_CONFIG_FILES([\
# 	scripts/adl-db \
# ],[chmod +x,-w scripts/adl-db])

AC_CONFIG_FILES([\
	scripts/xhtml2pdf \
],[chmod +x,-w scripts/xhtml2pdf ])

# For our own version of binutils, we're targeting PowerPC.
if [[ $minimal_build = false -a $lang_frontend = true ]]; then
	# We're not using ac_config_subdirs here so that we can set a different
	# target and use a cache file.
	echo "Configuring included binutils package."
	binbuild="external/binutils-adl"
	if [[ ! -d ${binbuild} ]] ; then
	  echo "Creating binutils build directory:  ${binbuild}"
	  mkdir -p ${binbuild}
	fi
	binsrc=$(cd ${srcdir}/${binbuild} && echo $(pwd))
	if [[ x$USE_MAINTAINER_MODE = xno ]]; then
		 maintainer_flag=""
	else
		maintainer_flag="--enable-maintainer-mode"
	fi
	# Note:  We're forcing in the -fpic option so that we can use these libraries with shared objects.
	(cd ${binbuild} && ${binsrc}/configure CFLAGS="${BU_FLAGS}" CXXFLAGS="${BU_FLAGS}" --target=ppc-elf64 --cache-file=config-elf.cache ${maintainer_flag})
	echo "Finished configuring included binutils packet."
fi

AC_OUTPUT
