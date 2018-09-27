Actor Pattern implementation for PDP coursework

#Compile using Makefile:

$ make

Run front-end of Cirrus. Note: ProcessesNr >= ActorsNr

$ mpirun -n 220 ./main

Run back-end of Cirrus 

$ qsub pdp.pbs



#Adjustable variables:

    variables.h

C++ files:

    main.cpp
    communicator.cpp
    cellActor.cpp
    squirrelActor.cpp
    timeActor.cpp

Given functions:

    pool.c
    squirrel-functions.c
    ran2.c

Other files:

    Makefile: compiles program
    pdp.pbs submit script for back-end Cirrus


