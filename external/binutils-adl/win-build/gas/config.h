/* config.h.  Generated from config.in by configure.  */
/* config.in.  Generated from configure.in by autoheader.  */

/* Define if building universal (internal helper macro) */
/* #undef AC_APPLE_UNIVERSAL_BUILD */

/* Define if using AIX 5.2 value for C_WEAKEXT. */
/* #undef AIX_WEAK_SUPPORT */

/* assert broken? */
/* #undef BROKEN_ASSERT */

/* Define to one of `_getb67', `GETB67', `getb67' for Cray-2 and Cray-YMP
   systems. This function is required for `alloca.c' support on those systems.
   */
/* #undef CRAY_STACKSEG_END */

/* Compiling cross-assembler? */
#define CROSS_COMPILE 1

/* Define to 1 if using `alloca.c'. */
/* #undef C_ALLOCA */

/* Default architecture. */
/* #undef DEFAULT_ARCH */

/* Default CRIS architecture. */
/* #undef DEFAULT_CRIS_ARCH */

/* Default emulation. */
#define DEFAULT_EMULATION ""

/* Supported emulations. */
#define EMULATIONS 

/* Define if you want run-time sanity checks. */
/* #undef ENABLE_CHECKING */

/* Define to 1 if translation of program messages to the user's native
   language is requested. */
// MSVC:  No translation enabled.
//#define ENABLE_NLS 1
#undef ENABLE_NLS

/* Define to 1 if you have `alloca', as a function or macro. */
#define HAVE_ALLOCA 1

/* Define to 1 if you have <alloca.h> and it should be used (not on Ultrix).
   */
/* #undef HAVE_ALLOCA_H */

/* Is the prototype for getopt in <unistd.h> in the expected format? */
#define HAVE_DECL_GETOPT 1

/* Define to 1 if you have the declaration of `mempcpy', and to 0 if you
   don't. */
#define HAVE_DECL_MEMPCPY 0

/* Define to 1 if you have the declaration of `vsnprintf', and to 0 if you
   don't. */
#define HAVE_DECL_VSNPRINTF 1

/* Define to 1 if you have the <dlfcn.h> header file. */
/* #undef HAVE_DLFCN_H */

/* Define to 1 if you have the <errno.h> header file. */
#define HAVE_ERRNO_H 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the <limits.h> header file. */
#define HAVE_LIMITS_H 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the `remove' function. */
/* #undef HAVE_REMOVE */

/* Define to 1 if you have the `sbrk' function. */
/* #undef HAVE_SBRK */

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define if <sys/stat.h> has struct stat.st_mtim.tv_nsec */
/* #undef HAVE_ST_MTIM_TV_NSEC */

/* Define if <sys/stat.h> has struct stat.st_mtim.tv_sec */
/* #undef HAVE_ST_MTIM_TV_SEC */

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <time.h> header file. */
#define HAVE_TIME_H 1

/* Define if <time.h> has struct tm.tm_gmtoff. */
/* #undef HAVE_TM_GMTOFF */

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to 1 if you have the `unlink' function. */
#define HAVE_UNLINK 1

/* Define to 1 if you have the <zlib.h> header file. */
/* #undef HAVE_ZLIB_H */

/* Using i386 COFF? */
/* #undef I386COFF */

/* Define to the sub-directory in which libtool stores uninstalled libraries.
   */
#define LT_OBJDIR ".libs/"

/* Using m68k COFF? */
/* #undef M68KCOFF */

/* Using m88k COFF? */
/* #undef M88KCOFF */

/* Default CPU for MIPS targets. */
/* #undef MIPS_CPU_STRING_DEFAULT */

/* Generate 64-bit code by default on MIPS targets. */
/* #undef MIPS_DEFAULT_64BIT */

/* Choose a default ABI for MIPS targets. */
/* #undef MIPS_DEFAULT_ABI */

/* Define if environ is not declared in system header files. */
/* #undef NEED_DECLARATION_ENVIRON */

/* Define if errno is not declared in system header files. */
/* #undef NEED_DECLARATION_ERRNO */

/* Define if ffs is not declared in system header files. */
#define NEED_DECLARATION_FFS 1

/* Define if free is not declared in system header files. */
/* #undef NEED_DECLARATION_FREE */

/* Define if malloc is not declared in system header files. */
/* #undef NEED_DECLARATION_MALLOC */

/* Define if sbrk is not declared in system header files. */
#define NEED_DECLARATION_SBRK 1

/* Define if strstr is not declared in system header files. */
/* #undef NEED_DECLARATION_STRSTR */

/* a.out support? */
/* #undef OBJ_MAYBE_AOUT */

/* b.out support? */
/* #undef OBJ_MAYBE_BOUT */

/* COFF support? */
/* #undef OBJ_MAYBE_COFF */

/* ECOFF support? */
/* #undef OBJ_MAYBE_ECOFF */

/* ELF support? */
/* #undef OBJ_MAYBE_ELF */

/* generic support? */
/* #undef OBJ_MAYBE_GENERIC */

/* SOM support? */
/* #undef OBJ_MAYBE_SOM */

/* Name of package */
#define PACKAGE "gas"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT ""

/* Define to the full name of this package. */
#define PACKAGE_NAME ""

/* Define to the full name and version of this package. */
#define PACKAGE_STRING ""

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME ""

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION ""

/* Define if defaulting to ELF on SCO 5. */
/* #undef SCO_ELF */

/* If using the C implementation of alloca, define if you know the
   direction of stack growth for your system; otherwise it will be
   automatically deduced at runtime.
	STACK_DIRECTION > 0 => grows toward higher addresses
	STACK_DIRECTION < 0 => grows toward lower addresses
	STACK_DIRECTION = 0 => direction of growth unknown */
/* #undef STACK_DIRECTION */

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Using strict COFF? */
/* #undef STRICTCOFF */

/* Target alias. */
#define TARGET_ALIAS "ppc-elf64"

/* Define as 1 if big endian. */
#define TARGET_BYTES_BIG_ENDIAN 1

/* Canonical target. */
#define TARGET_CANONICAL "powerpc-unknown-elf64"

/* Target CPU. */
#define TARGET_CPU "powerpc"

/* Target OS. */
#define TARGET_OS "elf64"

/* Define if default target is PowerPC Solaris. */
/* #undef TARGET_SOLARIS_COMMENT */

/* Define if target is Symbian OS. */
/* #undef TARGET_SYMBIAN */

/* Target vendor. */
#define TARGET_VENDOR "unknown"

/* Use b modifier when opening binary files? */
#define USE_BINARY_FOPEN 1

/* Use emulation support? */
/* #undef USE_EMULATIONS */

/* Allow use of E_MIPS_ABI_O32 on MIPS targets. */
/* #undef USE_E_MIPS_ABI_O32 */

/* Enable extensions on AIX 3, Interix.  */
#ifndef _ALL_SOURCE
# define _ALL_SOURCE 1
#endif
/* Enable GNU extensions on systems that have them.  */
#ifndef _GNU_SOURCE
# define _GNU_SOURCE 1
#endif
/* Enable threading extensions on Solaris.  */
#ifndef _POSIX_PTHREAD_SEMANTICS
# define _POSIX_PTHREAD_SEMANTICS 1
#endif
/* Enable extensions on HP NonStop.  */
#ifndef _TANDEM_SOURCE
# define _TANDEM_SOURCE 1
#endif
/* Enable general extensions on Solaris.  */
#ifndef __EXTENSIONS__
# define __EXTENSIONS__ 1
#endif


/* Using cgen code? */
/* #undef USING_CGEN */

/* Version number of package */
#define VERSION "2.21.1"

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

/* Number of bits in a file offset, on hosts where this is settable. */
/* #undef _FILE_OFFSET_BITS */

/* Define for large files, on AIX-style hosts. */
/* #undef _LARGE_FILES */

/* Define to 1 if on MINIX. */
/* #undef _MINIX */

/* Define to 2 if the system does not provide POSIX.1 features except with
   this defined. */
/* #undef _POSIX_1_SOURCE */

/* Define to 1 if you need to in order for `stat' and other things to work. */
/* #undef _POSIX_SOURCE */

/* Define to `__inline__' or `__inline' if that's what the C compiler
   calls it, or to nothing if 'inline' is not supported under any name.  */
#ifndef __cplusplus
/* #undef inline */
#endif

/* Define to `unsigned int' if <sys/types.h> does not define. */
/* #undef size_t */