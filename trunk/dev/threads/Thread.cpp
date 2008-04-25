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
#include "Threading.h"

namespace CYT_NS
{
	CThread::CThread()
	{
		Init();
		_targ._arg = (void *)"";
	}

	CThread::~CThread()
	{
		DEBUG2("Thread terminated...\n");
	}

	CThread::CThread(void *arg)
	{
		Init();
		_targ._arg = arg;
	}

	void CThread::Init(void)
	{
		bLock = false;
		bStart = true;
		State(TS_INIT);
		nID = 0;
		WaitTime = 0;
		_targ._bool = true;
	}

	int CThread::Run(bool _delay)
	{
		return Run((void *)&_targ,  _delay);
	}

	int CThread::Run(void *arg, bool _delay)
	{
		if(_delay)
			bStart = false;

		DEBUG2("Thread Created...");
		State(TS_START);
		Arg(arg);
		hThread = CreateThread(NULL, NULL, CThread::EntryPoint, this, CREATE_SUSPENDED, &nThreadID);
		if(bStart)
			RunDelay();

		bActive = bStart;
		return (int)nThreadID;
	}

	int CThread::RunDelay()
	{
		return ResumeThread(hThread);
	}

	DWORD WINAPI CThread::EntryPoint(void * pthis)
	{
		
		DEBUG2("Entry...");
		CThread *pt = (CThread *)pthis;
		pt->State(TS_ENTRY);
		pt->Start( pt->Arg() );
		//WaitForSingleObject(pt->hThread, TC_TIMEOUT);
		DEBUG("Thread ended...\n");
		return 0;
	}

	int CThread::Start(void * arg)
	{
		bActive = true;
		DEBUG2("started\n");
		State(TS_RUN);

		Setup();
		Execute( arg );
		
		return true;
	}

	void CThread::Setup(){}
	void CThread::Execute(void *arg){}

	bool CThread::Kill(void)
	{
		bActive = false;
		State(TS_EXIT);
		CloseHandle(hThread);
		ExitThread(0);
		return true;
	}

	int CThread::Resume(void)
	{
		return ResumeThread(hThread);
	}
}