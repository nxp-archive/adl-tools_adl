##
## AM_SYSTEMC([version], [, action-if-found [,action-if-not-found]])
##   Configure a program to use SystemC.
##
##   version:  Minimum require version.
##   action-if-found:  Executed if SystemC is found.
##   action-if-not-found:  Executed if SystemC is not found.
##
## Environment variables
##   SYSTEMC_INCLUDE_DIR:  Let the user specify the SystemC include directory.
##   SYSTEMC_LIB_DIR    :  Let the user specify the SystemC lib directory.
##
## Output:
##
##   SYSTEMC_CFLAGS :  C++ flags for compiling SystemC
##   SYSTEMC_LIBS   :  Linker options for linking with SystemC
##   SYSTEMC_EXISTS :  true | false
##
AC_DEFUN([AM_SYSTEMC],
[
  AC_ARG_VAR([SYSTEMC_INCLUDE_DIR],[Specify the SystemC include directory.  If not specified, then any SystemC related programs
									and/or regression tests will not be built.  Note:  If only specifying this variable for a
									single configure run (as opposed to a permanently set flag in your environment), then set
									it as a configure option, e.g. ./configure SYSTEMC_INCLUDE_DIR=... as opposesd to before the
									./configure command.])
  AC_ARG_VAR([SYSTEMC_LIB_DIR],    [Specify the SystemC library directory.  See SYSTEMC_INCLUDE_DIR for information about specifying
									this variable.])

  SyscExists=yes
  if [[ ! -f ${SYSTEMC_INCLUDE_DIR}/systemc.h ]] ; then
    AC_MSG_WARN([Could not find the SystemC header file.  Specify this with the SYSTEMC_INCLUDE_DIR environment variable.])
	SyscExists=no
  fi

  if [[ ! -d ${SYSTEMC_LIB_DIR} ]] ; then
    AC_MSG_WARN([Could not find the SystemC library directory.  Specify this with the SYSTEMC_INCLUDE_DIR environment variable.])
	SyscExists=no
  fi

  if [[ $SyscExists != "no" ]] ; then
    AC_MSG_CHECKING([that we can compile a SystemC program.])

  	AC_LANG_PUSH([C++])
		ac_cxxflags_orig="$CXXFLAGS"
		ac_ldflags_orig="$LDFLAGS"
		ac_libs_orig="$LIBS"

		SYSTEMC_CFLAGS="-I$SYSTEMC_INCLUDE_DIR"
		systemc_ldflags="-L$SYSTEMC_LIB_DIR -Wl,-R$SYSTEMC_LIB_DIR"
		systemc_libs="-lsystemc -lpthread"
		SYSTEMC_LIBS="${systemc_ldflags} ${systemc_libs}"

		CXXFLAGS="${CXXFLAGS} ${SYSTEMC_CFLAGS} -O0"
		LDFLAGS="${LDFLAGS} ${systemc_ldflags}"
		LIBS="${LIBS} ${systemc_libs}"
	
    AC_RUN_IFELSE([AC_LANG_SOURCE([

			#include <systemc.h>
			#define main _main
    
	    SC_MODULE(foo) { SC_CTOR(foo) {} } bar("bar");
			int sc_main(int argc, char *argv[[]]) { return 0; }
	    ])],
		SYSC_VERSION=`./conftest${ac_exeext} 2>&1`
		AC_MSG_RESULT([ok.]),
		AC_MSG_RESULT([no.])
		SyscExists=no
		)

		# If the program compiled, then run it to extract the version number.
		if [[ $SyscExists != "no" ]] ; then	
			 ## Note the use of quadrigraphs below to get autoconf to put $1 and $2
			 ## into the awk script.  Refer to the autoconf manual for more information.
			 ## Note:  We remove the micro version b/c it's sometimes not a simple numerical value, e.g. it contains a date.
			 SYSC_VERSION=`echo ${SYSC_VERSION} | awk '/SystemC/ { print @S|@2; }' | awk -F. '{ printf "%s.%s.0\n" , @S|@1 , @S|@2; }'`

			 #echo "SystemC Version:  ${SYSC_VERSION}"

			 if [[ -z "$SYSC_VERSION" ]]; then
			 	 SyscExists=no
			 else
			   AC_MSG_CHECKING([for SystemC's version ($1 required).])
    	 	 DPP_VERSION_CHECK([$SYSC_VERSION], [$1], , [SyscExists=no])
    		 if [[ $SyscExists = "no" ]] ; then	
      	   AC_MSG_RESULT([Bad:  Found $SYSC_VERSION but $1 is the minimum required.  Use the SYSTEMC_INCLUDE_DIR AND SYSTEMC_LIB_DIR environment variables to specify an alternative SystemC version.])
    		 else
       	   AC_MSG_RESULT([$SYSC_VERSION (okay).])
    		 fi
			 fi
		fi

		CXXFLAGS=$ac_cxxflags_orig
		LDFLAGS=$ac_ldflags_orig
		LIBS=$ac_libs_orig

		AC_LANG_POP
  fi

  if [[ $SyscExists = yes ]]; then
		SYSTEMC_EXISTS=true
    ifelse([$2], , :, [$2])
  else
		SYSTEMC_EXISTS=false
    ifelse([$3], , :, [$3])
  fi

  AC_SUBST(SYSTEMC_CFLAGS)
  AC_SUBST(SYSTEMC_LIBS)
  AC_SUBST(SYSTEMC_EXISTS)
])
