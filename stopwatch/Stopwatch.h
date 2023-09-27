#ifndef STOPWATCH_H
#define STOPWATCH_H

#include <chrono>
#include <string>
#include <unordered_map>

class Stopwatch {
    private:
        std::chrono::high_resolution_clock::time_point startTime;
        std::unordered_map<std::string, std::chrono::high_resolution_clock::time_point> times;

    public:
        void start();
        void recordTime(const std::string& token); 
        void reset(); 
        double getTimeDifference(const std::string& token1, const std::string& token2); 
};

#endif // !STOPWATCH
