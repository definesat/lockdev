#include <stdio.h>
#include "lockdev.h"

void
usage() {
	fprintf( stderr, "Usage: sample [-lurd] <device>\n" );
	exit( -1 );
}

int debug;


int
main( int argc, char *argv[] )
{
	int i, chld;
	char *p, *dev, ch;

	ch = '\0';
	for( i = argc - 1; i > 0; i-- ) {
		p = argv[i];
		if( *p == '-' ) {
			switch( *++p ) {
			case 'l': 
			case 'u': 
			case 'r': ch = *p; break;
			case 'd':
				debug = atoi(++p);
				break;
			default: usage(); break;
			}
		}
		else dev = p;
	}
	fprintf( stderr, "option %c, device %s\n", ch, dev );
	i = 0;
	switch( ch ) {
	case 'l':
		i = dev_lock( dev);
		break;
	case 'u':
		i = dev_unlock( dev, 0);
		break;
	case 'r':
		dev_lock( dev);
		if(( chld = fork()) == 0 ) {
			sleep(5);
		}
		else {
			sleep( 1);
			if (( i = dev_relock( dev, chld)) < 0 ) {
				fprintf( stderr, "Relock failed in parent.\n" );
			}
		}
		break;
	default:
		i = dev_testlock( dev);
		break;
	}
	exit( i);
}

