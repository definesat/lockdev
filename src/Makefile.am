sbin_PROGRAMS = lockdev
noinst_PROGRAMS = baudboy

include_HEADERS = lockdev.h baudboy.h ttylock.h

AM_CPPFLAGS = -include $(top_builddir)/config.h

lockdev_SOURCES = sample.c
lockdev_LDADD = liblockdev.la

baudboy_SOURCES = baudboy_test.c
baudboy_LDADD = liblockdev.la

lib_LTLIBRARIES = liblockdev.la
liblockdev_la_SOURCES = lockdev.c

liblockdev_la_LDFLAGS = -version-info $(LOCKDEV_CURRENT_INTERFACE):$(LOCKDEV_INTERFACE_AGE):$(LOCKDEV_BINARY_AGE)
