#ifdef __cplusplus
extern "C" {
#endif
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
#ifdef __cplusplus
}
#endif

#include "lockdev.h"

static int
not_here(s)
char *s;
{
    croak("%s not implemented on this architecture", s);
    return -1;
}

static double
constant(name, arg)
char *name;
int arg;
{
    errno = 0;
    switch (*name) {
    }
    errno = EINVAL;
    return 0;

not_there:
    errno = ENOENT;
    return 0;
}


MODULE = LockDev		PACKAGE = LockDev		


double
constant(name,arg)
	char *		name
	int		arg

pid_t
is_dev_lock( devname)
	const char * devname;
    OUTPUT:
	RETVAL

pid_t
lock_dev ( devname)
	const char * devname;
    OUTPUT:
	RETVAL

pid_t
relock_dev ( devname, old_pid)
	const char * devname;
	const pid_t old_pid;
    OUTPUT:
	RETVAL

pid_t
unlock_dev ( devname, pid)
	const char * devname;
	const pid_t pid;
    OUTPUT:
	RETVAL

