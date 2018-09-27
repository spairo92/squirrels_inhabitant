#include <stdio.h>
#include <iostream>
#include "mpi.h"
#include "pool.h"
#include "squirrel-functions.h"
#include "ran2.h"
#include "variables.h" 
#include "squirrelActor.h"
using namespace std;

/*
*Class containing all the actions the squirrel performs
*/

Squirrel::Squirrel(){
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	state = - 1 - rank;
	initialiseRNG(&state);
}

//Every check related to the squirel on one timestep
int Squirrel::oneStep(int stop) {
	updateStep();
	getSquirrelCell();	
	squirrelInformCell();
	updateValues();
	giveBirth();
	int toDie = willSquirrelDie();
	int toStop = shouldWorkerStop();
	if( toDie==1 || toStop==1 ) { 
		stop = 1;
	}
	return stop;
}

//Initialize a squirrel (actor)
void Squirrel::start() {
	x=0;
	y=0;
	infected = 0;
	simulationRunning = 1;
	numSteps = 0;
	int parent = getCommandData();
	//First four squirrels to be infected (randomly)
	if( parent == 0 && (rank - NUM_CELLS - EXTRA_ACTORS <  INITIAL_INFECTION)){
		infected = 1;
		infectedStep = 0;
	}
	//Update position when born
	if(parent != 0 ) {
		int parent = getCommandData();
		MPI_Recv(&pos[0], 2, MPI_FLOAT, parent, 224, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		x = pos[0]; 
		y = pos[1];
	}
	for(int i = 0; i<SQUIRREL_MIN_STEPS; i++) {
		infectionLastCells[i]=0;
		populationLastCells[i]=0;
	}
}

//New squirrel position
void Squirrel::updateStep() {
	float x_new, y_new;	
	squirrelStep(x,y,&x_new,&y_new,&state); 
	x=x_new;
	y=y_new;	
	numSteps ++;
	pos[0] = x;
	pos[1] = y;
}

//Communicate with the given cell and update population and infection arrays
void Squirrel::squirrelInformCell() {
	MPI_Status status;
	MPI_Ssend(&infected, 1, MPI_INT, cellRank, 221, MPI_COMM_WORLD);
	MPI_Recv(&infectionCurrentCell, 1, MPI_INT, cellRank, 222, MPI_COMM_WORLD, MPI_STATUS_IGNORE);	
	MPI_Recv(&populationCurrentCell, 1, MPI_INT, cellRank, 223, MPI_COMM_WORLD, MPI_STATUS_IGNORE);	
	populationLastCells[numSteps % SQUIRREL_MIN_STEPS ] = populationCurrentCell;
	infectionLastCells[numSteps % SQUIRREL_MIN_STEPS] = infectionCurrentCell;
}

//Update new infection and influx values
void Squirrel::updateValues() {
	avgInfectionLevel = 0;
	avgPopulationInflux = 0;
	for(int i = 0; i < SQUIRREL_MIN_STEPS; i++) { 
		avgInfectionLevel += infectionLastCells[i];
		avgPopulationInflux += populationLastCells[i];
	}
	avgInfectionLevel = avgInfectionLevel/SQUIRREL_MIN_STEPS;
	avgPopulationInflux = avgPopulationInflux/SQUIRREL_MIN_STEPS;
}

//Check if new squirrel should be born
void Squirrel::giveBirth() {
	int workingPid;
	birth = 0;
	if(numSteps % BIRTH_CHECK_FREQ == 0) {
		birth = willGiveBirth(avgPopulationInflux, &state);
		if(birth == 1 && shouldWorkerStop() == 0) {
			//Start a new worker process for the new born
			workingPid = startWorkerProcess();
			//Update communicator 
			MPI_Ssend(&workingPid, 1, MPI_INT, COMMUNICATOR_RANK, 332, MPI_COMM_WORLD); 
			//Send the position of the new born
			MPI_Ssend(&pos[0], 2, MPI_FLOAT, workingPid, 224, MPI_COMM_WORLD); 
		}
	}
}

//Check squirrels status
int Squirrel::willSquirrelDie() {
	death = 0;
	//Check whether squirrel will get infected
	if(infected == 0) {
		if(willCatchDisease(avgInfectionLevel, &state)==1){
			infected = 1;
			infectedStep = numSteps;
			//Update communicator squirrel got infected
			MPI_Ssend(&infected, 1, MPI_INT, COMMUNICATOR_RANK, 229, MPI_COMM_WORLD);
		}	
	}
	//Check whether squirrel will die
	if(infected == 1) {
		if(numSteps - infectedStep > 50) {
			death = willDie(&state);
			if(death == 1){
				//Update communicator squirrel is dead
				MPI_Ssend(&death, 1, MPI_INT, COMMUNICATOR_RANK, 331, MPI_COMM_WORLD);
			}
		}
	}
	return death;
}

//Cell containing squirrel
void Squirrel::getSquirrelCell() {
	cellRank = 1 + EXTRA_ACTORS + getCellFromPosition(x,y);
}

int Squirrel::getSquirrelRank() {
	return rank;
}	
