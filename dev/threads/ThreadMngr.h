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
#ifndef __THREAD_MNGR__
#define __THREAD_MNGR__

#include "Thread.h"

namespace CYT_NS
{
	class ThreadMngr
	{
	public:
		ThreadMngr(void);
		~ThreadMngr(void);
	public:
		void Create(CThread *Thread, void *arg);
		void Create(CThread *Thread, THREAD_ARG *_thread);
		void AddToQueue(THREAD_ARG *_thread);
		void ShutDown(void);

		THREAD_ARG *GetThread(int _id);

		CThread *Thread(int _id) { return GetThread(_id)->_Thread; };

		void Instance();

		int Size() { return _threads.size(); };

	protected:
		//CThread

	private:
		std::list<THREAD_ARG *>_threads;
	};
}

#endif
