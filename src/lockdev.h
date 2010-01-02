/*
 *	lockdev.h
 *	(c) 1997 by Fabrizio Polacco <fpolacco@debian.org>
 *	this source program is part of the liblockdev library.
 *
 *	liblockdev 0.1
 *	0.1-1	Wed,  3 Sep 1997 13:44:36 +0300
 *		- initial release
 *
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
 *	device (or test if it is locked), call the functions defined in
 *	the public interface of this library.
 *
 */

#ifndef _LOCKDEV_H_
#define _LOCKDEV_H_

#ifdef	__cplusplus
extern "C" {
#endif

#ifndef	__P
#  if defined	(__STDC__) || defined (_AIX) \
		|| (defined (__mips) && defined (_SYSTYPE_SVR4)) \
		|| defined(WIN32) || defined(__cplusplus)
#    define	__P(protos) protos
#  else
#    define	__P(protos) ()
#  endif
#endif

#include <sys/types.h>


/* API of the library */

void	liblockdev_incr_debug __P(());
void	liblockdev_reset_debug __P(());

pid_t	is_dev_lock __P(( const char * devname));

pid_t	lock_dev __P(( const char * devname));

pid_t	relock_dev __P(( const char * devname, const pid_t old_pid));

pid_t	unlock_dev __P(( const char * devname, const pid_t pid));


#ifdef	__cplusplus
};
#endif

#endif /* _LOCKDEV_H_ */
