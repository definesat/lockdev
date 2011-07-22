/*
 *	ttylock.h
 *	(c) 1997 by Fabrizio Polacco <fpolacco@debian.org>
 *	this header file is part of the lockdev library.
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
 *	This header file provides an alternative API to the services of
 *	the lockdev library, the same API implemented by AIX.
 *	It is intended as a quick help for any program originally
 *	designed for the AIX that wants to move to lockdev.
 *
 */

#ifndef _TTYLOCK_H_
#define _TTYLOCK_H_

#ifdef	__cplusplus
extern "C" {
#endif

#include <lockdev.h>

/* API of the library */

int ttylock (const char * devname);
int ttywait (const char * devname);
int ttyunlock (const char * devname);
int ttylocked (const char * devname);

#ifdef	__cplusplus
};
#endif

#endif /* _TTYLOCK_H_ */
