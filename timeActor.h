#ifndef Time_H
#define Time_H


class Time
{
	private: 
		int rank;
		int month;
		int livingSquir;
		int infectedSquir;

		
	public:
		Time();
		void kill();
		void timer();
		void endMonth();
};

#endif
