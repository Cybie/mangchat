/* MULTI THREADING MANAGER
 * 
 * Copyright (C) 2008 Cybr@x Cybersp@ce <cybraxvd@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * for a copy of the GNU General Public License write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#ifndef __THREAD_CONF__
#define __THREAD_CONF__

#include "common.h"

#define CYT_NS CyTHREAD

namespace CYT_NS
{
	#define TC_TIMEOUT	INFINITE

	#define TS_EXIT		0
	#define TS_INIT		1
	#define TS_START	2
	#define TS_ENTRY	3
	#define TS_RUN		4
	#define TS_EXEC		5

	#define DEBUG(what) printf(what)
	#define DEBUG2(what) //printf(what)
}

#endif

//DWORD WINAPI
