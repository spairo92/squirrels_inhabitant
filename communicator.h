#ifndef COMMUNICATOR_H
#define COMMUNICATOR_H
#include "variables.h"

class Communicator
{
private:
	long state;
	int rank;
	int contSimul;
	int comingMsg;
	int livingSquir;
	int infectedSquir;
	int squirrelSum;
	int squirrelArray[NUM_CELLS + EXTRA_ACTORS + NUM_SQUIRRELS + MAX_NUM_SQUIRRELS];

	
public:
	Communicator();
	void counter();
	void birthMsg(int sender, int tag, MPI_Status status);
	void deathMsg(int sender, int tag, MPI_Status status);
	void shutDownMsg(int sender, int tag, MPI_Status status);
	void monthFinishing(int sender, int tag, MPI_Status status);
	void infectedSquirrel(int sender, int tag, MPI_Status status);
};

#endif
