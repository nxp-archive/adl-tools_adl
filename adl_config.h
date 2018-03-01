#ifndef _ADL_CONFIG_H
#define _ADL_CONFIG_H 1
 
/* adl_config.h. Generated automatically at end of configure. */
/* adl_config_internal.h.  Generated from adl_config_internal.h.in by configure.  */
/* adl_config_internal.h.in.  Generated from configure.ac by autoheader.  */

/* Define if building universal (internal helper macro) */
/* #undef AC_APPLE_UNIVERSAL_BUILD */

/* Set to the path of the C compiler that we're using. */
#ifndef ADL_CPROG 
#define ADL_CPROG  "/pkg/OSS-gcc-/5.4.0/x86_64-linux2.6-glibc2.12/bin/gcc" 
#endif

/* Set to the path of the C++ compiler that we're using. */
#ifndef ADL_CXXPROG 
#define ADL_CXXPROG  "/pkg/OSS-gcc-/5.4.0/x86_64-linux2.6-glibc2.12/bin/g++" 
#endif

/* Set to the path of Apache FOP. */
#ifndef ADL_FOP 
#define ADL_FOP  "/pkg/apache-fop-/1.0/bin/fop" 
#endif

/* define if the Boost library is available */
#ifndef ADL_HAVE_BOOST 
#define ADL_HAVE_BOOST  /**/ 
#endif

/* Define to 1 if you have the <dlfcn.h> header file. */
#ifndef ADL_HAVE_DLFCN_H 
#define ADL_HAVE_DLFCN_H  1 
#endif

/* Define to 1 if you have the `fork' function. */
#ifndef ADL_HAVE_FORK 
#define ADL_HAVE_FORK  1 
#endif

/* Define to 1 if you have the <inttypes.h> header file. */
#ifndef ADL_HAVE_INTTYPES_H 
#define ADL_HAVE_INTTYPES_H  1 
#endif

/* Defined and set to 1 if lz4 exists. */
#ifndef ADL_HAVE_LZ4 
#define ADL_HAVE_LZ4  1 
#endif

/* Define to 1 if you have the <memory.h> header file. */
#ifndef ADL_HAVE_MEMORY_H 
#define ADL_HAVE_MEMORY_H  1 
#endif

/* Define to 1 if you have the <stdint.h> header file. */
#ifndef ADL_HAVE_STDINT_H 
#define ADL_HAVE_STDINT_H  1 
#endif

/* Define to 1 if you have the <stdlib.h> header file. */
#ifndef ADL_HAVE_STDLIB_H 
#define ADL_HAVE_STDLIB_H  1 
#endif

/* Define to 1 if you have the <strings.h> header file. */
#ifndef ADL_HAVE_STRINGS_H 
#define ADL_HAVE_STRINGS_H  1 
#endif

/* Define to 1 if you have the <string.h> header file. */
#ifndef ADL_HAVE_STRING_H 
#define ADL_HAVE_STRING_H  1 
#endif

/* Define to 1 if you have the <sys/stat.h> header file. */
#ifndef ADL_HAVE_SYS_STAT_H 
#define ADL_HAVE_SYS_STAT_H  1 
#endif

/* Define to 1 if you have the <sys/types.h> header file. */
#ifndef ADL_HAVE_SYS_TYPES_H 
#define ADL_HAVE_SYS_TYPES_H  1 
#endif

/* Define to 1 if you have the <unistd.h> header file. */
#ifndef ADL_HAVE_UNISTD_H 
#define ADL_HAVE_UNISTD_H  1 
#endif

/* Define to 1 if you have the `vfork' function. */
#ifndef ADL_HAVE_VFORK 
#define ADL_HAVE_VFORK  1 
#endif

/* Define to 1 if you have the <vfork.h> header file. */
/* #undef HAVE_VFORK_H */

/* Define to 1 if `fork' works. */
#ifndef ADL_HAVE_WORKING_FORK 
#define ADL_HAVE_WORKING_FORK  1 
#endif

/* Define to 1 if `vfork' works. */
#ifndef ADL_HAVE_WORKING_VFORK 
#define ADL_HAVE_WORKING_VFORK  1 
#endif

/* Defined and set to 1 if we're creating the front-end libraries and
   programs. */
#ifndef ADL_LANG_FRONTEND 
#define ADL_LANG_FRONTEND  1 
#endif

/* Set to 1 if usage logging is enabled. */
#ifndef ADL_LOG_USAGE 
#define ADL_LOG_USAGE  1 
#endif

/* Define to the sub-directory in which libtool stores uninstalled libraries.
   */
#ifndef ADL_LT_OBJDIR 
#define ADL_LT_OBJDIR  ".libs/" 
#endif

/* Defined and set to 1 if we're creating a minimal build. */
/* #undef MINIMAL_BUILD */

/* Define to the address where bug reports for this package should be sent. */
#ifndef ADL_PACKAGE_BUGREPORT 
#define ADL_PACKAGE_BUGREPORT  "" 
#endif

/* Define to the full name of this package. */
#ifndef ADL_PACKAGE_NAME 
#define ADL_PACKAGE_NAME  "adl" 
#endif

/* Define to the full name and version of this package. */
#ifndef ADL_PACKAGE_STRING 
#define ADL_PACKAGE_STRING  "adl 3.17.2" 
#endif

/* Define to the one symbol short name of this package. */
#ifndef ADL_PACKAGE_TARNAME 
#define ADL_PACKAGE_TARNAME  "adl" 
#endif

/* Define to the home page for this package. */
#ifndef ADL_PACKAGE_URL 
#define ADL_PACKAGE_URL  "" 
#endif

/* Define to the version of this package. */
#ifndef ADL_PACKAGE_VERSION 
#define ADL_PACKAGE_VERSION  "3.17.2" 
#endif

/* Set to the path of the Perl interpreter we're using. */
#ifndef ADL_PERL 
#define ADL_PERL  "/pkg/perl-/5.8.9-FSL/x86_64-linux2.6-glibc2.5/bin/perl" 
#endif

/* Set to the path of the Restructured Text (prest) interpreter we're using.
   */
#ifndef ADL_PREST 
#define ADL_PREST  "/pkg/prest-/0.003040/bin/prest" 
#endif

/* Define to 1 if you have the ANSI C header files. */
#ifndef ADL_STDC_HEADERS 
#define ADL_STDC_HEADERS  1 
#endif

/* Set to the port number of the server to use for usage logging, if enabled.
   */
#ifndef ADL_USAGE_LOG_PORT 
#define ADL_USAGE_LOG_PORT  1999 
#endif

/* Set to the name of the server to use for usage logging, if enabled. */
#ifndef ADL_USAGE_LOG_SERVER 
#define ADL_USAGE_LOG_SERVER  "usagelog.am.freescale.net" 
#endif

/* Define WORDS_BIGENDIAN to 1 if your processor stores words with the most
   significant byte first (like Motorola and SPARC, unlike Intel). */
#if defined AC_APPLE_UNIVERSAL_BUILD
# if defined __BIG_ENDIAN__
#  define WORDS_BIGENDIAN 1
# endif
#else
# ifndef WORDS_BIGENDIAN
/* #  undef WORDS_BIGENDIAN */
# endif
#endif

/* Define to 1 if `lex' declares `yytext' as a `char *' by default, not a
   `char[]'. */
/* #undef YYTEXT_POINTER */

/* Define to `int' if <sys/types.h> does not define. */
/* #undef pid_t */

/* Define as `fork' if `vfork' does not work. */
/* #undef vfork */
 
/* once: _ADL_CONFIG_H */
#endif
