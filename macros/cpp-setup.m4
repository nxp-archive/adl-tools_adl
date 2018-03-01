## AM_CPP_SETUP
##
## This performs a variety of checks related to C++ usage and sets various
## variables accordingly.  The idea is that you include the variable CPPSETUP
## when defining CXXFLAGS.
##
## If the --disable-opt option is set, we disable optimization.  Otherwise,
## it's on by default.
##
## If --enable-profile is set, PROFILE is set to a flag.
## 
## If --enable-efence is set, we set EFENCE to the location of the library
## and substitute that variable.  The user can specify a location of Electric Fence
## by supplying a value for this option.
##
## Profile-guided optimization:
##
##   --enable-pgo-1:  Enable the first pass of profile-guided optimization.
##
##   --enable-pgo-2:  Enable the first pass of profile-guided optimization.  This
##                    uses .gcda files created from running code compiled using the
##                    first pass.
##
## We also define, and substitute, the variable RFLAG, which is set to something
## that should act as an option to encode a path for library lookup.  You can use
## this in a config file, e.g. "@RFLAG@/foo/bar".
##
## We also set and substitute OPT_ENABLED (set to yes or no) to indicate whether
## optimization was enabled.
##
## AC_DEFUN([MAJOR],[MINOR])
##
## If major and minor are ommitted, no version checking is performed.  If included,
## GCC must have at least this version number in order to pass.
##
AC_DEFUN([AM_CPP_SETUP],
[

RFLAG="-Wl,-R"
AC_SUBST(RFLAG)
OPT_ENABLED=no

AC_ARG_ENABLE(opt,AC_HELP_STRING([--enable-opt],[Enable optimization flag (-O3), disables debug]),
 if [[[ x${enableval} != xno ]]] ; then
   OPT="-O3"
   OPT_ENABLED=yes
   AC_MSG_RESULT([Optimizing with -O3])	
 else
   OPT=""
   AC_MSG_RESULT([Optimization disabled.])	
 fi
 ,
 OPT_ENABLED=yes
 OPT="-O3"
 AC_MSG_RESULT([Optimizing with -O3 by default.]),
)

AC_SUBST(OPT_ENABLED)

PROFILE=
AC_ARG_ENABLE(profile,AC_HELP_STRING([--enable-profile],[Enable profiling]),
 PROFILE="-pg"
 AC_MSG_RESULT([Profiling with -pg])
)

PGO=
AC_ARG_ENABLE(pgo-1,AC_HELP_STRING([--enable-pgo-1],[Enable the first pass of profile-guided optimization.]),
 PGO="-fprofile-arcs"
 AC_MSG_RESULT([Enabling first-pass of profile-guided optimization.])
)

AC_ARG_ENABLE(pgo-2,AC_HELP_STRING([--enable-pgo-2],[Enable the second pass of profile-guided optimization.]),
 PGO="-fbranch-probabilities"
 AC_MSG_RESULT([Enabling second-pass of profile-guided optimization.])
)

default_efence="-lefence"

AC_ARG_ENABLE(efence,
[  --enable-efence=<location>    link test programs with electric fence. If no location
                                 is given, /nfs/ibmoto/tools/tlv4/raptor/lib/obj.SunOS/libefence
                                 is used],
 if test x$enableval = x; then
   EFENCE=$default_efence
 else
   EFENCE=$enableval
 fi
 AC_MSG_RESULT(linking with electric fence - $EFENCE),
 EFENCE=""
 AC_MSG_RESULT(efence will not be used)
)
AC_SUBST(EFENCE)

##
## Include this flag in CXXFLAGS.
##

CPPSETUP="$OPT $PROFILE $PGO"

##
## Remember GCC major, minor, and micro versions. Sometimes
## micro is missing. In those cases, set micro to 0.
##
GCC_VERSION=`$CC -dumpversion`
## Note the use of quadrigraphs below to get autoconf to put $1 and $2
## into the awk script.  Refer to the autoconf manual for more information.
GCC_MAJOR=`echo $GCC_VERSION | awk -F. ' { print @S|@1; }'`
GCC_MINOR=`echo $GCC_VERSION | awk -F. ' { print @S|@2; }'`
AC_MSG_RESULT([GCC Version:  $GCC_MAJOR.$GCC_MINOR])

REQUIRED_MAJOR=$1
REQUIRED_MINOR=$2

## Do version checking.
AC_MSG_CHECKING([that gcc has the required version ($REQUIRED_MAJOR.$REQUIRED_MINOR)])
if [[ x$REQUIRED_MAJOR != x ]]; then
  if [[ $GCC_MAJOR -lt $REQUIRED_MAJOR ]]; then
    AC_MSG_ERROR([GCC must have a minimum major version of $REQUIRED_MAJOR.])
  fi
  if [[ x$REQUIRED_MINOR != x -a $GCC_MAJOR -eq $REQUIRED_MAJOR ]]; then
    if [[ $GCC_MINOR -lt $REQUIRED_MINOR ]]; then
      AC_MSG_ERROR([GCC must have a minimum minor version of $REQUIRED_MINOR.])
    fi
  fi
fi
AC_MSG_RESULT([okay])

AC_SUBST(GCC_VERSION)
AC_SUBST(GCC_MAJOR)
AC_SUBST(GCC_MINOR)

])
