all: Scheduler.cpp Entry.o
	g++ -std=c++11 -pthread Entry.o Scheduler.cpp -o Scheduler

Entry.o: Entry.cpp Entry.h
	g++ -c Entry.cpp Entry.h
