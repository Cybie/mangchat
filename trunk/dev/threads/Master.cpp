#include "Master.h"

Master::Master(void){}
Master::~Master(void){}
Master::Master(void *arg){}

void Master::Setup(){}

void Master::Execute(void *arg)
{
	CyTHREAD::THREAD_ARG *targ = (CyTHREAD::THREAD_ARG *)arg;

	//GetTM()->GetThread(0)->_int = 1000;

	do
	{
		
		printf("Hello - %d - %s\n", targ->_bool, (char *)targ->_arg);
		Sleep(targ->_int);

		Sleep(5000);
		//GetTM()->GetThread(1)->_bool = false;
		//AppRunning = false;
	}while(targ->_bool);
}

