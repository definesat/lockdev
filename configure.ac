AC_INIT([lockdev],[1.0.3],[lockdev-devel@lists.alioth.debian.org])
AC_CONFIG_SRCDIR([src/lockdev.c])
AM_CONFIG_HEADER([config.h])
AC_CONFIG_MACRO_DIR([m4])

AC_PREREQ(2.53)
AC_CANONICAL_HOST
AM_INIT_AUTOMAKE([1.6 foreign no-dist-gzip dist-bzip2 -Wall])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])],
	[AC_SUBST([AM_DEFAULT_VERBOSITY], [1])])

dnl Checks for programs.
AC_PROG_CC

dnl Checks for header files.
AC_HEADER_STDC

LT_INIT([disable-static])

dnl check if user wants debug
AC_MSG_CHECKING([whether to enable debug output])
AC_ARG_ENABLE(debug,AS_HELP_STRING([--enable-debug],[turn on debugging code]),,enable_debug=no)
if test x$enable_debug != xno; then
	test x$enable_debug != xyes || enable_debug=1
	AC_MSG_RESULT([yes, level $enable_debug])
else
	enable_debug=0
	AC_MSG_RESULT([no])
fi
AC_DEFINE_UNQUOTED([DEBUG], $enable_debug, [set default debug level])

AC_CONFIG_FILES([
	Makefile
	src/Makefile
	docs/Makefile
	lockdev.pc
])
AC_OUTPUT
