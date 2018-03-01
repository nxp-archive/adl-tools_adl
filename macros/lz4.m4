##
## AM_LZ4([, action-if-found [,action-if-not-found]])
##   Configure a program to use lz4.
##
##   action-if-found:  Executed if lz4 is found.
##   action-if-not-found:  Executed if lz4 is not found.
##
## Command-line options:
##   --with-lz4 to let a user specify the location of the lz4 program.
##
## Output:
##
##   LZ4               :  executable path.
##   LZ4_CFLAGS        :  cflags.
##   LZ4_LDFLAGS       :  link line.
##   LZ4_STATIC_LDFLAGS:  static link line.
##   LZ4_EXISTS        :  true|false
##
AC_DEFUN([AM_LZ4],
[
	lz4Exists=yes

	ac_lz4_path="$PATH"
 	AC_ARG_WITH([lz4],AC_HELP_STRING([--with-lz4],[specify a path to use to find the lz4 library and executable.]),ac_lz4=$withval)
 	if [[ x$ac_lz4 != x ]] ; then
 			ac_lz4_include="-I$ac_lz4/include"
   	 	ac_lz4_lib="-L$ac_lz4/lib -Wl,-R$ac_lz4/lib"
			ac_lz4_path="$ac_lz4/bin:$PATH"
	fi

	# We also need to make sure that the program exists for regression use.
	AC_PATH_PROG(LZ4, [lz4], no, $ac_lz4_path)

	if [[ x$LZ4 = xno ]] ; then
			lz4Exists=no
	fi

	lz4CheckPaths=no
	ac_libs_orig=$LIBS
	LIBS="$LIBS -llz4"
	if [[ $lz4Exists = yes ]]; then

		# First, does it just work using default paths?
		AC_MSG_CHECKING([if lz4 works using default paths])

	  AC_RUN_IFELSE([AC_LANG_SOURCE([
											
#include "lz4.h"

    int main() {
	  LZ4_versionNumber();
	}
	        ])],AC_MSG_RESULT([ok (using default paths).]),AC_MSG_RESULT([no])
					lz4CheckPaths=yes)
	fi
	LIBS="$ac_libs_orig"

	if [[ $lz4Exists = yes -a $lz4CheckPaths = yes ]]; then
			if [[ x$ac_lz4_include = x ]]; then
		      # Try to discover the location using which.  If it's a symbolic link,
          # we'll follow that.
          loc=$(which lz4)
          if [[ -L $loc ]] ; then
              loc=$(${READLINK} $loc)
          fi
          loc=$(dirname $loc)
          if [[ -d $loc/../include ]] ; then
              ac_lz4_include=$(cd $loc/../include && pwd)
              ac_lz4_lib=$(cd $loc/../lib && pwd)
          else
							# Wasn't found, so maybe we can use the wrapper or tr.dynamic.
							dyncmd=
							if which w.which > /dev/null 2>&1 ; then
									dyncmd="w.which"
							elif which tr.which > /dev/null 2>&1 ; then
									dyncmd="tr.which"
							fi
							if [[ ! -z $dyncmd ]]; then
									loc=$(dirname $($dyncmd lz4))
									if [[ -d $loc/../include ]] ; then
											ac_lz4_include=$(cd $loc/../include && pwd)
											ac_lz4_lib=$(cd $loc/../lib && pwd)
									else
											AC_MSG_ERROR([Could not find lz4\'s location either by using which or $dyncmd.  You must specify its location manually using --with-lz4])
									fi
							else
									AC_MSG_ERROR([Could not find lz4\'s location either by using which and no dynamic dispatch command (w.which or tr.which) was found.  You must specify its location manually using --with-lz4])
              fi
          fi

					ac_lz4_include="-I$ac_lz4_include"
					ac_lz4_lib="-L$ac_lz4_lib -Wl,-R$ac_lz4_lib"

      fi
			
		  # First, try with default or user-specified paths.
  		AC_MSG_CHECKING([that we can compile against the lz4 library.])

  		ac_cflags_orig=$CPPFLAGS
  		ac_ldflags_orig=$LDFLAGS
			ac_libs_orig=$LIBS
		
  		CPPFLAGS="$CPPFLAGS $ac_lz4_include"
			LDFLAGS="$LDFLAGS $ac_lz4_lib"
			LIBS="$LIBS -llz4"
			
			lz4Exists=yes
	    AC_RUN_IFELSE([AC_LANG_SOURCE([
											
#include "lz4.h"

    int main() {
	  LZ4_versionNumber();
	}
	    ])],AC_MSG_RESULT([ok.]),AC_MSG_RESULT([no])
			lz4Exists=no)

	  	CPPFLAGS="$ac_cflags_orig"
			LDFLAGS="$ac_ldflags_orig"
			LIBS="$ac_libs_orig"
			
	fi

	if [[ ! -z "$ac_lz4_include" ]]; then
		LZ4_CFLAGS="$ac_lz4_include"
	fi
	if [[ ! -z "$ac_lz4_lib" ]]; then
		LZ4_TMP_FLAGS="$ac_lz4_lib"
	fi
	LZ4_LDFLAGS="${LZ4_TMP_FLAGS} -llz4"
	LZ4_STATIC_LDFLAGS="${LZ4_TMP_FLAGS} -llz4"
			
  if [[ $lz4Exists = yes ]]; then
		LZ4_EXISTS=true 
    ifelse([$1], , :, [$1])
  else
		LZ4_EXISTS=false
		ifelse([$2], , :, [$2])
  fi

	AC_SUBST(LZ4)
	AC_SUBST(LZ4_CFLAGS)
	AC_SUBST(LZ4_LDFLAGS)
	AC_SUBST(LZ4_STATIC_LDFLAGS)
	AC_SUBST(LZ4_EXISTS)
	
])
