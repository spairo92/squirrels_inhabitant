#ifndef SQUIRREL_H
#define SQUIRREL_H
#include "variables.h"

class Squirrel
{
	private: 
		long state;
		int rank;
		float x;
		float y;
		float avgInfectionLevel;
		float avgPopulationInflux;
		float pos[2];
		int infected;
		int numSteps;
		int infectedStep;
		int populationLastCells[SQUIRREL_MIN_STEPS];
		int infectionLastCells[SQUIRREL_MIN_STEPS];
		int populationCurrentCell;
		int infectionCurrentCell;
		int simulationRunning;
		int cellRank;
		int birth;
		int death;
		int flag;
		int tag;	


	public: 
		Squirrel();
		void start();
		void getSquirrelCell();
		void squirrelInformCell();
		void updateStep();
		void updateValues();
		void giveBirth();
		int getSquirrelRank();
		int getCellRank();
		int getSimulation();
		int willSquirrelDie();
		int oneStep(int stop);
};

#endif
