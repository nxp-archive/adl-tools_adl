##
## ADL_BISON([version], [, action-if-found [,action-if-not-found]])
##   configure bison and check bison version
##
## Command-line options:
##   --with-bison to let a user specify the location of the bison program.
##
##   version:  Minimum required version,
##   action-if-found:  Executed if bison is found.
##   action-if-not-found:  Executed if bison is not found.
##
## Output: 
##
##   YACC
##
AC_DEFUN([ADL_BISON],
[
  AC_ARG_WITH([bison],AC_HELP_STRING([--with-bison],[specify a path to use to find the bison program.]),ac_bison=$withval)
  if [[ x$ac_bison != x ]] ; then
		 YACC=${ac_bison}/bison
  fi

  ## check bison
  AC_PROG_YACC
  
  BisonVersion=`$YACC -V | head -1 | cut -d' ' -f4`

	AC_MSG_CHECKING([for bison's version ($1 required).])
  BisonGood=yes
	if [[ ! -z ${BisonVersion} ]]; then
  	 DPP_VERSION_CHECK([$BisonVersion], [$1], , [BisonGood=no])	
	else
		 BisonVersion="(not found)"
		 BisonGood=no
	fi

  if [[ $BisonGood = yes ]]; then
	  AC_MSG_RESULT([$BisonVersion (okay).])
  	ifelse([$2], , :, [$2])

	  # Now create the bison-version file.  We only touch/create the file if needed (a version change has occurred),
	  old_version=$(cat bison-version 2>/dev/null)
	  if [[ "x{$BisonVersion}" != "x{$old_version}" ]]; then
		  AC_MSG_NOTICE([New bison version file created:  $BisonVersion])      
		  echo "$BisonVersion" > bison-version		
    fi
  else
		AC_MSG_RESULT([Found $BisonVersion but $1 is the minimum required.])	
    ifelse([$3], , :, [$3])
  fi	

])
