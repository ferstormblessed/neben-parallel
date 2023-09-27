#include <iostream>
#include "Stopwatch.h"

int main (int argc, char *argv[]) {

    Stopwatch uhr;

    uhr.start();
    std::cout << "clock starts" << std::endl;
    uhr.recordTime("A");
    std::cout << "recording time A" << std::endl;

    uhr.recordTime("B");
    std::cout << "recording time B" << std::endl;

    uhr.recordTime("C");
    std::cout << "recording time C" << std::endl;

    uhr.recordTime("D");
    std::cout << "recording time D" << std::endl;
    
    double diff = uhr.getTimeDifference("A", "B");
    std::cout << "time difference between A and B " << diff << " microseconds" << std::endl;
    diff = uhr.getTimeDifference("A", "C");
    std::cout << "time difference between A and C " << diff << " microseconds" << std::endl;
    diff = uhr.getTimeDifference("B", "D");
    std::cout << "time difference between B and D " << diff << " microseconds" << std::endl;
    diff = uhr.getTimeDifference("C", "D");
    std::cout << "time difference between C and D " << diff << " microseconds" << std::endl;
    diff = uhr.getTimeDifference("A", "D");
    std::cout << "time difference between A and D " << diff << " microseconds" << std::endl;
}
