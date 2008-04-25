#include "common.h"
#include "Master.h"
#include "ThreadMngr.h"

// Thread Manager
CyTHREAD::ThreadMngr TM;

int main(int argc, char *argv[])
{
	char hello[] = "Hi there 1";
	char hello2[] = "Hey there 2";

	TM.Create(new Master(), (void *)&hello);
	TM.Create(new Master(), (void *)&hello2);

	// Sleep time
	TM.GetThread(0)->_int = 1000;
	TM.GetThread(1)->_int = 3000;

	printf("Queued threads : %d", TM.Size());
	
	std::cin.get();

	// Start threads
	TM.Thread(0)->Resume();
	TM.Thread(1)->Resume();

	std::cin.get();
	printf("Shutting down threads\n");

	// End thread manager
	TM.ShutDown();
	
	std::cin.get();

  return 0;
}
