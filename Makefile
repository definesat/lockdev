# this include do nothing if nor run as a sub-make of debian/rules
-include debian/policy


libname	= liblockdev

objs	= src/lockdev.o


VER	= $(shell expr `pwd` : '.*-\([0-9.]*\)')
MVER	= ${shell expr `pwd` : '.*-\([0-9]*\).[0-9]*'}

static	= ${libname}.a
shared	= ${libname}.so.${VER}
soname	= ${libname}.so.${MVER}

# overwritten by caller (e.g.: debian/rules)
basedir	= /usr/local
srcdir=.

libdir	= ${basedir}/lib
incdir	= ${basedir}/include
mandir	= ${basedir}/man

CC	= gcc
CFLAGS	= "-g -O2 -fPIC -Wall -pipe -D_REENTRANT"

ALL:	shared static perl-lib

install:	install_dev install_dbg install_doc install_run

static ${static}:       ${objs}
	$(AR) $(ARFLAGS) ${static} $^

shared ${shared}:	${objs}
	${CC} ${CFLAGS} -shared -Wl,-soname,${soname} $^ -lc ${LIBS} -o ${shared}

perl-lib:	static
	cd LockDev && perl Makefile.PL INSTALLDIRS=perl
	cd LockDev && make
	cd LockDev && make test

install_dev:	${static} src/lockdev.h
	install -m755 -d	${libdir}
	install -m644 ${static}	${libdir}
	install -m755 -d	${incdir}
	install -m644 src/lockdev.h	${incdir}

install_debug:	${static} ${shared}
	install -m755 -d	${libdir}/debug
	install -m644 ${static}	${libdir}/debug
	install -m644 ${shared}	${libdir}/debug/${soname}

install_profile:	${static} ${shared}
	install -m755 -d	${libdir}/profile
	install -m644 ${static}	${libdir}/profile
	install -m644 ${shared}	${libdir}/profile/${soname}

install_doc:	docs/lockdev.3
	install -m755 -d	${mandir}/man3
	install -m644 docs/lockdev.3	${mandir}/man3

install_run:	${shared}
	install -m755 -d	${libdir}
	install -m644 ${shared}	${libdir}

perl-clean:	clean
	cd LockDev && rm -rf *~ *.o LockDev.bs LockDev.c \
		Makefile Makefile.old blib pm_to_blib 

clean:
	-find . -name '*~' | xargs --no-run-if-empty  rm -f 
	-find . -name '*.o' | xargs --no-run-if-empty  rm -f 

mostyclean:	clean
	-rm -f *.a *.so *.so.*
	-rm -f shared static debug profile _SRCDIR_

distclean:	mostyclean perl-clean

distribute:
	${MAKE} -f debian/rules clean
	cd .. 	&& tar -cvf - ${libname}-${VER} | gzip -9c > ${libname}_${VER}.tgz

.PHONY: install install_dev install_doc install_run clean distclean perl-clean distribute 

