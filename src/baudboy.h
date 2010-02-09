/* Copyright (C) 2001 Red Hat, Inc.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License
   as published by the Free Software Foundation; either version 2 of
   the License, or (at your option) any later version.

   It is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General
   Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this software; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */


#ifndef _BAUDBOY_H_
#define _BAUDBOY_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

#ifdef	__cplusplus
extern "C" {
#endif

#define	LOCKDEV_PATH	"/usr/sbin/lockdev"

static inline int _doit(const char * argv[])
{
    pid_t child;
    int status;
    void (*osig) (int) = signal(SIGCHLD, SIG_DFL);
    int rc;

    if (!(child = fork())) {
	int fd;
        /* these have to be open to something */
	if ((fd = open("/dev/null", 2)) < 0)
	    exit(-1);
	dup2(fd, 0);
	dup2(fd, 1);
	dup2(fd, 2);
	close(fd);
	/* Swap egid and gid for lockdev's access(2) device check. */
	setregid(getegid(), getgid());
	execv(argv[0], (char *const *)argv);
	exit(-1);
    }

    rc = (int) waitpid(child, &status, 0);
    signal(SIGCHLD, osig);
    if (rc == child && WIFEXITED(status)) {
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
	rc = WEXITSTATUS(status);
	switch(rc) {
	case  0:	rc = 0;		break;
	default:
	case  1:	rc = -EPERM;	break;
	case  2:	rc = -EACCES;	break;
	case  3:	rc = -EROFS;	break;
	case  4:	rc = -EFAULT;	break;
	case  5:	rc = -EINVAL;	break;
	case  6:	rc = -ENAMETOOLONG;	break;
	case  7:	rc = -ENOENT;	break;
	case  8:	rc = -ENOTDIR;	break;
	case  9:	rc = -ENOMEM;	break;
	case 10:	rc = -ELOOP;	break;
	case 11:	rc = -EIO;	break;
	}
    } else if (rc == -1)
	rc = -errno;
    else
	rc = -ECHILD;

    return rc;

}

static inline int ttylock(const char * devname)
{
    const char * argv[] = { LOCKDEV_PATH, "-l", NULL, NULL};
    argv[2] = devname;
    return _doit(argv);
}

static inline int ttyunlock(const char * devname)
{
    const char * argv[] = { LOCKDEV_PATH, "-u", NULL, NULL};
    argv[2] = devname;
    return _doit(argv);
}

static inline int ttylocked(const char * devname)
{
    const char * argv[] = { LOCKDEV_PATH, NULL, NULL};
    argv[1] = devname;
    return _doit(argv);
}

static inline int ttywait(const char * devname)
{
    int rc;
    while((rc = ttylocked(devname)) == 0)
	sleep(1);
    return rc;
}

#ifdef	__cplusplus
};
#endif

#endif /* _BAUDBOY_H_ */
