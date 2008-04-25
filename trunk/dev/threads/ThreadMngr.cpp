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
#include "ThreadMngr.h"

namespace CYT_NS
{
	ThreadMngr::ThreadMngr(void)
	{
		DEBUG("Thread Manager Initialized.\n");
	}

	ThreadMngr::~ThreadMngr(void)
	{
		for(std::list<THREAD_ARG *>::iterator _tr = _threads.begin(); _tr != _threads.end();_tr++)
		{
			delete (*_tr);
			_tr = _threads.erase(_tr);
		}
	}

	void ThreadMngr::Instance()
	{
		for(std::list<THREAD_ARG *>::iterator _tr = _threads.begin(); _tr != _threads.end();_tr++)
		{
		}
	}

	void ThreadMngr::AddToQueue(THREAD_ARG *_thread)
	{
		DEBUG("Thread created...\n");
		_threads.push_back(_thread);
	}

	void ThreadMngr::Create(CThread *Thread, void *arg)
	{
		THREAD_ARG *THREAD = new THREAD_ARG(arg);
		THREAD->_Thread = Thread;

		Thread->_tm(this);
		Thread->Run((void *)THREAD, true); //HANDLE hThread = (HANDLE)
		AddToQueue(THREAD);
	}

	void ThreadMngr::Create(CThread *Thread, THREAD_ARG *_thread)
	{
		Thread->Run((void *)_thread); //HANDLE hThread = (HANDLE)
		AddToQueue(_thread);
	}

	THREAD_ARG *ThreadMngr::GetThread(int _id)
	{
		int tcount = 0;
		for(std::list<THREAD_ARG *>::iterator _tr = _threads.begin(); _tr != _threads.end();_tr++)
		{
			if(_id == tcount)
				return (*_tr);
			tcount++;
		}
	}

	void ThreadMngr::ShutDown(void)
	{
		for(std::list<THREAD_ARG *>::iterator _tr = _threads.begin(); _tr != _threads.end();_tr++)
		{
			if((*_tr)->_Thread->IsActive())// _bool = false;
			{
				(*_tr)->_bool = false;
				DEBUG("Shutdown signal sent\n");
			}
		}
		
	}
}