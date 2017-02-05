all: Scheduler.cpp Entry.o
	g++-4.7 -std=c++11 -pthread Entry.o Scheduler.cpp -o Scheduler

Entry.o: Entry.cpp Entry.h
	g++-4.7 -c Entry.cpp Entry.h
