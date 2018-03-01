##
## AM_PROG_SWIG([version], [, action-if-found [,action-if-not-found]])
##   Configure a program to swig
##
##   version:  Minimum require version.
##   action-if-found:  Executed if swig is found.
##   action-if-not-found:  Executed if swig is not found.
##
## Command-line options:
##   --with-swig to let a user specify the location of the swig program.
##
## Output:
##
##  SWIG_LIBS_PATH
##  SWIG_VERSION
##

AC_DEFUN([AM_PROG_SWIG], 
[
   AC_ARG_WITH([swig],AC_HELP_STRING([--with-swig],[specify a path to use to find the swig program.]),ac_swig=$withval)
   if [[ x$ac_swig = x ]] ; then
     ac_swig=$PATH
   fi

   #-- check for swig in PATH
   AC_PATH_PROG([SWIG], [swig], [no])

   SwigExists=yes
   if [(test x"$SWIG" == x"no") && (test x"$1" != x"0")]; then
        AC_MSG_ERROR([swig not found in the path. Please modify your path or use the --with-swig option.])
	SwigExists=no
   fi

   #-- check vor swig version
   AC_MSG_CHECKING([swig version])
   full_version=`swig -version | grep "SWIG" | awk ' { print @S|@3; }'`
   SWIG_MAJOR=`echo $full_version | awk -F. ' { print @S|@1; } '`
   SWIG_MINOR=`echo $full_version | awk -F. ' { print @S|@2; } '`
   SWIG_MACRO=`echo $full_version | awk -F. ' { print @S|@3; } '`
   version_majmin="$SWIG_MAJOR.$SWIG_MINOR"
   AC_MSG_RESULT([Swig Version: $version_majmin.$SWIG_MACRO])


   SWIG_REQ_MAJOR=`echo $1 | awk -F. ' { print @S|@1; } '`
   SWIG_REQ_MINOR=`echo $1 | awk -F. ' { print @S|@2; } '`
   SWIG_REQ_MACRO=`echo $1 | awk -F. ' { print @S|@3; } '`

   REQUIRED_MAJMIN="$SWIG_REQ_MAJOR.$SWIG_REQ_MINOR"
   # Do version checking
   AC_MSG_CHECKING([that swig has the required version ($REQUIRED_MAJMIN.$SWIG_REQ_MACRO)])

   #-- compare swig version with min-version
   if [(test x"$1" != x"")  && (test "${REQUIRED_MAJMIN/./}" -gt "${version_majmin/./}")]; then 
        AC_MSG_ERROR([swig version $1 required])
   else
        if [(test "${REQUIRED_MAJMIN/./}" -eq "${version_majmin/./}") && (test "${SWIG_REQ_MACRO}" -gt "${SWIG_MACRO}")]; then
            AC_MSG_ERROR([swig version $1 required])
        fi
   fi

   AC_MSG_RESULT([okay])

   #-- Fill in the include paths and the library paths
   if [[ $SwigExists != "no" ]] ; then
        SWIG_LIB_PATH=`swig -swiglib`
        SWIG_VERSION=$version
   fi

   
   AC_SUBST(SWIG_LIB_PATH)
   AC_SUBST(SWIG_VERSION)

])dnl