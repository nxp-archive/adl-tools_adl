##
## ADL_READLINE([, action-if-found [,action-if-not-found]])
##   Find readline and make sure that we can link against it.
##
##   action-if-found:  Executed if readline is found.
##   action-if-not-found:  Executed if readline is not found.
##
## Command-line options:
##   --with-readline: Specify location of readline.  This is the root, where
##                    the library should be in <path>/lib.
##
## Output:
##
##   RL_CFLAGS:         Readline compile options.
##   RL_LDFLAGS:        Readline link options.
##

AC_DEFUN([ADL_READLINE],
[

  ac_rl_base=
  AC_ARG_WITH([readline],AC_HELP_STRING([--with-readline],[specify readline's location.]),ac_rl_base=$withval)

  ac_rl_libs=
  ac_rl_include=
  if [[ x$ac_rl_base != x ]]; then
     ac_rl_libs="-L$ac_rl_base/lib -Wl,-R$ac_rl_base/lib"
	 ac_rl_include="-I$ac_rl_base/include"
  fi


  ac_cflags_orig=$CPPFLAGS
  ac_ldflags_orig=$LDFLAGS
  ac_libs_orig=$LIBS

  CPPFLAGS="$CPPFLAGS $ac_rl_include"
  LDFLAGS="$LDFLAGS $ac_rl_libs"

  AC_CHECK_LIB([readline],[readline], ,AC_MSG_ERROR([The readline library is required in order to build ADL.]),[-lhistory -lcurses])

  CPPFLAGS="$ac_cflags_orig"
  LDFLAGS="$ac_ldflags_orig"
  LIBS="$ac_libs_orig"

  RL_CFLAGS="$ac_rl_include"
  RL_LDFLAGS="$ac_rl_libs -lreadline -lhistory -lcurses"

  AC_SUBST(RL_CFLAGS)
  AC_SUBST(RL_LDFLAGS)

])
