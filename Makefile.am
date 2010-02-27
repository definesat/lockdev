SUBDIRS = src docs

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = lockdev.pc

dist-hook:
# Remove junk from the generated tarball.
	$(FIND) $(distdir) -name '.*' -print | $(XARGS) xargs $(RM) -rf
	$(FIND) $(distdir) -name '#*' -print | $(XARGS) $(RM) -rf
	$(FIND) $(distdir) -name '*~' -print | $(XARGS) $(RM) -rf

# Generate ChangeLog
	if [ -d .git ]; then			\
	  git log > $(distdir)/ChangeLog;	\
	fi

EXTRA_DIST =		\
	ChangeLog.old	\
	SHLIBS		\
	VERSION
