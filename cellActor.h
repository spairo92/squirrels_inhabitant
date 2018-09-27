#ifndef CELL_H
#define CELL_H

class Cell
{
	private: 
		int rank; 
		int comingMsg;
		int sender;
		int tag;
		int month;
		int simulationRunning;
		int currentPopulInflx;
		int populInflx;
		int populInflxLastMonths[3];
		int currentInfectionLevel;
		int infectionLevel;
		int infectionLevelLastMonths[2];


	public:
		Cell();
		void start();
		void receiveInfo();
		void getKilled (int sender, int tag, MPI_Status status);
		void informSquirrel(int sender, int tag, MPI_Status status);
		void monthlyUpdates(int sender, int tag, MPI_Status status);
		
};

#endif
