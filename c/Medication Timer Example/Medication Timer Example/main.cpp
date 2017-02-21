#include <iostream>
#include <queue>
#include <string>
#include "Entry.h"

int main(int argc, char** argv) {

    Entry* e = new Entry(0,0,0,0,0,0,0, "ABC");

    std::queue<std::string> names; /* Declare a queue */
    names.push("Ben"); /* Add some values to the queue */
    names.push("Erin"); /* Much like vectors */
    names.push("Dan"); /* This basically does the same thing */

    std::cout << "Welcome to US Coney and Cone!" << std::endl << std::endl;
    std::cout << "Now serving: "
        << names.front() << std::endl << std::endl;
    names.pop();
    std::cout << "There are currently " << names.size()
        << " people in the queue. "
        << "The next person in the queue is "
        << names.front() << "." << std::endl << std::endl

        << names.back() << " is the last person in the queue."
        << std::endl;

    std::cin.get();

    return 0;
}
