m4_define([lockdev_m4_chomp],
[m4_format([[%.*s]], m4_bregexp(m4_translit([$1], [
]]m4_dquote(_m4_defn([m4_cr_all]))[, [/]]m4_format([%255s], [])[),
  [/*$]), [$1])])
m4_define([lockdev_m4_esyscmd_s],
[lockdev_m4_chomp(m4_esyscmd([$1]))])
dnl Quoting the first argument results in a bizarrely corrupted package tarname
AC_INIT(lockdev_m4_esyscmd_s([sed -ne '/^Package:/{s/Package:[[:space:]][[:space:]]*//p;q}' VERSION]),
        [lockdev_m4_esyscmd_s([sed -ne '/^Version:/{s/Version:[[:space:]][[:space:]]*//p;q}' VERSION])],
        [lockdev-devel@lists.alioth.debian.org])
AC_CONFIG_SRCDIR([src/lockdev.c])
AC_CONFIG_AUX_DIR([scripts])
AC_CONFIG_HEADER([config.h])
AC_CONFIG_MACRO_DIR([m4])

AC_PREREQ(2.53)
AC_CANONICAL_HOST

. "${srcdir}/SHLIBS"
AC_SUBST([LOCKDEV_CURRENT_INTERFACE])
AC_SUBST([LOCKDEV_INTERFACE_AGE])
AC_SUBST([LOCKDEV_BINARY_AGE])

AM_INIT_AUTOMAKE([1.6 gnu no-dist-gzip dist-bzip2 -Wall])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])],
	[AC_SUBST([AM_DEFAULT_VERBOSITY], [1])])

RELEASE_DATE='lockdev_m4_esyscmd_s(date --date='lockdev_m4_esyscmd_s([sed -ne '/^Release-Date:/{s/Release-Date:[[:space:]][[:space:]]*//p;q}' VERSION])' '+%s')'
RELEASE_DATE_S='lockdev_m4_esyscmd_s([sed -ne '/^Release-Date:/{s/Release-Date:[[:space:]][[:space:]]*//p;q}' VERSION])'
AC_DEFINE_UNQUOTED([RELEASE_DATE_S], ["$RELEASE_DATE_S"], [Package release date (string).])
AC_DEFINE_UNQUOTED([RELEASE_DATE], [$RELEASE_DATE], [Package release date (integer).])
AC_SUBST([RELEASE_DATE])
AC_SUBST([RELEASE_DATE_S])

dnl Checks for programs.
AC_PROG_CC

dnl Checks for header files.
AC_HEADER_STDC

LT_INIT([disable-static])

AC_MSG_CHECKING([whether the library needs to call a helper])
AC_ARG_ENABLE(helper,AS_HELP_STRING([--enable-helper],[use helper binary for locking]),,enable_helper=auto)
if test x$enable_helper = xauto; then
	varlockperms=`stat -c %a /var/lock`
	if test x"$varlockperms" != x777; then
		enable_helper=yes
	fi
fi
if test x$enable_helper = xyes; then
	AC_MSG_RESULT([yes])
	AC_DEFINE_UNQUOTED([TTYLOCK_USE_HELPER], 1, [set to 1 to enable use of helper binary])
else
	AC_MSG_RESULT([no])
fi

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
