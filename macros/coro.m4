##
## ADL_CORO
##   Configure coroutine usage.  This prefers the setjmp/longjmp routines for Linux.  Otherwise,
##   if makecontext/swapcontext are defined, it will use those if possible.  For Solaris, which
##   will use makecontext, we must specify that we want to store the top-of-stack pointer, in order
##   to work around a bug in Solaris' code.
##

AC_DEFUN([ADL_CORO],
[
	AC_CHECK_FUNCS(makecontext getcontext swapcontext)	

	AH_TEMPLATE([USE_LONGJMP],[Set to 1 if the longjmp/setjmp coroutine technique is to be preferred.])
	AH_TEMPLATE([STORE_TOP_OF_STACK],[Set to 1 if the top-of-stack location should be stored for makecontext.])	

	UseLongJmp=no
	case $host in
	*linux*)
		UseLongJmp=yes
		;;
	*solaris*)
		AC_DEFINE([STORE_TOP_OF_STACK],1)
		;;
	esac

	# We can only use setjmp/longjmp if the appropriate defines exist for
	# elements in the jump buffer.  If not, then we have to use ucontext.
	if [[ ${UseLongJmp} = yes ]]; then
		AC_MSG_CHECKING([that we can use setjmp/longjmp for coroutines])
    		AC_COMPILE_IFELSE([AC_LANG_SOURCE([

  #include "setjmp.h"

  int main() {
  #if defined(__GLIBC__) && defined(__GLIBC_MINOR__) && __GLIBC__ >= 2 && __GLIBC_MINOR__ >= 0 
  # if defined(JB_PC) && defined(JB_SP)
	return 0;
  # elif defined (JB_PC) && defined (JB_RSP)
	return 0;
  # else
  #   error "Unsupported setjmp/longjmp platform (1)."
  # endif
  #elif defined(__GLIBC__) && defined(__GLIBC_MINOR__)\
    && __GLIBC__ >= 2 && __GLIBC_MINOR__ >= 0 && defined(__mc68000__)
	  return 0;
  #elif defined(__GNU_LIBRARY__) && defined(__i386__)
	  return 0;
  #elif defined(_WIN32) && (defined(_MSC_VER) || defined(__MINGW32__))
	  return 0;
  #elif defined(__GLIBC__) && defined(__GLIBC_MINOR__) \
    && __GLIBC__ >= 2 && __GLIBC_MINOR__ >= 0 && (defined(__powerpc64__) || defined(__powerpc__))
	  return 0;
  #else
  # error "Unsupported setjmp/longjmp platform (2)."
  #endif
  }
		])],AC_MSG_RESULT([ok.]),
		AC_MSG_RESULT([no.]) 
		UseLongJmp=no
		)				
	fi

	if [[ ${UseLongJmp} = yes ]]; then
		AC_DEFINE([USE_LONGJMP],1)
	fi

])
