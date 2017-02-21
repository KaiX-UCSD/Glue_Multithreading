// entry.h
#ifndef ENTRY_H
#define ENTRY_H

#include <string>
#include <time.h>

class Entry {
private:
    time_t t;
    int dose;
    int window;
    int repeat; // time in seconds to repeat
    std::string pillName;

public:
    Entry(int wday, int hour, int min, int sec, 
        int dose, int window, int repeat, std::string pillName);
//    Entry(int wday, int hour, int min, std::string pillName);
    time_t getTime();
    int getDose();
    int getWindow();
    int getRepeat();
    std::string getName();
};

#endif
