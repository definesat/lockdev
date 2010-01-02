/*
 *	lockdev.c
 *	(c) 1997 by Fabrizio Polacco <fpolacco@debian.org>
 *	this source program is part of the liblockdev library.
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU Library General Public
 *	License as published by the Free Software Foundation; version 2
 *	dated June, 1991.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU Library General
 *	Public License along with this program;  if not, write to the
 *	Free Software Foundation, Inc., 675 Mass Ave., Cambridge, MA
 *	02139, USA.
 *
 *
 *	This library provides a stable way to lock devices on a Linux or
 *	Unix system, both following FSSTND 1.2 and SVr4 device locking
 *	methods, and it's built to be portable and to avoid conflicts
 *	with other programs.
 *	It is highly reccommended that all programs that needs to lock a
 *	device (or test if it is locked, call the functions defined in
 *	the public interface of this library.
 *
 *	To use the library on other Operating Systems, with different
 *	lock file name conventions, simply add the proper sprintf in the
 *	conditional statement at the beginning of this source.
 *
 *	pid_t	is_dev_lock( const char * devname)
 *
 *		Given a device name returns zero if the device is not
 *		locked, otherways returns the pid number ( > 0 ) of the
 *		owner of the lock. The owner process is checked for
 *		existance. Stale locks are removed. ( -1 ) is returned
 *		on error (device not exist or has wrong name).
 *
 *	pid_t	lock_dev( const char * devname)
 *		First it makes a very simple check that the device
 *		filename isn't locked, then checks for the lockfile SVr4
 *		style (these checks are redundant, just to avoid the
 *		rest in case the device was locked properly by another
 *		process.
 *		Then it creates a lock file with a surely unused name,
 *		using the pid number, then it tryes to acquire the lock
 *		doing hardlinks, which are atomic and don't overwrite
 *		existing files.
 *		The lock is considered acquired if both links are
 *		established. Then the function returns zero to mean that
 *		this process owns the lock. Otherways it returns the pid
 *		of the active process that owns the lock or -1 for some
 *		kind of error.
 *
 *	pid_t	relock_dev( const char * devname, const pid_t old_pid)
 *		checks that the locks exist and is owned by the old_pid
 *		(if non zero), and then rewrites the pid in the files,
 *		thus reassigning the ownership of the lock to the
 *		current process, usually a child of the original one.
 *
 *	pid_t	unlock_dev( const char * devname, const pid_t pid)
 *		Given a device name it checks that the lock files be
 *		owned by the process in argument (if non-zero), then
 *		deletes them. Lock files are anyway removed unless they
 *		are owned by a different existing process whose pid is
 *		returned; a null value is returned after the successfull
 *		removal of both files, otherways a -1 value is returned
 *		to indicate an error. The order in which they are
 *		deleted is important, and is the reverse of the order in
 *		which they are created in lock_dev() .
 *
 *	All these functions use a semaphore (a file named LOCKDEV in the
 *	defined lock directory) locked using flock( LOCK_EX) on the
 *	beginning of the operations and released when returning to the
 *	caller.
 *	A macro insures that the semaphore is released before each
 *	return to the caller (thus the releasing function may be called
 *	several times if nested calls were involved. It is obvious why
 *	we use a macro and not an inline function (return isn't a
 *	function).
 *
 *	Two functions and a global int are provided for debugging
 *	purposis: when compiling with -DDEBUG some printf are added in
 *	the code to help debug the program. A verbosity value (1 by
 *	default) is stored in a global int that can be incremented
 *	calling liblockdev_incr_debug() and set to zero calling
 *	liblockdev_reset_debug() . The value used are only three: 1 (the
 *	default) is for error conditions and semaphore status, 2 is for
 *	normal conditions checks, and 3 is to check all entries in a
 *	function.
 *	If the library is compiled without _DDEBUG, the global int is
 *	not used in any part, but the simbols still exists so it is
 *	possible to use them safely in your code.
 *
 */

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <paths.h>
#ifndef _PATH_LOCK
#  define LOCK_PATH	"/var/lock"
#else
#  define LOCK_PATH	_PATH_LOCK
#endif
#ifndef _PATH_DEV
#  define DEV_PATH	"/dev"
#else
#  define DEV_PATH	_PATH_DEV
#endif
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <linux/kdev_t.h>

#include "lockdev.h"

/*
 *	PROTOTYPES for internal functions
 *	all internal functions names start with _dl_
 */

static inline	int	_dl_filename_0	( char * name, const pid_t pid);
static inline	int	_dl_filename_1	( char * name, const struct stat * st);
static inline	int	_dl_filename_2	( char * name, const char * dev);
static		void	_dl_sig_handler	( int sig);
static		int	_dl_get_semaphore	( int flag);
static		int	_dl_unlock_semaphore	( int value);
static inline	int	_dl_lock_semaphore	( void);
static inline	int	_dl_block_semaphore	( void);
static		pid_t	_dl_check_lock	( const char * lockname);
static const	char *	_dl_check_devname	( const char * devname);


#define	SEMAPHORE	"LOCKDEV"
#define	close_n_return( v)	return( _dl_unlock_semaphore( v))

#if DEBUG
static	int	liblockdev_debug = DEBUG;	/* 1 by default */
#	define	_debug(val,fmt,arg...)	(liblockdev_debug && (liblockdev_debug >= val) && printf(fmt,##arg))
static	int	env_var_debug = -1;		/* record set from env var */
#	define	_env_var	"LIBLOCKDEV_DEBUG"
#else
static	int	liblockdev_debug = 0;
#	define	_debug(val,fmt,arg...)	do { } while (0)
#endif

/* exported by the interface file lockdev.h */
void
liblockdev_incr_debug()
{
	liblockdev_debug++;
}

/* exported by the interface file lockdev.h */
void
liblockdev_reset_debug()
{
	liblockdev_debug = 0;
}


/*
 * for internal use *
 *
 * inline functions that builds the three different filenames that are
 * used by these routines:
 * type 0 is the name containing the process ID
 * type 1 is the name build using the major and minor numbers (as in SVr4)
 * type 2 is the name containing the device name (as in FSSTND)
 * All types uses the macro LOCK_PATH
 */

#if defined (__linux__)

/* for internal use */
static inline int
_dl_filename_0( name, pid)
	char * name;
	const pid_t pid;
{
	int l;
	_debug( 3, "_dl_filename_0 (pid=%d)\n", (int)pid);
	/* file of type /var/lock/LCK..<pid> */
	l = sprintf( name, "%s/LCK..%d", LOCK_PATH, (int)pid);
	_debug( 2, "_dl_filename_0 () -> len=%d, name=%s<\n", l, name);
	return l;
}

/* for internal use */
static inline int
_dl_filename_1( name, st)
	char * name;
	const struct stat * st;
{
	int l;
	_debug( 3, "_dl_filename_1 (stat=%d)\n", (int)st->st_rdev);
	/* lockfile of type /var/lock/LCK.004.064 */
	l = sprintf( name, "%s/LCK.%03d.%03d", LOCK_PATH,
		(int)MAJOR( st->st_rdev), (int)MINOR( st->st_rdev));
	_debug( 2, "_dl_filename_1 () -> len=%d, name=%s<\n", l, name);
	return l;
}

/* for internal use */
static inline int
_dl_filename_2( name, dev)
	char * name;
	const char * dev;
{
	int l;
	_debug( 3, "_dl_filename_2 (dev=%s)\n", dev);
	/* lockfile of type /var/lock/LCK..ttyS2 */
	l = sprintf( name, "%s/LCK..%s", LOCK_PATH, dev);
	_debug( 2, "_dl_filename_2 () -> len=%d, name=%s<\n", l, name);
	return l;
}

#else
#  error "lock filename build missing"
#endif

/* handler for signals */
static void
_dl_sig_handler( sig)
	int sig;
{
	signal( sig, _dl_sig_handler);
	switch( sig ) {
#if DEBUG
	case SIGUSR1:
		liblockdev_debug++;
		break;
	case SIGUSR2:
		liblockdev_debug = 0;
		break;
#endif
	default:
		break;
	}
}

/* holds the file descriptor of the lock between the various *_semaphore function calls.
 * its positive conten doesn't tells that we lock the file.
 */
static int semaphore= -1;
static char sem_name[MAXPATHLEN+1];
static int oldmask= -1;

/* for internal use */
static int
_dl_get_semaphore( flag)
	int flag;
{
	int flag2 = flag;

	sprintf( sem_name, "%s/%s", LOCK_PATH, SEMAPHORE);
	_debug( 1, "_dl_get_semaphore(name=%s)=%d\n", sem_name, semaphore);
	while ( semaphore == -1 ) {
		semaphore = creat( sem_name, (S_IRWXU | S_IRWXG | S_IRWXO));
		if ( semaphore == -1 ) {
			if ( errno == EAGAIN ) {
				if ( flag-- ) {
					sleep( 1);
					continue;
				}
				return EAGAIN;
			}
			return -1;
		}
	}
	while ( flock( semaphore, LOCK_EX | LOCK_NB) < 0 ) {
		if ( errno == EWOULDBLOCK ) {
			if ( flag2-- ) {
				sleep( 1);
				continue;
			}
			return EWOULDBLOCK;
		}
		return -1;
	}
	return 0;	/* file locked */
}

/* for internal use */
static int
_dl_unlock_semaphore( value)
	int value;
{
	if ( semaphore != -1 ) {
		_debug( 1, "_dl_unlock_semaphore(return=%d)=%d\n", value, semaphore);
		unlink( sem_name);
		close( semaphore);
		semaphore = -1;
	}
	if (oldmask != -1 ) {
		umask( oldmask);	/* restore original mask value */
		oldmask = -1;
	}
	return( value);
}

/* for internal use */
static inline int
_dl_lock_semaphore()
{
	return  _dl_get_semaphore( 0);
}

/* for internal use */
static inline int
_dl_block_semaphore()
{
	return  _dl_get_semaphore( 3);
}

/* for internal use */
/* zero means: we don't own the lock; maybe someone else */
static pid_t
_dl_check_lock( lockname)
	const char * lockname;
{
	/* no check on lockname */
	pid_t pid = 0;
	FILE *fd = 0;
	int j = 0;

	_debug( 3, "_dl_check_lock(lockname=%s)\n", lockname);
	if ( _dl_block_semaphore() ) {
		return -1;
	}
	if ( ! (fd=fopen( lockname, "r")) ) {
		if ( errno == ENOENT ) {
			_debug( 2, "_dl_check_lock() no lock\n");
			return 0;	/* no file, no lock */
		} else {
			_debug( 1, "_dl_check_lock() fopen error %d\n", errno);
			return -1;
		}
	}
	j = fscanf( fd, "%d", &pid);
	fclose( fd);
	_debug( 2, "_dl_check_lock() read %d value = %d\n", j, pid);

	/* checks content's format */
	if ( j == 1 ) {
		/* checks process existence */
		if ( kill( pid, 0) == 0 ) {
			_debug( 2, "_dl_check_lock() locked by %d\n", (int)pid);
			return pid;
		}
	}
	/* wrong format or no process */
	_debug( 2, "_dl_check_lock() removing stale lock by %d\n", (int)pid);

	/* We own a semaphore lock, so ALL the processes that uses this
	 * library are waiting for us to complete, or are exiting.
	 * But in the world there are several programs that uses only
	 * the "standard" locking method dictated by FSSTND, which
	 * permits to a third process to step in, acquire a lock that is
	 * unlinked by the running process, and therefore fall in a
	 * state where two processes think they own the lock on the same
	 * device.
	 * Therefore, instead of simply unlinking, we rename the
	 * file to a safe name (one with the process id, but different
	 * from that used to create the lock, and we unlink it before,
	 * just for safety); (AFAIK) rename is atomic if the
	 * newpath doesn't exist. Then we check the content again. If it
	 * wasn't changed then we can unlink safely, otherways we should
	 * quickly link it back to the old name (not rename because of
	 * the overwriting).
	 * This is a curtesy we use to a process that uses ths FSSTND
	 * standard, which present natively this possibility.
	 * Our courtesy isn't fully secure: a fourth process could step
	 * in and compete the lock with the "third" one. Both these
	 * processes don't use this library and what happens then is
	 * already possible to happen when we are not running, so ...
	 * ... why should we care more about this?
	 */
	{
		char tpname[MAXPATHLEN+1];
		pid_t pid2 = 0;

		/* make a unique filename, but different from anyone else.
		 * maybe also this sprintf should be added to the
		 * conditional part, as the others
		 */
		sprintf( tpname, "%s/.%d", LOCK_PATH, (int)getpid());
		unlink( tpname);	/* in case there was */
		rename( lockname, tpname);
		if ( ! (fd=fopen( tpname, "r")) ) {
			return -1;
		}
		fscanf( fd, "%d", &pid2);
		if ( pid2 && (pid2 != pid) && ( kill( pid2, 0) == 0 )) {
			/* lock file was changed! let us quickly
			 * put it back again
			 */
			link( tpname, lockname);
			/* could fail, meaning that there is a fourth
			 * process involved which now owns the lock, but
			 * it is possible that the third process, who
			 * acquired the lock during the race condition,
			 * still thinks it have the lock.
			 * We leave them go lost in their way!
			 */
			_debug( 2, "_dl_check_lock() locked by %d\n", (int)pid2);
			fclose( fd);
			unlink( tpname);
			return( pid2);
		}
		fclose( fd);
		unlink( tpname);
	}
	_debug( 2, "_dl_check_lock() no lock\n");
	return 0;
}

/* for internal use */
static const char *
_dl_check_devname( devname)
	const char * devname;
{
	int l;
	const char * p;

	/* devname can be absolute, relative to PWD or a single
	 * filename, in any case we assume that the file is in /dev;
	 * maybe we should check it and do something if not?
	 */
	if ( ! (p=strrchr( devname, '/')))
		p = devname;	/* only a filename */
	else
		p++;	/* was pointing to the slash */
	_debug( 3, "_dl_check_devname(%s) name = %s\n", devname, p);
	if ( ((l=strlen( p)) == 0 ) || ( l > (MAXPATHLEN - strlen(LOCK_PATH)) ))
	 	return 0;
	return p;
}


/* exported by the interface file lockdev.h */
/* ZERO means that the device wasn't locked, but could have been locked later */
pid_t
is_dev_lock( devname)
	const char * devname;
{
	const char * p;
	char device[MAXPATHLEN+1];
	char lock[MAXPATHLEN+1];
	struct stat statbuf;
	pid_t pid;

#if DEBUG
	if ( env_var_debug == -1 ) {
		char *value;
		if ( value=getenv( _env_var ) )
			env_var_debug = liblockdev_debug = atoi( value);
		signal( SIGUSR1, _dl_sig_handler);
		signal( SIGUSR2, _dl_sig_handler);
	}
#endif /* DEBUG */
	_debug( 3, "is_dev_locked(%s)\n", devname);
	if ( ! (p=_dl_check_devname( devname)) )
	 	close_n_return( -1);
	strcpy( device, DEV_PATH);
	strcat( device, p);	/* now device has a copy of the pathname */
	_debug( 2, "is_dev_locked() device = %s\n", device);

	/* check the device name for existence and retrieve the major
	 * and minor numbers
	 */
	if ( stat( device, &statbuf) == -1 ) {
		close_n_return( -1);
	}

	/* first check for the FSSTND-1.2 lock, get the pid of the
	 * owner of the lock and test for its existence; in case,
	 * return the pid of the owner of the lock.
	 */
	/* lockfile of type /var/lock/LCK..ttyS2 */
	_dl_filename_2( lock, p);
	if ( (pid=_dl_check_lock( lock)) )
		close_n_return( pid);

	/* do the same with the new style lock file name (a la SVr4);
	 * the reason for this order is that is much more probable that
	 * another program uses the FSSTND lock without the mew one than
	 * the contrary; anyway we do both tests.
	 */
	/* lockfile of type /var/lock/LCK.004.064 */
	_dl_filename_1( lock, &statbuf);
	if ( (pid=_dl_check_lock( lock)) )
		close_n_return( pid);
	close_n_return( 0);
}

/* exported by the interface file lockdev.h */
pid_t
lock_dev( devname)
	const char * devname;
{
	const char * p;
	char device[MAXPATHLEN+1];
	char lock0[MAXPATHLEN+1];
	char lock1[MAXPATHLEN+1];
	char lock2[MAXPATHLEN+1];
	struct stat statbuf;
	pid_t pid, pid2, our_pid;
	FILE *fd = 0;

#if DEBUG
	if ( env_var_debug == -1 ) {
		char *value;
		if ( value=getenv( _env_var ) )
			env_var_debug = liblockdev_debug = atoi( value);
		signal( SIGUSR1, _dl_sig_handler);
		signal( SIGUSR2, _dl_sig_handler);
	}
#endif /* DEBUG */
	_debug( 3, "lock_dev(%s)\n", devname);
	if (oldmask == -1 )
		oldmask = umask( 0);	/* give full permissions to files created */
	if ( ! (p=_dl_check_devname( devname)) )
	 	close_n_return( -1);
	strcpy( device, DEV_PATH);
	strcat( device, p);	/* now device has a copy of the pathname */
	_debug( 2, "lock_dev() device = %s\n", device);

	/* check the device name for existence and retrieve the major
	 * and minor numbers
	 */
	if ( stat( device, &statbuf) == -1 ) {
		close_n_return( -1);
	}

	/* now get our own pid */
	our_pid = getpid();
	_debug( 2, "lock_dev() our own pid = %d\n", (int)our_pid);

	/* We will use this algorithm:
	 * first we build a file using the pid in the name (garanteed to
	 * be unique), then we try to link to the lockname (atomic
	 * operation which doesn't overwrite existing files). If we
	 * succeed then we link it to the other lockname. Only when both
	 * this operations succeed we own the lock.
	 */
	/* file of type /var/lock/LCK..<pid> */
	_dl_filename_0( lock0, our_pid);
	if ( ! (fd=fopen( lock0, "w")) )
		close_n_return( -1);	/* no file, no lock */
	fprintf( fd, "%10d\n", (int)our_pid);
	fclose( fd);

	/* first check for the FSSTND-1.2 lock, get the pid of the
	 * owner of the lock and test for its existence; in case,
	 * return the pid of the owner of the lock.
	 */
	/* lockfile of type /var/lock/LCK..ttyS2 */
	_dl_filename_2( lock2, p);
	pid = _dl_check_lock( lock2);
	if ( pid && pid != our_pid ) {
		unlink( lock0);
		close_n_return( pid);	/* error or locked by someone else */
	}
	/* not locked or already owned by us */

	/* test the lock and try to lock; repeat untill an error or a
	 * lock happens
	 */
	/* lockfile of type /var/lock/LCK.004.064 */
	_dl_filename_1( lock1, &statbuf);
	while ( ! (pid=_dl_check_lock( lock1)) ) {
		if (( link( lock0, lock1) == -1 ) && ( errno != EEXIST )) {
			unlink( lock0);
			close_n_return( -1);
		}
	}
	if ( pid != our_pid ) {
		/* error or another one owns it now */
		unlink( lock0);
		close_n_return( pid);
	}
	/* from this point lock1 is OUR! */

	/* lockfile of type /var/lock/LCK..ttyS2 */
	while ( ! (pid=_dl_check_lock( lock2)) ) {
		if (( link( lock0, lock2) == -1 ) && ( errno != EEXIST )) {
			unlink( lock0);
			unlink( lock1);
			close_n_return( -1);
		}
	}
	if ( pid != our_pid ) {
		/* error or another one owns it now */
		/* this is more probable here, because of the existance
		 * of processes who uses broken lock from FSSTND.
		 * We let them win.
		 */
		unlink( lock0);
		unlink( lock1);
		close_n_return( pid);
	}

	/* OK, the lock is our! now remove the pid-file */
	unlink( lock0);
	/* Paranoid mode on: are we sure we still own the lock? There is
	 * a race condition in removing stale locks that could have let
	 * a fourth process acquire the lock that we beleave is our.
	 * So simply check again for the lock should let us know if we
	 * were so much unlucky.
	 */
	pid = _dl_check_lock( lock1);
	pid2 = _dl_check_lock( lock2);
	if (( pid == pid2 ) && ( pid == our_pid )) {
		_debug( 2, "lock_dev() got lock\n");
		close_n_return( 0);	/* locked by us */
	}
	/* oh, no! someone else stepped in! */
	if ( pid == our_pid ) {
		unlink( lock1);
		pid = 0;
	}
	if ( pid2 == our_pid ) {
		unlink( lock2);
		pid2 = 0;
	}
	if ( pid && pid2 ) {
		/* two different processes owns the two files: bleah! */
		_debug( 1, "lock_dev() process %d owns file %s\n", (int)pid, lock1);
		_debug( 1, "lock_dev() process %d owns file %s\n", (int)pid2, lock2);
		_debug( 1, "lock_dev() process %d (we) have no lock!\n", (int)our_pid);
		close_n_return( -1);
	}
	close_n_return( (pid + pid2));
}


/* exported by the interface file lockdev.h */
pid_t
relock_dev( devname, old_pid)
	const char * devname;
	const pid_t old_pid;
{
	const char * p;
	char device[MAXPATHLEN+1];
	char lock1[MAXPATHLEN+1];
	char lock2[MAXPATHLEN+1];
	struct stat statbuf;
	pid_t pid, our_pid;
	FILE *fd = 0;

#if DEBUG
	if ( env_var_debug == -1 ) {
		char *value;
		if ( value=getenv( _env_var ) )
			env_var_debug = liblockdev_debug = atoi( value);
		signal( SIGUSR1, _dl_sig_handler);
		signal( SIGUSR2, _dl_sig_handler);
	}
#endif /* DEBUG */
	_debug( 3, "relock_dev(%s, %d)\n", devname, (int)old_pid);
	if (oldmask == -1 )
		oldmask = umask( 0);	/* give full permissions to files created */
	if ( ! (p=_dl_check_devname( devname)) )
	 	close_n_return( -1);
	strcpy( device, DEV_PATH);
	strcat( device, p);	/* now device has a copy of the pathname */
	_debug( 2, "relock_dev() device = %s\n", device);

	/* check the device name for existence and retrieve the major
	 * and minor numbers
	 */
	if ( stat( device, &statbuf) == -1 ) {
		close_n_return( -1);
	}

	/* now get our own pid */
	our_pid = getpid();
	_debug( 2, "relock_dev() our own pid = %d\n", (int)our_pid);

	/* first check for the FSSTND-1.2 lock, get the pid of the
	 * owner of the lock and test for its existence; in case,
	 * return the pid of the owner of the lock.
	 */
	/* lockfile of type /var/lock/LCK..ttyS2 */
	_dl_filename_2( lock2, p);
	pid = _dl_check_lock( lock2);
	if ( pid && old_pid && pid != old_pid )
		close_n_return( pid);	/* error or locked by someone else */

	/* lockfile of type /var/lock/LCK.004.064 */
	_dl_filename_1( lock1, &statbuf);
	pid = _dl_check_lock( lock1);
	if ( pid && old_pid && pid != old_pid )
		close_n_return( pid);	/* error or locked by someone else */

	if ( ! pid ) {	/* not locked ??? */
		/* go and lock it */
		return( lock_dev( devname));
	}

	/* we don't rewrite the pids in the lock files untill we're sure
	 * we own all the lockfiles
	 */
	if ( ! (fd=fopen( lock1, "w")) )
		close_n_return( -1);	/* something strange */
	fprintf( fd, "%10d\n", (int)our_pid);
	fclose( fd);
	/* under normal conditions, this second file is a hardlink of
	 * the first, so we have yet modifyed it also, and this write
	 * seems redundant ... but ... doesn't hurt.
	 */
	if ( ! (fd=fopen( lock2, "w")) )
		/* potentially a problem */
		close_n_return( -1);	/* something strange */
	fprintf( fd, "%10d\n", (int)our_pid);
	fclose( fd);

	_debug( 2, "relock_dev() lock changed\n");
	close_n_return( 0);	/* locked by us */
}


/* exported by the interface file lockdev.h */
pid_t
unlock_dev( devname, pid)
	const char * devname;
	const pid_t pid;
{
	const char * p;
	char device[MAXPATHLEN+1];
	char lock1[MAXPATHLEN+1];
	char lock2[MAXPATHLEN+1];
	struct stat statbuf;
	pid_t wpid;

#if DEBUG
	if ( env_var_debug == -1 ) {
		char *value;
		if ( value=getenv( _env_var ) )
			env_var_debug = liblockdev_debug = atoi( value);
		signal( SIGUSR1, _dl_sig_handler);
		signal( SIGUSR2, _dl_sig_handler);
	}
#endif /* DEBUG */
	_debug( 3, "unlock_dev(%s, %d)\n", devname, (int)pid);
	if (oldmask == -1 )
		oldmask = umask( 0);	/* give full permissions to files created */
	if ( ! (p=_dl_check_devname( devname)) )
	 	close_n_return( -1);
	strcpy( device, DEV_PATH);
	strcat( device, p);	/* now device has a copy of the pathname */
	_debug( 2, "unlock_dev() device = %s\n", device);

	/* check the device name for existence and retrieve the major
	 * and minor numbers
	 */
	if ( stat( device, &statbuf) == -1 ) {
		close_n_return( -1);
	}

	/* first remove the FSSTND-1.2 lock, get the pid of the
	 * owner of the lock and test for its existence; in case,
	 * return the pid of the owner of the lock.
	 */
	/* lockfile of type /var/lock/LCK..ttyS2 */
	_dl_filename_2( lock2, p);
	wpid = _dl_check_lock( lock2);
	if ( pid && wpid && pid != wpid )
		close_n_return( wpid);	/* error or locked by someone else */

	/* lockfile of type /var/lock/LCK.004.064 */
	_dl_filename_1( lock1, &statbuf);
	wpid = _dl_check_lock( lock1);
	if ( pid && wpid && pid != wpid )
		close_n_return( wpid);	/* error or locked by someone else */

	/* anyway now we remove the files, in the reversed order than
	 * they have been built.
	 */
	unlink( lock2);
	unlink( lock1);
	_debug( 2, "unlock_dev() unlocked\n");
	close_n_return( 0);	/* successfully unlocked */
}

