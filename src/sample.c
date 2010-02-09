#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include "lockdev.h"

void
usage (void)
{
	fprintf(stderr, "Usage: %s [-lud] <device>\n", "lockdev");
	exit(-1);
}


int
main (int   argc,
      char *argv[])
{
	int i;
	char *p = NULL, *dev = NULL, ch;

	ch = '\0';
	for( i = argc - 1; i > 0; i-- ) {
		p = argv[i];
		if( *p == '-' ) {
			switch( *++p ) {
			case 'l': 
			case 'u': ch = *p; break;
			case 'd':
				liblockdev_incr_debug();
				break;
			default: usage(); break;
			}
		}
		else dev = p;
	}
	if (dev == NULL)
	    usage();
	i = 0;
	(void) dev_setpid(getppid());
	switch( ch ) {
	case 'l':
		i = dev_lock( dev);
		break;
	case 'u':
		i = dev_unlock(dev, 0);
		break;
	default:
		if (dev)
			i = dev_testlock(dev);
		break;
	}

	/*
	 * Exit		dev_lock	dev_unlock	dev_testlock
	 *	  0	OK		OK		not locked
	 *	  1	locked other	locked other	locked
	 *	  2	EACCES
	 *	  3	EROFS
	 *	  4	EFAULT
	 *	  5	EINVAL
	 *	  6	ENAMETOOLONG
	 *	  7	ENOENT
	 *	  8	ENOTDIR
	 *	  9	ENOMEM
	 *	 10	ELOOP
	 *	 11	EIO
	 *	255	error		error		error
	 */
	switch (i) {
	case -EACCES:	i = 2;	break;
	case -EROFS:	i = 3;	break;
	case -EFAULT:	i = 4;	break;
	case -EINVAL:	i = 5;	break;
	case -ENAMETOOLONG:	i = 6;	break;
	case -ENOENT:	i = 7;	break;
	case -ENOTDIR:	i = 8;	break;
	case -ENOMEM:	i = 9;	break;
	case -ELOOP:	i = 10;	break;
	case -EIO:	i = 11;	break;
	default:
	    if (i < 0) i = 255;
	    else if (i > 0) 	i = 1;
	    break;
	}
	exit(i);
}
