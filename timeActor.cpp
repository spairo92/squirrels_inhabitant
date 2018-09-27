#include <stdio.h>
#include <iostream>
#include "unistd.h"
#include "mpi.h"
#include "pool.h"
#include "squirrel-functions.h"
#include "ran2.h"
#include "variables.h" 
#include "timeActor.h"
using namespace std;

/*
*Class containing all the methods on administrating the simulation
*/

Time::Time() {
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
}


void Time::timer() {
	int comingMsg;
	int flag =0 ;
	MPI_Status status;
	
	//Simulating time
	for(month = 1 ; month <= NUM_MONTHS; month++) {
		sleep(SECONDS_PER_MONTH);	//How long a month lasts
		endMonth();
		printf("SUMMARY -- Month = %d. Number of squirrels = %d. Number Infected = %d. \n", month, livingSquir, infectedSquir);			
	}
	// Shut down procedure
	shutdownPool();
	MPI_Recv(&livingSquir, 1, MPI_INT, COMMUNICATOR_RANK, 228, MPI_COMM_WORLD, &status); //Get informed on death of all squirrels
	kill(); //Kill cells
	printf("Simulation finished with %d Squirrels alive\n", livingSquir);
}


void Time::endMonth(){
	//Inform cells for the end of the month
	for(int a= 1 + EXTRA_ACTORS; a <= EXTRA_ACTORS + NUM_CELLS; a++) {
		MPI_Ssend(&month, 1, MPI_INT, a, 333, MPI_COMM_WORLD);
	}
	int comingMsg[2];
	//Update communicator
	MPI_Ssend(&month, 1, MPI_INT, COMMUNICATOR_RANK, 225, MPI_COMM_WORLD);
	//Recieve array with squirrels alive and infected
	MPI_Recv(&comingMsg[0], 2, MPI_INT, COMMUNICATOR_RANK, 226, MPI_COMM_WORLD, MPI_STATUS_IGNORE);		
	livingSquir = comingMsg[0]; 
	infectedSquir = comingMsg[1];
	//Shut down simulation if number of squirrels passed boundaries
	if(livingSquir > MAX_NUM_SQUIRRELS || livingSquir < 1) {	
		if(livingSquir > MAX_NUM_SQUIRRELS)printf("Shutting down! Number of squirrels too high.\n");
		if(livingSquir < 1)printf("Shutting down! All squirrels are dead. \n");
		month = NUM_MONTHS;
	}
}


void Time::kill() {	//Kill cells
	int a, comingMsg;
	sleep(2);
	for(a = EXTRA_ACTORS + 1; a <= EXTRA_ACTORS + NUM_CELLS; a++) {
		MPI_Ssend(&comingMsg, 1, MPI_INT, a, 334, MPI_COMM_WORLD);
	}	
}
