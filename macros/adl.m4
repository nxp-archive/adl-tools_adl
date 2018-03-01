##
## AM_ADL([version], [, action-if-found [,action-if-not-found]])
##   Configure a program to use ADL.
##
##   version:              Minimum require version.
##   action-if-found:      Executed if ADL is found.
##   action-if-not-found:  Executed if ADL is not found.
##
##
## Environment Variables:
##   AdlConfigFlags:  Set this to any extra flags which should be included in the adl-config calls.
##
## Command-line options:
##   --with-adl to let a user specify the location of the adl-config program.
##              This is the complete path to the program's location, e.g. /usr/foobar/bin.
##
## Output:
##   ADL_PREFIX:    The ADL install prefix.
##   ADL_CFLAGS:    ADL cflags.
##   ADL_LIBS:      ADL link line.
##   ADL_LTLIBS:    ADL link line for libtool.
##   ADL_LIB_PATHS: ADL library paths.
##   ADL_CXX:       Path to C++ compiler used by this version of ADL.
##
AC_DEFUN([AM_ADL],
[
  AdlConfig=
  AdlExists=yes

  AC_ARG_VAR([AdlConfigFlags],[Specify any flags which should be passed to the adl-config ADL configuration program.])

  # If the user has specified a path to use, then use that path, otherwise use the user's PATH.
  AC_ARG_WITH([adl],AC_HELP_STRING([--with-adl],[specify the path containing the adl-config program.]),ac_adl="${withval}")
  if [[ x$ac_adl = x ]] ; then
    ac_adl=$PATH
  fi

  # Now try to find the configuration program using the path (either user-specified or the user's PATH).
  AC_PATH_PROG(AdlConfig, [adl-config], no, $ac_adl)
  if [[ "$AdlConfig" = no ]] ; then
    AC_MSG_WARN([Could not find adl-config in the path.  Please modify your path or use the --with-adl option.])
    AdlExists=no
  fi

  # If found, then try and run it as a sanity check.
  if [[ $AdlExists != no ]] ; then
    AC_MSG_CHECKING([that we can run adl-config])
    AdlTest="${AdlConfig} --prefix"
    ${AdlTest} > /dev/null
    if [[ $? = 0 ]]; then
      AC_MSG_RESULT([yes])
    else
      AC_MSG_WARN([no.  Please modify your path or use the --with-adl option.])
      AdlExists=no
    fi
  fi

  # If we found it and it looks good, then collect up our information.
  if [[ $AdlExists != no ]] ; then
    ADL_PREFIX=`$AdlConfig ${AdlConfigFlags} --prefix`
    ADL_CFLAGS=`$AdlConfig ${AdlConfigFlags} --cflags`
    ADL_LIBS=`$AdlConfig ${AdlConfigFlags} --libs`
    ADL_LTLIBS=`$AdlConfig ${AdlConfigFlags} --ltlibs`
    ADL_LIB_PATHS=`$AdlConfig ${AdlConfigFlags} --lib-paths`
    ADL_VERSION=`$AdlConfig ${AdlConfigFlags} --version`
	ADL_CXX=`$AdlConfig ${AdlConfigFlags} --cxxpath`
	# The cxxpath option might not be supported, so use the default if there's an error.
	if [[ $? != 0 ]]; then
		ADL_CXX=${CXX}
	fi

    # Minimum version checking.
    AC_MSG_CHECKING([for adl's version ($1 required).])
    DPP_VERSION_CHECK([$ADL_VERSION], [$1], , [AdlExists=no])

    if [[ $AdlExists = no ]] ; then	
      AC_MSG_WARN([Bad:  Found $ADL_VERSION but $1 is the minimum required.])
    else
      AC_MSG_RESULT([$ADL_VERSION (okay).])
    fi

  fi

  # If found, execute the user's action-if-found codde, otherwise execute the user's action-if-not-found code.
  if [[ $AdlExists = yes ]]; then
    ifelse([$2], , :, [$2])
  else
    ifelse([$3], , :, [$3])
  fi

  # These will be accessible within any autoconf-generated file, such as Makefiles.
  AC_SUBST(ADL_PREFIX)
  AC_SUBST(ADL_CFLAGS)
  AC_SUBST(ADL_LIBS)
  AC_SUBST(ADL_LTLIBS)
  AC_SUBST(ADL_LIB_PATHS)
  AC_SUBST(ADL_CXX)
])
