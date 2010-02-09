-include Rules.mk

libname	= liblockdev
pkgname = lockdev

objs	= src/lockdev.o

lockdev	= src/sample.c

VER	= 1.0.3
MVER	= 1

static	= ${libname}.a
shared	= ${libname}.so.${VER}
soname	= ${libname}.so.${MVER}

# overwritten by caller (e.g.: debian/rules)
basedir	= /usr/local
srcdir=.

sbindir	= ${basedir}/sbin
libdir	= ${basedir}/lib
incdir	= ${basedir}/include
mandir	= ${basedir}/share/man

CC	= gcc
CFLAGS	= -g -O2 -Wall -pipe -fPIC 
LCFLAGS	= ${CFLAGS} -D_REENTRANT
LDLIBS	= -llockdev

.PHONY: shared static perl-lib
ALL:	shared static lockdev perl-lib

static ${static}:       ${objs}
	$(AR) $(ARFLAGS) ${static} $^

shared ${shared}:	${objs}
	${CC} ${LCFLAGS} -shared -Wl,-soname,${soname} $^ -lc -o ${shared}

lockdev.o: ${lockdev}
	${CC} ${CFLAGS} -I./src -o $@ -c $^

lockdev: lockdev.o ${static}
	${CC} -o $@ $^

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
	install -m644 src/baudboy.h	${incdir}

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
	install -m755 -d	${mandir}/man8
	install -m644 docs/lockdev.3	${mandir}/man3
	install -m644 docs/lockdev.8	${mandir}/man8

install_run:	${shared}
	install -m755 -d	${libdir}
	install -m755 ${shared}	${libdir}
	ln -s ${shared}		${libdir}/liblockdev.so
	install -m755 -d	${sbindir}
	install -m755 lockdev	${sbindir}

.PHONY: clean distclean perl-clean mostyclean 
perl-clean:	clean
	cd LockDev && rm -rf *~ *.o LockDev.bs LockDev.c \
		Makefile Makefile.old blib pm_to_blib 

clean:
	-find . -name '*~' | xargs --no-run-if-empty  rm -f 
	-find . -name '*.o' | xargs --no-run-if-empty  rm -f 
	-find . -name '*.z' | xargs --no-run-if-empty  rm -f 

mostyclean:	clean
	-rm -f *.a *.so *.so.*
	-rm -f shared static debug profile _SRCDIR_

distclean:	mostyclean perl-clean

.PHONY: distribute dist tarball
dist distribute:	tarball
tarball: distclean
	cd .. 	&& tar -cvf - ${pkgname}-${VER} | gzip -9c > ${pkgname}_${VER}.tgz
