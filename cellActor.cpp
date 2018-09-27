#include <stdio.h>
#include <iostream>
#include "mpi.h"
#include "pool.h"
#include "squirrel-functions.h"
#include "ran2.h"
#include "variables.h" 
#include "cellActor.h"
using namespace std;

/*
*Class containing all the methods related to Cell actor
*/

Cell::Cell() {
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
}

//Initialize cell
void Cell::start() {
	simulationRunning = 1;
	currentPopulInflx = 0;
	currentInfectionLevel = 0;
	populInflx = 0;
	infectionLevel = 0;
	for(int i =0; i < 2; i++){
		populInflxLastMonths[i] = 0;
		infectionLevelLastMonths[i] = 0;
	}
	populInflxLastMonths[2] = 0;
	
}

//Regarding the messages recieved call functions
void Cell::receiveInfo() {
	MPI_Status status;
	int flag = 0;
	while(simulationRunning) {
		MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);
		if(flag == 1) {			
			MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			sender = status.MPI_SOURCE;
			tag = status.MPI_TAG;
			if(tag == 333) { //Recieve from time actor the end of month
				monthlyUpdates(sender, tag, status);
			}
			if(tag == 221) { //Infection status of squirrel
				informSquirrel(sender, tag, status);
			}
			if(tag == 334) { //Kill order from time actor
				getKilled(sender, tag, status);
			}
			flag = 0;
		}
	}
}


//Updates on population influx and infection levels
void Cell::monthlyUpdates(int sender, int tag, MPI_Status status) {
	MPI_Recv(&month, 1, MPI_INT, sender, tag, MPI_COMM_WORLD, &status);
	printf("Month: %d - Cell: %d - Population Influx: %d - Infection Level: %d \n", month, rank, populInflx, infectionLevel);
	//Over write on the old ones
	populInflxLastMonths[month%3] = currentPopulInflx; 
	infectionLevelLastMonths[month%2] = currentInfectionLevel;
	currentPopulInflx = 0;
	currentInfectionLevel = 0;
	infectionLevel = infectionLevelLastMonths[0] + infectionLevelLastMonths[1];
	populInflx = populInflxLastMonths[0] + populInflxLastMonths[1] + populInflxLastMonths[2];
}

//Update squirrel (actors) on the cell about Infection Level and Population Influx in cell
void Cell::informSquirrel(int sender, int tag, MPI_Status status) {
	MPI_Recv(&comingMsg, 1, MPI_INT, sender, tag, MPI_COMM_WORLD, &status);
	MPI_Ssend(&infectionLevel, 1, MPI_INT, sender, 222, MPI_COMM_WORLD);
	MPI_Ssend(&populInflx, 1, MPI_INT, sender, 223, MPI_COMM_WORLD);
	currentPopulInflx++;
	if(comingMsg == 1) {
		currentInfectionLevel++;
	}
}

//Shut cell down
void Cell::getKilled (int sender, int tag, MPI_Status status) {
	MPI_Recv(&comingMsg, 1, MPI_INT, sender, tag, MPI_COMM_WORLD, &status);
	simulationRunning = 0;
}



