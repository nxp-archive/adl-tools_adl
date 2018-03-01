##
## AM_PLASMA([version], [, action-if-found [,action-if-not-found]])
##   Configure a program to use plasma.
##
##   version:              Minimum require version.
##   compile:              If set to "false", then a test program will not be compiled.
##   action-if-found:      Executed if plasma is found.
##   action-if-not-found:  Executed if plasma is not found.
##
## Command-line options:
##   --with-plasma to let a user specify the location of the plasma program.
##                 This is the complete path to the program's location, e.g. /usr/foobar/bin.
##
## Output:
##   OCC:             OpenC++ executable.
##   PLASMA:          The Plasma executable.
##   PLASMA_PATH:     The Plasma install prefix.
##   PLASMA_CFLAGS:   Plasma cflags.
##   PLASMA_LIBS:     Plasma link line.
##   PLASMA_LTLIBS:   Plasma link line for libtool.
##   PLASMA_LIB_PATHS: Plasma library paths.
##
AC_DEFUN([AM_PLASMA],
[
  AC_ARG_WITH([plasma],AC_HELP_STRING([--with-plasma],[specify a path to use to find the Plasma program.]),ac_plasma=$withval)
  if [[ x$ac_plasma = x ]] ; then
    ac_plasma=$PATH
  fi
  AC_PATH_PROG(PlasmaConfig, [plasma-config], no, $ac_plasma)
  AC_PATH_PROG(PLASMA, [plasma], no, $ac_plasma)
  AC_PATH_PROG(OCC, [occ], no, $ac_plasma)

  PlasmaExists=yes
  if [[ $PLASMA = "no" ]] ; then
    AC_MSG_WARN([Could not find plasma in the path.  Please modify your path or use the --with-plasma option.])
	  PlasmaExists=no
  fi
  if [[ $PlasmaConfig = "no" ]] ; then
    AC_MSG_WARN([Could not find plasma-config in the path.  Please modify your path or use the --with-plasma option.])
	  PlasmaExists=no
  fi

  if [[ $PlasmaExists != "no" ]] ; then
    PLASMA_PATH=`$PlasmaConfig --prefix`
    PLASMA_CFLAGS=`$PlasmaConfig --cflags`
    PLASMA_LIBS=`$PlasmaConfig --libs`
    PLASMA_LTLIBS=`$PlasmaConfig --ltlibs`
	  PLASMA_LIB_PATHS=`$PlasmaConfig --lib-paths`
    PLASMA_VERSION=`$PlasmaConfig --version`

    AC_MSG_CHECKING([for plasma's version ($1 required).])
    DPP_VERSION_CHECK([$PLASMA_VERSION], [$1], , [PlasmaExists=no])

    if [[ $PlasmaExists = "no" ]] ; then	
      AC_MSG_RESULT([Found $PLASMA_VERSION but $1 is the minimum required.])
    else
      AC_MSG_RESULT([$PLASMA_VERSION (okay).])
    fi

  fi

  ShouldCompile="$2"
  if [[ $PlasmaExists != "no" -a x$ShouldCompile != "xfalse" ]] ; then
    AC_MSG_CHECKING([that we can compile and link plasma program.])

		CXXFLAGS_orig=$CXXFLAGS
		LDFLAGS_orig=$LDFLAGS
		LIBS_orig=$LIBS

		CXXFLAGS="${PLASMA_CFLAGS}"
		LDFLAGS="-L${PLASMA_PATH}/lib -Wl,-R${PLASMA_PATH}/lib"
		LIBS="-locc -lgc"

    AC_LINK_IFELSE([AC_LANG_SOURCE([
#include <iostream>
#include <iomanip>

#include "opencxx/mop2.h"

using namespace std;
using namespace Opencxx;
using namespace PtreeUtil;

class ParseTest : public Opencxx::Class {
public:
  static bool Initialize()
  {
    SetMetaclassForPlain("ParseTest");
    cout.setf(ios_base::boolalpha);
    
    return true;
  }

  const char* MetaclassName() { return "ParseTest"; }

};

static Class* parse_test_setup(Ptree* def, Ptree* marg)
{
  Class* c = new ParseTest;
  c->InitializeInstance(def, marg);
  return c;
}

static MetaclassRegistration _parse_test_setup_var("ParseTest", parse_test_setup, 
                                                   ParseTest::Initialize, 0);
])],AC_MSG_RESULT([ok.]),PlasmaExists=no)

	CXXFLAGS=$CXXFLAGS_orig
	LDFLAGS=$LDFLAGS_orig
	LIBS=$LIBS_orig

	if [[ $PlasmaExists = "no" ]] ; then
      AC_MSG_WARN([failed.])
    fi

  fi

  if [[ $PlasmaExists = yes ]]; then
    ifelse([$3], , :, [$3])
  else
    ifelse([$4], , :, [$4])
  fi

  AC_SUBST(OCC)
  AC_SUBST(PLASMA)
  AC_SUBST(PLASMA_PATH)
  AC_SUBST(PLASMA_CFLAGS)
  AC_SUBST(PLASMA_LIBS)
  AC_SUBST(PLASMA_LTLIBS)
  AC_SUBST(PLASMA_LIB_PATHS)
])
