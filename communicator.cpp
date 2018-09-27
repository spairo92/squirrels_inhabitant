#include <stdio.h>
#include <iostream>
#include "mpi.h"
#include "pool.h"
#include "squirrel-functions.h"
#include "ran2.h"
#include "variables.h" 
#include "communicator.h"
using namespace std;


/*
*Class adjusting the simulation by coordinating all the actors
*/

Communicator::Communicator() {
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	state = - 1 - rank;
	//Save the number of living and infected squirrels
	livingSquir = NUM_SQUIRRELS;
	infectedSquir = INITIAL_INFECTION;
	for (int i =0 ; i< NUM_CELLS + EXTRA_ACTORS + NUM_SQUIRRELS + MAX_NUM_SQUIRRELS; i++){
		squirrelArray[i] = 0;
	}
	for( int i = EXTRA_ACTORS + NUM_CELLS + 1; i < EXTRA_ACTORS + NUM_CELLS + NUM_SQUIRRELS; i++) {
		squirrelArray[i] = 1;
	}
}


void Communicator::counter() {
	int sender;
	int tag;
	int flag =0;
	contSimul = 1;
	MPI_Status status;
	
	while(contSimul) {
		flag =0;
		//Go through the messages recieved
		MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);
		if(flag == 1) {			
			sender = status.MPI_SOURCE;
			tag = status.MPI_TAG;
			if(tag == 331) {	//Dead squirrel
				deathMsg(sender, tag, status);
			}
			else if(tag == 229) {	//Infected squirrel
				infectedSquirrel(sender, tag, status);
			}
			else if(tag == 332) {	//Newborn squirrel
				birthMsg(sender,tag,status);
			}
			else if(tag == 225) { //Month ended
				monthFinishing(sender, tag, status);
			}
			else if(tag == 227) { //Squirrels are shutting down
				shutDownMsg(sender, tag, status);
			}
		}
	}
}

//Squirrel dies 
void Communicator::deathMsg(int sender, int tag, MPI_Status status) {
	MPI_Recv(&comingMsg, 1, MPI_INT, sender, tag, MPI_COMM_WORLD, &status);
	//Update values of living and infected squirrels
	livingSquir --;
	infectedSquir--;
}

//Count infected squirrel 
void Communicator::infectedSquirrel(int sender, int tag, MPI_Status status) {
	MPI_Recv(&comingMsg, 1, MPI_INT, sender, tag, MPI_COMM_WORLD, &status);
	infectedSquir++;				
}

//Update on the new number of squirrels
void Communicator::birthMsg(int sender, int tag, MPI_Status status) {
	MPI_Recv(&comingMsg, 1, MPI_INT, sender, tag, MPI_COMM_WORLD, &status);
	livingSquir ++;
	if(squirrelArray[comingMsg] == 0) {
		squirrelArray[comingMsg] = 1; 
	} 
}

//Month has ended
void Communicator::monthFinishing(int sender, int tag, MPI_Status status){
	int livingNumb[2];
	livingNumb[0] = livingSquir;
	livingNumb[1] = infectedSquir;
	MPI_Recv(&comingMsg, 1, MPI_INT, sender, tag, MPI_COMM_WORLD, &status);	
	//Send array of living & infected squirrels 					
	MPI_Ssend(&livingNumb[0], 2, MPI_INT, sender, 226, MPI_COMM_WORLD);
}

void Communicator::shutDownMsg(int sender, int tag, MPI_Status status) {
	//Check all the squirrel actors are dead
	MPI_Recv(&comingMsg, 1, MPI_INT, sender, tag, MPI_COMM_WORLD, &status);						
	squirrelArray[sender] = 0;
	squirrelSum = 0;
	for (int i =0; i< NUM_SQUIRRELS + MAX_NUM_SQUIRRELS; i++){
		squirrelSum += squirrelArray[i];
	}
	//Inform time all squirrels are dead, for simulation to turn off
	if(squirrelSum == 0 ) {
		MPI_Ssend(&livingSquir, 1, MPI_INT, TIME_RANK, 228, MPI_COMM_WORLD);
		printf("All squirrels have shut down. Communicator shutting down \n");
		contSimul = 0;			
	}
}

