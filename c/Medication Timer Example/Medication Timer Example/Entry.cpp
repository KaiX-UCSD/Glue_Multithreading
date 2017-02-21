#include "Entry.h"

Entry::Entry(int wday, int hour, int min, int sec, int dose, int window, int repeat, std::string pillName) {
    struct tm timeinfo = {0};
    timeinfo.tm_wday = wday;
    timeinfo.tm_hour = hour;
    timeinfo.tm_min = min;
    timeinfo.tm_sec = sec;
    t = mktime(&timeinfo);

    this->dose = dose;
    this->window = window;
    this->repeat = repeat;
    this->pillName = pillName;
}

time_t Entry::getTime() { return t; }
int Entry::getDose() { return dose; }
int Entry::getWindow() { return window; }
int Entry::getRepeat() { return repeat; }
std::string Entry::getName() { return pillName; }
