SRC = main.cpp squirrelActor.cpp cellActor.cpp ran2.c squirrel-functions.c pool.c timeActor.cpp communicator.cpp
LFLAGS=-lm
CC=mpicxx

all: 
	$(CC) -o main $(SRC) $(LFLAGS)

clean:
	rm main
	rm *.o
	rm *~
	rm *.dat
	rm *.txt
