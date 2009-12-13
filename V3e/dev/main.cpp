#define DEBUG(x) printf(x);

#include "IRC.h"

int main(int argc, char *argv[])
{
	IRC irc;
	irc.run();

	std::cin.get();
	return 0;
}
