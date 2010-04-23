#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <ttylock.h>

void
usage (void)
{
	fprintf(stderr, "Usage: %s [-lu] <device>\n", "lockdev");
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
			default: usage(); break;
			}
		}
		else dev = p;
	}
	i = 0;
	switch( ch ) {
	case 'l':
		i = ttylock(dev);
		break;
	case 'u':
		i = ttyunlock(dev);
		break;
	default:
		if (dev)
			i = ttylocked(dev);
		else
			    usage();
		break;
	}
	if(i < 0) {
		fprintf(stderr, "error: %d %s\n", -i, strerror(-i));
		i = -1;
	}
	exit(i);
}
