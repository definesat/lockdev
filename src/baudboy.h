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

#ifdef	__cplusplus
extern "C" {
#endif

int ttylock_helper (const char * devname);
int ttywait_helper (const char * devname);
int ttyunlock_helper (const char * devname);
int ttylocked_helper (const char * devname);

#ifndef _LIBLOCKDEV_NO_BAUDBOY_DEFINES
#define ttylock(devname) ttylock_helper(devname)
#define ttywait(devname) ttywait_helper(devname)
#define ttyunlock(devname) ttyunlock_helper(devname)
#define ttylocked(devname) ttylocked_helper(devname)
#endif

#ifdef	__cplusplus
};
#endif

#endif /* _BAUDBOY_H_ */
