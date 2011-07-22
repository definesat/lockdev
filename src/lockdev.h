/*
 *	lockdev.h
 *	(c) 1997, 1999 by Fabrizio Polacco <fpolacco@debian.org>
 *	this source program is part of the liblockdev library.
 *
 *	This library is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU Lesser General Public
 *	License as published by the Free Software Foundation; either
 *	version 2 of the License, or (at your option) any later version.
 *
 *	This library is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *	Lesser General Public License for more details.
 *
 *	You should have received a copy of the GNU Lesser General Public
 *	License along with this library; if not, write to the Free Software
 *	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *
 *	This library provides a stable way to lock devices on a Linux or
 *	Unix system, both following FSSTND 1.2 and SVr4 device locking
 *	methods, and it's built to be portable and to avoid conflicts
 *	with other programs.
 *	It is highly reccommended that all programs that needs to lock a
 *	device (or test if it is locked), call the functions defined in
 *	the public interface of this library.
 *
 */

#ifndef _LOCKDEV_H_
#define _LOCKDEV_H_

#ifdef	__cplusplus
extern "C" {
#endif

#include <sys/types.h>
#include <errno.h>

/* API of the library */

void liblockdev_incr_debug (void);
void liblockdev_reset_debug (void);

pid_t dev_getpid (void);
pid_t dev_setpid (pid_t pid);

pid_t dev_testlock (const char *devname);

pid_t dev_lock (const char *devname);

pid_t dev_relock (const char * devname,
		  const pid_t  old_pid);

pid_t dev_unlock (const char * devname,
		  const pid_t pid);

#ifdef	__cplusplus
};
#endif

#endif /* _LOCKDEV_H_ */
