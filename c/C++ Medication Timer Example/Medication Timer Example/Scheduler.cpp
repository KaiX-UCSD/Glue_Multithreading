/* include stuff to do things */
#include <iostream>
#include <time.h>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <queue>
#include <pthread.h>
#include "Entry.h"

#define WEEKDAYS 7

/* events list */
std::vector<Entry> entries[WEEKDAYS];
unsigned int ptr = 0;

/* global queue */
std::queue<Entry> q;

/* variables to track time */
time_t timer;           // used to track unix timestamp 
struct tm * timeinfo;   // break down timestamp into manageable components
int second;             // keep track of current second, base unit time
int wday;               // keep track of current weekday
int hour, min, sec;     // needed because of struct tm bug

/* weekday strings */
const char* weekday[] = {
    "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"
};

/* function protocols */
void *timerRoutine(void* threadID);
void *queueRoutine(void* threadID);
void loadEntries(const char* filename);
void checkEntries();
bool timeToTake(Entry e);

/* main function used to start threads */
int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: ./Scheduler entryfile" << std::endl; 
        return -1;
    }

    loadEntries(argv[1]);

    /* start threads */
    pthread_t timerThread;
    pthread_t queueThread;
    
    // timer thread
    if (pthread_create(&timerThread, NULL, timerRoutine, NULL)) {
        std::cerr << "Unable to create timerThread" << std::endl;
        return -1;
    }

    // queue thread
    if (pthread_create(&queueThread, NULL, queueRoutine, NULL)) {
        std::cerr << "Unable to create queueThread" << std::endl;
        return -1;
    }

    pthread_exit(NULL);
    // warning: now reachable code
    return 0;
}

/* thread for timer */
void *timerRoutine(void* threadID) {
    /* initialize time */
    time(&timer);                   // get unix timestamp
    timeinfo = localtime(&timer);   // convert timestamp to local time
    second = timeinfo->tm_sec;      // set current second
    wday = timeinfo->tm_wday;        // set current day

    bool endOfWorld = false;        // keep track of the end of the world

    while (!endOfWorld) {           // loop while world exists
        time(&timer);
        timeinfo = localtime(&timer);
        hour = timeinfo->tm_hour;
        min = timeinfo->tm_min;
        sec = timeinfo->tm_sec;
        
        // check if second has changed, process events and update second
        if (second != timeinfo->tm_sec) {
            std::cout << asctime(timeinfo);
            second = timeinfo->tm_sec;

            // new day
            if (wday != timeinfo->tm_wday) {
                ptr = 0;
                wday = timeinfo->tm_wday;
            }
            
            checkEntries();
        }
    }

    pthread_exit(NULL);
}

/* thread for queue */
void *queueRoutine(void* threadID) {
    while (true) {
        if (!q.empty()) {
            Entry e = q.front();
            q.pop();
            std::cout << "Popped off " << e.getName() << std::endl;
        }
    }

    pthread_exit(NULL);
}

void loadEntries(const char* filename) {
    // example entry
    entries[6].push_back(Entry(6, 20, 11, 10, 0, 0, 1000000, "Jesse")); 

    std::ifstream infile;
    std::string line;
    infile.open(filename);

    int wday, hour, min, sec;
    int repeat;
    std::string pillName;

    if (!infile.is_open()) {
        std::cerr << "Could not open file" << std::endl;
        exit(EXIT_FAILURE);
    } else {
        while (std::getline(infile, line)) {
            std::stringstream ss(line);
            ss >> wday >> hour >> min >> sec >> repeat >> pillName;
            //std::cout << wday << "," << hour << "," << min << "," << sec << "," << pillName <<std::endl;
            entries[wday].push_back(Entry(wday, hour, min, sec, 0, 0, repeat, pillName));
        }
    }

    infile.close();
}

void checkEntries() {
    if (entries[wday].empty()) {
        std::cout << weekday[wday] << ": ";
        std::cout << "No pills to take today!" << std::endl;
    } 
    
    for (int i = 0; i < entries[wday].size(); ++i) {
        //std::cout << entries[wday].size() << std::endl;
        //std::cout << entries[wday][i].getName() << std::endl;
        if (timeToTake(entries[wday][i])) {
            std::cout << "Time to take " << entries[wday][i].getName() << std::endl;
            q.push(entries[wday][i]);
        }
    }

    /*if (ptr < entries[wday].size() && sameTime(timeinfo, entries[wday][ptr])) {
        std::cout << "Time to take " << entries[wday][ptr].getName() << std::endl;
        ptr++;
    }*/
}

bool timeToTake(Entry e) {
    /* localtime stupidly overwrites timeinfo for some reason */
    time_t time = e.getTime();
    struct tm * t2 = localtime(&time);

    // check if repeat time
    // diff can be over 60 years
    int diff = timer - time;
    bool repeat = (diff <= 0 && (diff % e.getRepeat() == 0));

    //std::cout << "Current" << "-" << hour << ":" << min << "." << sec << std::endl;
    //std::cout << e.getName() << "-" << t2->tm_hour << ":" << t2->tm_min << "." << t2->tm_sec << std::endl;

    return ((hour == t2->tm_hour && min == t2->tm_min && sec == t2->tm_sec) || 
            repeat);
}
