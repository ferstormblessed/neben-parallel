#include <iostream>
#include <cmath>
#include <thread>
#include <vector>
#include <atomic>
#include "../stopwatch/Stopwatch.h"

std::atomic<double> pi{0.0};

void calculatePiPartial(int start, int end) {
    double partialPi = 0.0;
    for (int k = start; k < end; k++) {
        double term = 1.0 / (2.0 * k + 1);
        if (k % 2 == 0) {
            partialPi += term;
        } else {
            partialPi -= term;
        }
    }
    pi = pi + partialPi;
}

int main() {
    Stopwatch c;
    c.start();

    int numThreads = std::thread::hardware_concurrency(); // Bestimmen Sie die Anzahl der verfügbaren Threads.

    std::vector<std::thread> threads;
    // 10 to 5
    int n = std::pow(10, 5);
    int chunkSize = n / numThreads;

    c.recordTime("10to5s");
    for (int i = 0; i < numThreads; i++) {
        int start = i * chunkSize;
        int end = (i == numThreads - 1) ? n : (i + 1) * chunkSize;
        threads.emplace_back(calculatePiPartial, start, end);
    }

    for (auto& thread : threads) {
        thread.join();
    }
    c.recordTime("10to5e");


    double result = pi.load() * 4.0;

    std::cout << "PI approximated with " << n << " terms using " << numThreads << " threads: " << result << std::endl;
    std::cout << "Time to calculate: " << c.getTimeDifference("10to5s", "10to5e") << " ms" << std::endl;


    // 10 to 6
    pi = 0;
    threads.clear();
    n = std::pow(10, 6);
    chunkSize = n / numThreads;

    c.recordTime("10to6s");
    for (int i = 0; i < numThreads; i++) {
        int start = i * chunkSize;
        int end = (i == numThreads - 1) ? n : (i + 1) * chunkSize;
        threads.emplace_back(calculatePiPartial, start, end);
    }

    for (auto& thread : threads) {
        thread.join();
    }
    c.recordTime("10to6e");


    result = pi.load() * 4.0;

    std::cout << "PI approximated with " << n << " terms using " << numThreads << " threads: " << result << std::endl;
    std::cout << "Time to calculate: " << c.getTimeDifference("10to6s", "10to6e") << " ms" << std::endl;


    // 10 to 7
    pi = 0;
    threads.clear();
    n = std::pow(10, 7);
    chunkSize = n / numThreads;

    c.recordTime("10to7s");
    for (int i = 0; i < numThreads; i++) {
        int start = i * chunkSize;
        int end = (i == numThreads - 1) ? n : (i + 1) * chunkSize;
        threads.emplace_back(calculatePiPartial, start, end);
    }

    for (auto& thread : threads) {
        thread.join();
    }
    c.recordTime("10to7e");


    result = pi.load() * 4.0;

    std::cout << "PI approximated with " << n << " terms using " << numThreads << " threads: " << result << std::endl;
    std::cout << "Time to calculate: " << c.getTimeDifference("10to7s", "10to7e") << " ms" << std::endl;
    return 0;
}

