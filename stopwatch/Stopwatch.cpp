#include "Stopwatch.h"
#include <ratio>

void Stopwatch::start(){
    startTime = std::chrono::high_resolution_clock::now();
}

void Stopwatch::recordTime(const std::string& token) {
    std::chrono::high_resolution_clock::time_point currTime = std::chrono::high_resolution_clock::now();
    times[token] = currTime;
}

void Stopwatch::reset() {
    startTime = std::chrono::high_resolution_clock::now();
    times.clear();
}

double Stopwatch::getTimeDifference(const std::string& token1, const std::string& token2) {
    if (times.find(token1) == times.end() || times.find(token2) == times.end()) {
        return -1.0;
    }
    return std::chrono::duration<double, std::micro>(times[token2] - times[token1]).count();
}
