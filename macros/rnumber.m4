dnl AM_PATH_RNUMBER([MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND [, MODULES]]]])
dnl Test for RNUMBER, and define RNUMBER_CFLAGS, RNUMBER_LIBS, RNUMBER_STATIC_LIBS, RNUMBER_HELPERS, RNUMBER_LIB_PATH, 
dnl and RNUMBER_LIB_LIB
dnl
AC_DEFUN([AM_PATH_RNUMBER],
[dnl 
   AC_ARG_WITH([rnumber],AC_HELP_STRING([--with-rnumber],[specify a path to use to find the rnumber-config program.]),ac_rnumber=$withval)
   if [[ x$ac_rnumber = x ]] ; then
     ac_rnumber=$PATH
   fi
   AC_PATH_PROG(RNUMBER_CONFIG, rnumber-config, no, $ac_rnumber)
   min_rnumber_version=ifelse([$1], ,0.0.1,$1)
   AC_MSG_CHECKING(for rnumber version $min_rnumber_version)
   no_rnumber="" 
   enable_rnumbertest="yes"
   if test "$RNUMBER_CONFIG" = "no" ; then
     no_rnumber=yes
   else 
     RNUMBER_CFLAGS=`$RNUMBER_CONFIG $rnumber_config_args --cflags`
	 RNUMBER_LIBS=`$RNUMBER_CONFIG $rnumber_config_args --libs`
	 RNUMBER_LTLIBS=`$RNUMBER_CONFIG $rnumber_config_args --ltlibs`
     RNUMBER_STATIC_LIBS=`$RNUMBER_CONFIG $rnumber_config_args --static-libs`
	 RNUMBER_LIB_PATHS=`$RNUMBER_CONFIG $rnumber_config_args --libs-only-L`
	 RNUMBER_LIBS_ONLY=`$RNUMBER_CONFIG $rnumber_config_args --libs-only-l`
     RNUMBER_BL_LIB_PATH=`$RNUMBER_CONFIG --bl-libs-only-L`
     RNUMBER_BL_LIB_LIB=`$RNUMBER_CONFIG --bl-libs-only-l`
     RNUMBER_HELPERS=`$RNUMBER_CONFIG $rnumber_config_args --helper-path`
     RNUMBER_DEPEND=`$RNUMBER_CONFIG $rnumber_config_args --depend`
     rnumber_config_major_version=`$RNUMBER_CONFIG $rnumber_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
     rnumber_config_minor_version=`$RNUMBER_CONFIG $rnumber_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
     rnumber_config_micro_version=`$RNUMBER_CONFIG $rnumber_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`
     if test "x$enable_rnumbertest" = "xyes" ; then
       ac_save_CFLAGS="$CFLAGS"
       ac_save_CXXFLAGS="$CXXFLAGS"
       ac_save_LIBS="$LIBS"
       CFLAGS="$CFLAGS $RNUMBER_CFLAGS"
       CXXFLAGS=$CFLAGS
       LIBS="$RNUMBER_STATIC_LIBS $LIBS"
dnl
dnl Now check if the installed rnumber is sufficiently new. (Also sanity
dnl checks the results of rnumber-config to some extent).
dnl
      rm -f conf.rnumbertest
			AC_TRY_RUN([
#include <rnumber/rnumber_version.h>
#include <rnumber/RNumber.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <strings.h>

int 
main ()
{
  int major, minor, micro;
  char *tmp_version;

  system ("touch conf.rnumbertest");

  /* HP/UX 9 (%@#!) writes to sscanf strings */
  tmp_version = strdup("$min_rnumber_version");
  if (sscanf(tmp_version, "%d.%d.%d", &major, &minor, &micro) != 3) {
     printf("%s, bad version string\n", "$min_rnumber_version");
     exit(1);
   }

	 /*  printf("rnumber version %d.%d.%d.\n", rnumber_major_version, 
	 	rnumber_minor_version, rnumber_micro_version ); */

  if ((rnumber_major_version != $rnumber_config_major_version) ||
      (rnumber_minor_version != $rnumber_config_minor_version) ||
      (rnumber_micro_version != $rnumber_config_micro_version))
    {
      printf("\n*** 'rnumber-config --version' returned %d.%d.%d, but RNUMBER (%d.%d.%d)\n", 
             $rnumber_config_major_version, $rnumber_config_minor_version, $rnumber_config_micro_version,
             rnumber_major_version, rnumber_minor_version, rnumber_micro_version);
      printf ("*** was found! If rnumber-config was correct, then it is best\n");
      printf ("*** to remove the old version of RNUMBER. You may also be able to fix the error\n");
      printf("*** by modifying your LD_LIBRARY_PATH enviroment variable, or by editing\n");
      printf("*** /etc/ld.so.conf. Make sure you have run ldconfig if that is\n");
      printf("*** required on your system.\n");
      printf("*** If rnumber-config was wrong, set the environment variable RNUMBER_CONFIG\n");
      printf("*** to point to the correct copy of rnumber-config, and remove the file config.cache\n");
      printf("*** before re-running configure\n");
    } 
#if defined (RNUMBER_MAJOR_VERSION) && defined (RNUMBER_MINOR_VERSION) && defined (RNUMBER_MICRO_VERSION)
  else if ((rnumber_major_version != RNUMBER_MAJOR_VERSION) ||
	   (rnumber_minor_version != RNUMBER_MINOR_VERSION) ||
           (rnumber_micro_version != RNUMBER_MICRO_VERSION))
    {
      printf("*** RNUMBER header files (version %d.%d.%d) do not match\n",
	     RNUMBER_MAJOR_VERSION, RNUMBER_MINOR_VERSION, RNUMBER_MICRO_VERSION);
      printf("*** library (version %d.%d.%d)\n",
	     rnumber_major_version, rnumber_minor_version, rnumber_micro_version);
    }
#endif /* defined (RNUMBER_MAJOR_VERSION) ... */
  else
    {
      if ((rnumber_major_version > major) ||
        ((rnumber_major_version == major) && (rnumber_minor_version > minor)) ||
        ((rnumber_major_version == major) && (rnumber_minor_version == minor) && (rnumber_micro_version >= micro)))
      {
        return 0;
       }
     else
      {
        printf("\n*** An old version of RNUMBER (%d.%d.%d) was found.\n",
               rnumber_major_version, rnumber_minor_version, rnumber_micro_version);
        printf("*** You need a version of RNUMBER newer than %d.%d.%d. The latest version of\n",
	       major, minor, micro);
        printf("*** RNUMBER is always available from ftp://ftp.rnumber.org.\n");
        printf("***\n");
        printf("*** If you have already installed a sufficiently new version, this error\n");
        printf("*** probably means that the wrong copy of the rnumber-config shell script is\n");
        printf("*** being found. The easiest way to fix this is to remove the old version\n");
        printf("*** of RNUMBER, but you can also set the RNUMBER_CONFIG environment to point to the\n");
        printf("*** correct copy of rnumber-config. (In this case, you will have to\n");
        printf("*** modify your LD_LIBRARY_PATH enviroment variable, or edit /etc/ld.so.conf\n");
        printf("*** so that the correct libraries are found at run-time))\n");
      }
    }
  return 1;
}
],dnl action if true
,dnl action if false
 no_rnumber=yes,dnl
dnl action if cross-compiling
[echo $ac_n "cross compiling; assumed OK... $ac_c"])dnl end of AC_TRY_RUN

       CFLAGS="$ac_save_CFLAGS"
       CXXFLAGS="$ac_save_CXXFLAGS"
       LIBS="$ac_save_LIBS"
     fi
  fi
  if test "x$no_rnumber" = x ; then
     AC_MSG_RESULT(yes)
     ifelse([$2], , :, [$2])     
  else
     AC_MSG_RESULT(no)
     if test "$RNUMBER_CONFIG" = "no" ; then
       echo "*** The rnumber-config script installed by RNUMBER could not be found"
       echo "*** If RNUMBER was installed in PREFIX, make sure PREFIX/bin is in"
       echo "*** your path, or set the RNUMBER_CONFIG environment variable to the"
       echo "*** full path to rnumber-config."
     else
       if test -f conf.rnumbertest ; then
        :
       else
          echo "*** Could not run RNUMBER test program, checking why..."
          CFLAGS="$CFLAGS $RNUMBER_CFLAGS"
	  CXXFLGAS="$CXXFLAGS $RNUMBER_CFLAGS"
          LIBS="$LIBS $RNUMBER_STATIC_LIBS"
          AC_TRY_LINK([
#include <rnumber/rnumber_version.h>
#include <rnumber/RNumber.h>
#include <stdio.h>
],      [ return ((rnumber_major_version) || (rnumber_minor_version) || (rnumber_micro_version)); ],
        [ echo "*** The test program compiled, but did not run. This usually means"
          echo "*** that the run-time linker is not finding RNUMBER or finding the wrong"
          echo "*** version of RNUMBER. If it is not finding RNUMBER, you'll need to set your"
          echo "*** LD_LIBRARY_PATH environment variable, or edit /etc/ld.so.conf to point"
          echo "*** to the installed location.  Also, make sure you have run ldconfig if that"
          echo "*** is required on your system. Some systems don't support /etc/ld.so.conf. On"
	  echo "*** those systems you must use LD_LIBRARY_PATH."
	  echo "***"
          echo "*** If you have an old version installed, it is best to remove it, although"
          echo "*** you may also be able to get things to work by modifying LD_LIBRARY_PATH"
          echo "***" ],
        [ echo "*** The test program failed to compile or link. See the file config.log for the"
          echo "*** exact error that occured. This usually means RNUMBER was incorrectly installed"
          echo "*** or that you have moved RNUMBER since it was installed. In the latter case, you"
          echo "*** may want to edit the rnumber-config script: $RNUMBER_CONFIG" ])
          CFLAGS="$ac_save_CFLAGS"
          CXXFLAGS="$ac_save_CXXFLAGS"
          LIBS="$ac_save_LIBS"
       fi
     fi
     RNUMBER_CFLAGS=""
     RNUMBER_LIBS=""
     ifelse([$3], , :, [$3])
  fi

  AC_SUBST(RNUMBER_CFLAGS)
  AC_SUBST(RNUMBER_LIBS)
  AC_SUBST(RNUMBER_LTLIBS)
  AC_SUBST(RNUMBER_STATIC_LIBS)
  AC_SUBST(RNUMBER_LIBS_ONLY)
  AC_SUBST(RNUMBER_LIB_PATHS)
  AC_SUBST(RNUMBER_BL_LIB_PATH)
  AC_SUBST(RNUMBER_BL_LIB_LIB)
  AC_SUBST(RNUMBER_HELPERS)
  AC_SUBST(RNUMBER_DEPEND)
  rm -f conf.rnumbertest
])
