#include <stdio.h>
#include <iostream>
#include "unistd.h"
#include "mpi.h"
#include "pool.h"
#include "squirrel-functions.h"
#include "ran2.h"
#include "variables.h"
#include "cellActor.h"
#include "squirrelActor.h"
#include "timeActor.h"
#include "communicator.h"
using namespace std;

/*
*Start of the simulation and creation of every actor
*/

int main(int argc, char *argv[])
{
	double start_time, end_time, total_time;
	MPI_Init(&argc, &argv);
	start_time = MPI_Wtime();
	int statusCode = processPoolInit();
	int tempRank;
	MPI_Comm_rank(MPI_COMM_WORLD, &tempRank);

	//Attach actor type, regarding rank
	if (statusCode == 1)	//Worker
	{
		int myRank, parentId;
		MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

		if (myRank > EXTRA_ACTORS && myRank <= EXTRA_ACTORS + NUM_CELLS)//Cell Actors
		{
			Cell gridPoint;
			gridPoint.start();
			gridPoint.receiveInfo();
		}
		else if (myRank > NUM_CELLS + EXTRA_ACTORS)	//Squirrel Actors
		{
			int contSimul = 1;
			int stop;
			while (contSimul == 1)
			{
				Squirrel squir;
				squir.start();
				stop = 0;
				while (stop == 0)
				{
					stop = squir.oneStep(stop);
				}
				contSimul = workerSleep();

				if (contSimul == 0)
				{
					int myRank = squir.getSquirrelRank();
					MPI_Ssend(&myRank, 1, MPI_INT, COMMUNICATOR_RANK, 227, MPI_COMM_WORLD);
				}
			}
		}
		else if (myRank == TIME_RANK)	//Time Actor
		{
			Time masterProcess;
			masterProcess.timer();
		}
		else if (myRank == COMMUNICATOR_RANK)	//Communicator
		{
			Communicator track;
			track.counter();
		}
	}

	else if (statusCode == 2)	//Master (Rank 0)
	{	//Start the apropriate number of workers 
		int myRank, parentId;
		MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

		for (int j = 0; j < NUM_CELLS + NUM_SQUIRRELS + EXTRA_ACTORS; j++)
		{
			int workerPid = startWorkerProcess();
		}

		do{

		} while (masterPoll());

	}

	processPoolFinalise();
	end_time = MPI_Wtime();
	MPI_Finalize();

	if (tempRank == 0)
	{
		total_time = end_time - start_time;
		printf("Total time = %f seconds \n", total_time);
	}
	return 0;
}
