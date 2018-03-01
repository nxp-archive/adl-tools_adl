##
## AM_PROG_TCL([version], [, action-if-found [,action-if-not-found]])
##   Configure a program to tcl
##
##   version:  Minimum require version.
##   action-if-found:  Executed if tcl is found.
##   action-if-not-found:  Executed if tcl is not found.
##
## Command-line options:
##   --with-tcl to let a user specify the location of the tcl program.
##
## Output:
##
##  TCLSH
##  TCL_LIBS_PATHS
##  TCL_LIBS
##  TCL_STATIC_LIBS
##  TCL_VERSION
##  TCL_INC_PATHS
##

AC_DEFUN([AM_PROG_TCL], 
[
   AC_ARG_WITH([tcl],AC_HELP_STRING([--with-tcl],[specify a path to use to find the tcl program.]),ac_tcl=$withval)
   if [[ x$ac_tcl = x ]] ; then
     ac_tcl=$PATH
   fi

   AC_ARG_WITH([tclinc],AC_HELP_STRING([--with-tclinc],[specify a single path to use to find the tcl include files.]),ac_tclinc=$withval)

   #-- check for swig in PATH
   AC_PATH_PROG([SWIG], [swig], [no])

   #-- check for tclsh in PATH
   AC_PATH_PROG([TCLSH], [tclsh], [no], [$ac_tcl])

   #-- tclsh modifies its auto_path, based upon where it's run.  Therefore, if it's a link, run it from its original location.
   if [[ -L $TCLSH ]] ; then
     TCLSH=$($READLINK $TCLSH)
		 if [[ $TCLSH = "wrapper" ]]; then
		 		TCLSH=$(wrapper --which=tclsh)
		 elif [[ $TCLSH = "tr.dynamic" ]]; then
		 		TCLSH=$(tr.dynamic --which=tclsh)
		 fi
   fi   

   TclExists=yes
   if [(test x"$TCLSH" == x"no") && (test x"$1" != x"0")]; then
        AC_MSG_ERROR([tclsh not found in the path. Please modify your path or use the --with-tcl option.])
				TclExists=no
   fi

   #-- check for wish in PATH
   AC_PATH_PROG([WISH], [wish], [no], [$ac_tcl])

   #-- check vor tcl version
   AC_MSG_CHECKING([tcl version])
   version=`echo "puts [[set tcl_version]]" | $TCLSH -`
   AC_MSG_RESULT([$version])

   #-- compare tcl version with min-version
   required=$1
   if [(test x"$1" != x"") && (test "${required/./}" -gt "${version/./}")]; then
        AC_MSG_ERROR([tcl version $1 required])
   fi

   #-- Fill in the include paths and the library paths
   if [[ $TclExists != "no" ]] ; then
      TCL_LIB_PATH=`echo "puts [[set auto_path]]" | $TCLSH -`
			TCL_LIB_PATHS=`echo "$TCL_LIB_PATH" | perl -n -e '@list = split(/\s/);  @hash{@list} = 1 x @list;  foreach @S|@ele (keys %hash) { @S|@ele =~ s/\/\//\//g; print "-L@S|@ele -Wl,-R@S|@ele " }'` 
      TCL_VERSION=$version
      # Annoyingly, on Cygwin, the library doesn't include a '.' in the name.
			case ${build_os} in
			cygwin*)
				TCL_CW_VERSION=$(echo "$TCL_VERSION" | sed 's/\.//')
	     	TCL_LIBS=-ltcl$TCL_CW_VERSION		
	     	TCL_STATIC_LIBS=-l:libtcl${TCL_CW_VERSION}.a
				;;
			*)
	      TCL_LIBS=-ltcl$TCL_VERSION
	      TCL_STATIC_LIBS=-l:libtcl${TCL_VERSION}.a
			esac
			min_version=8.5
			if [(test "${version/./}" -lt "${min_version/./}")]; then
	    	 #-- use implicit include path discovery
	    	TCL_EXE=`echo "puts [[info nameofexecutable]]" | $TCLSH -`
	    	TCL_INC_PATH=`echo "$TCL_EXE" | perl -n -e '$line = $_; $line =~ s/\w+\/tclsh/include\/tcl/; print $line'`
	    	TCL_INC_PATHS=-I$TCL_INC_PATH
			else #-- if version found is >= 8.5 then use include explicitly
        TCL_INC_PATH=`echo "puts [[::tcl::pkgconfig get includedir,runtime]]" | $TCLSH - | perl -n -e '@S|@line = @S|@_; @S|@line =~ s/\/\//\//g; print @S|@line'`
	    	TCL_INC_PATHS=-I$TCL_INC_PATH
			fi

			## Check if user overide of include file directory is to be used
			if [[ x$ac_tclinc != x ]] ; then
     	   TCL_INC_PATHS=-I$ac_tclinc
   		fi

  		ac_cflags_orig=$CPPFLAGS
  		ac_ldflags_orig=$LDFLAGS
			ac_libs_orig=$LIBS

			# Make sure that we can compile a simple program which links with tcl.
			AC_MSG_CHECKING([that we can compile a simple TCL C program.])
  		CPPFLAGS="$CPPFLAGS $TCL_INC_PATHS"
			LDFLAGS="$LDFLAGS $TCL_LIB_PATHS"
			LIBS="$LIBS $TCL_LIBS"
			
			bzip2Exists=yes
	    AC_RUN_IFELSE([AC_LANG_SOURCE([
											
#include <tcl.h>

int main(int argc,const char **argv)
{
  Tcl_CreateInterp();
}

	        ])],AC_MSG_RESULT([ok.]),AC_MSG_RESULT([no.]); TclExists=no)

	  	CPPFLAGS="$ac_cflags_orig"
			LDFLAGS="$ac_ldflags_orig"
			LIBS="$ac_libs_orig"
			
   fi
  
   AC_SUBST(TCLSH)
   AC_SUBST(TCL_LIB_PATHS)
   AC_SUBST(TCL_VERSION)
   AC_SUBST(TCL_LIBS)
   AC_SUBST(TCL_STATIC_LIBS)
   AC_SUBST(TCL_INC_PATHS)

   if [[ $TclExists = yes ]]; then
    	ifelse([$2], , :, [$2])
   else
    	ifelse([$3], , :, [$3])
   fi
])dnl
