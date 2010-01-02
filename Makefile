-include Rules.mk

libname	= liblockdev
pkgname = lockdev

objs	= src/lockdev.o


VER	= $(shell expr `pwd` : '.*-\([0-9.]*\)')
MVER	= ${shell expr `pwd` : '.*-\([0-9]*\).[0-9]*'}

static	= ${libname}.a
shared	= ${libname}.${VER}.so
soname	= ${libname}.so.${MVER}

# overwritten by caller (e.g.: debian/rules)
basedir	= /usr/local
srcdir=.

libdir	= ${basedir}/lib
incdir	= ${basedir}/include
mandir	= ${basedir}/share/man

CC	= gcc
LCFLAGS	= -g -O2 -fPIC -Wall -pipe -D_REENTRANT 
CFLAGS	= -g
LDLIBS	= -llockdev

.PHONY: shared static perl-lib
ALL:	shared static perl-lib

static ${static}:       ${objs}
	$(AR) $(ARFLAGS) ${static} $^

shared ${shared}:	${objs}
	${CC} ${LCFLAGS} -shared -Wl,-soname,${soname} $^ -lc -o ${shared}


perl-lib:	static
	cd LockDev && perl Makefile.PL INSTALLDIRS=vendor
	cd LockDev && make OPTIMIZE="-O2 -g -Wall"
	cd LockDev && make test

.PHONY: install install_dev install_dbg install_doc install_run 
install:	install_dev install_dbg install_doc install_run

install_dev:	${static} src/lockdev.h
	install -m755 -d	${libdir}
	install -m644 ${static}	${libdir}
	install -m755 -d	${incdir}
	install -m644 src/lockdev.h	${incdir}
	install -m644 src/ttylock.h	${incdir}

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

.PHONY: clean distclean perl-clean mostyclean 
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

.PHONY: distribute dist tarball
dist distribute:	tarball
tarball: distclean
	cd .. 	&& tar -cvf - ${pkgname}-${VER} --exclude='${pkgname}-${VER}/debian' | gzip -9c > ${pkgname}_${VER}.tgz
