##
## ADL_FLEX([version], [, action-if-found [,action-if-not-found]])
##   Find flex, check its version, and export any necessary variables for its use.
##
##   This also creates a file, flex-version, which stores the version number.  Your makefiles
##   may use this as a dependency so that generated source will be re-built if there's a version change.
##
##   version:  Minimum required version,
##   action-if-found:  Executed if flex is found.
##   action-if-not-found:  Executed if flex is not found.
##
## Command-line options:
##   --with-flex to let a user specify the location of the flex program.
##   --with-flex-include: Specify location of flex's include file.
##
## Output:
##
##   LEX:             flex program.
##   LEX_CFLAGS:      Include directive/CFlags for using flex.
## 
##
AC_DEFUN([ADL_FLEX],
[
  AC_ARG_WITH([flex],AC_HELP_STRING([--with-flex],[specify a path to use to find the flex program.]),ac_flex=$withval)
  if [[ x$ac_flex != x ]] ; then
		 LEX=${ac_flex}/flex
  fi

  FlexExists=yes
  AM_PROG_LEX
  if $LEX --version | grep -q flex; then
		FlexExists=yes
	else
    AC_MSG_WARN([Could not find flex in the path.  Please modify your path or use the --with-flex option.])
		FlexExists=no
  fi

  ac_flex_include=
  AC_ARG_WITH([flex-include],AC_HELP_STRING([--with-flex-include],[specify the location of flex\'s header file FlexLexer.h.]),ac_flex_include=$withval)

  FLEX_VERSION=
  if [[ $FlexExists != no ]] ; then
	    # First, check for the version.
			FLEX_VERSION=`$LEX --version | awk '{print $(NF) }'`
			AC_MSG_CHECKING([for flex's version ($1 required).])
			DPP_VERSION_CHECK([$FLEX_VERSION], [$1], , [FlexExists=no])	
			if [[ $FlexExists = no ]] ; then	
					AC_MSG_RESULT([Found $FLEX_VERSION but $1 is the minimum required.])
			else
					AC_MSG_RESULT([$FLEX_VERSION (okay).])
			fi

			AC_MSG_CHECKING([for FlexLexer.h])
			if [[ $FlexExists = yes ]]; then
  	      # Now try and find the header file required for creating C++ scanners.
		      # If the user has specified a path, then use it.  Otherwise, try to find flex's
          # location and locate the include file from there.
					if [[ x$ac_flex_include = x ]] ; then
              # Try to discover the location using which.  If it's a symbolic link,
	            # we'll follow that.
							loc=$(which $LEX)
							if [[ -L $loc ]] ; then
									loc=$($READLINK $loc)
							fi
							loc=$(dirname $loc)
							if [[ -d $loc/../include ]] ; then
									ac_flex_include=$(cd $loc/../include && pwd)
							else
                  # Wasn't found, so maybe we can use the wrapper or tr.dynamic.
									dyncmd=
									if which w.which > /dev/null 2>&1 ; then
											dyncmd="w.which"
									elif which tr.which > /dev/null 2>&1 ; then
											dyncmd="tr.which"
									fi
									if [[ ! -z $dyncmd ]]; then
											loc=$(dirname $($dyncmd $LEX))
											if [[ -d $loc/../include ]] ; then
	      									ac_flex_include=$(cd $loc/../include && pwd)
											else
													AC_MSG_WARN([Could not find FlexLexer.h\'s location either by using which or $dyncmd.  You must specify its location manually using --with-flex-include])
													FlexExists=no
											fi
									else
										AC_MSG_WARN([Could not find FlexLexer.h\'s location which and no dynamic dispatch command (w.which or tr.which) was found.  You must specify its location manually using --with-flex-include])
										FlexExists=no
									fi
							fi
 					fi
			fi
  fi

	if [[ $FlexExists = no ]]; then
	  ac_flex_include=$(pwd ${srcdir})/extras
	fi

  if [[ ! -e $ac_flex_include/FlexLexer.h ]] ; then
		# We really shouldn't ever get this error, since we'll use our own copy if
		# flex doesn't exist.  So, this mainly exists to catch bad flex installs.
    AC_MSG_ERROR([Could not find FlexLexer.h\'s location.  You must specify its location manually using --with-flex-include])
  else
		AC_MSG_RESULT([$ac_flex_include/FlexLexer.h ])
  fi

	# We can always set this because we always have FlexLexer.h (either our own, or the one specified by flex).
  LEX_CFLAGS="-I$ac_flex_include"
  if [[ $FlexExists = yes ]]; then

			ifelse([$2], , :, [$2])

	    # Now create the flex-version file.  We only touch/create the file if needed (a version change has occurred),
			old_version=$(cat flex-version 2>/dev/null)
			if [[ "x{$FLEX_VERSION}" != "x{$old_version}" ]]; then
					AC_MSG_NOTICE([New flex version file created:  $FLEX_VERSION])      
					echo "$FLEX_VERSION" > flex-version		
			fi
  else
			ifelse([$3], , :, [$3])
  fi

  AC_SUBST(LEX_CFLAGS)
])
