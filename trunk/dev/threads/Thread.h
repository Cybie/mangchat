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

#ifndef __THREAD__
#define __THREAD__
#include "ThreadConfig.h"

namespace CYT_NS
{
	class CThread;
	class ThreadMngr;

	struct THREAD_ARG
	{
		void *_arg;
		bool _bool;
		int _int;

		CThread *_Thread;

		THREAD_ARG(){};
		THREAD_ARG(void *arg)
		{
			_arg = arg;
			_bool = true;
			_int = 0;
			_Thread = NULL;
		};
		
	};

	template <class T>
	T GetMax (T a, T b) {
	  T result;
	  result = (a>b)? a : b;
	  return (result);
	}

	class CThread
	{
		public:
			CThread();
			~CThread();
			CThread(void *arg);

		public:
			int Run(bool _delay = false);
			int Run(void *arg, bool _delay = false);
			int RunDelay();
			int Resume(void);
			bool Kill(void);

			bool IsActive() { return bActive; };

			void _tm(ThreadMngr *_val) { tMngr = _val; };

			THREAD_ARG *GetArg(){ return &_targ; };
			HANDLE GetHandle() { return hThread; };

			void Wait( int nMilSec ) { WaitTime = nMilSec; };

		protected:
			static DWORD WINAPI EntryPoint(void *);
			
			int Start(void *arg);
			virtual void Setup();
			virtual void Execute(void *arg);

			void *Arg() const { return _arg; };
			void Arg(void *_void) { _arg = _void; };

			int _state;
			int State() const { return _state; };
			void State(int _val) { _state = _val; };
			ThreadMngr *GetTM() const { return tMngr; };

		protected:
			bool bActive;
			bool bLock;
			bool bStart;
			int nID;

			int WaitTime;

			ThreadMngr *tMngr;

		private:
			void Init(void);

		private:
			void *_arg;
			THREAD_ARG _targ;

		private:
			HANDLE		 hThread;
			DWORD		 nThreadID;                    
	};
}
#endif

/*
	pthread_t    hThread;
	int          nThreadID;
	nThreadID = pthread_create( &hThread, NULL, &TRMain, (void*)tmsg);
	//pthread_join( hThread, NULL);
    void         *TRMain( void *ptr );
*/
