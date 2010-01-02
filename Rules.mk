### this fragment of makefile _must_ be included in the makefile
### and this variable ( create_debug_lib ) must be passed to the 
### sub-make to activate this rule. Do this in the debug building 

ifdef create_debug_lib

_SRCDIR_:
	ln -sf . $@

### How does this work?
### a little explanation:
### the build stage (and all the binary) depends from a file not phony
### named _SRCDIR_ which is created in the current working dir as a 
### symlink to . (dot, the current dir), and removed in the clean stage.
### We overwrite the implicit rule to make objects from .c sources,
### producing the asm source file from the .c source seen through the
### _SRCDIR_ symlink. In the rule we change that strange name with the
### absolute pathname where we will store the sources, so that the
### object (and the libraries) will record that name instead that the 
### actual one.
### The purpose is to have a default link from the libraries to the
### place where the debugging binary package will put the sources, so that
### gdb will diplay source lines without _any_ need of strange declarations.

define compile.c
	${COMPILE.c} -S _SRCDIR_/$< -o $*.ss
endef
define compile.cc
	${COMPILE.cc} -S _SRCDIR_/$< -o $*.ss
endef
define the_rest
	sed 's,^.stabs \"_SRCDIR_\/,.stabs \"${abs_src}/,' $*.ss > $*.s
	${COMPILE.s} -o $@ $*.s
	${RM} $*.ss $*.s
endef

# overwriting implicit rule 
%.o: %.c _SRCDIR_
	@echo "=== overwritten rule .o:.c ($@: $^) ==="
	${compile.c}
	${the_rest}

# overwriting implicit rule 
%.o: %.cc _SRCDIR_
	@echo "=== overwritten rule .o:.cc ($@: $^) ==="
	${compile.cc}
	${the_rest}

endif # create_debug_lib

###############################################################################
