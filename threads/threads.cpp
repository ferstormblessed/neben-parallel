#include <iostream>
#include <thread>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../stopwatch/Stopwatch.h"
#include <future>
#include <chrono>

const int n_dimensions = 100;
const int range = 100000;
const int max_num_vectors = 16;
const int num_threads = 4;

int randomNum(int limit, bool zero) {
    std::srand(std::rand() % limit);
    if(zero) {
        return std::rand() % limit;
    }
        return std::rand() % limit + 1;
}

std::vector<double> generateRandomVector() {
    int vlen = randomNum(n_dimensions, false);
    std::vector<double>randVec(vlen, 0);

    for(int i = 0; i < vlen; i++) {
        randVec[i] = (double)randomNum(range, true) / (double)randomNum(range, false);
    }
    return randVec;
}

void calculateVectorLength(const std::vector<double>& v) {
    double len = 0.0;

    for(int i = 0; i < v.size(); i++) {
        len += v[i] * v[i];
    }
    std::cout << "length is: " << std::sqrt(len) << std::endl;
}


void printVector(std::vector<double>& v) {
    for(int i = 0; i < v.size(); i++) {
        std::cout << v[i] << " ";
    }
    std::cout << std::endl;
}

void foo()
{
    // simulate expensive operation
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

int main (int argc, char *argv[]) {
    std::srand(time(NULL));

    int num_vectors = randomNum(max_num_vectors, false) % 4;
    std::vector<std::vector<double> > vectors(num_vectors);

    Stopwatch stopwatch;
    stopwatch.start();

    stopwatch.recordTime("A");
    for(int i = 0; i < num_vectors; i++) {
        vectors[i] = generateRandomVector();
    }
    stopwatch.recordTime("B");
    std::cout << "time to create vectors " << stopwatch.getTimeDifference("A", "B") << " miliseconds" << std::endl;

    //std::jthread
    stopwatch.recordTime("threadA");
    //implementation
    std::vector<std::thread> threads;
    std::thread t(foo);
    t.join();

    stopwatch.recordTime("threadB");
    std::cout << "time to calculate with threads " << stopwatch.getTimeDifference("threadA", "threadB") << " miliseconds" << std::endl;

    //std::async 
    stopwatch.recordTime("asyncA");
    //implementation
    stopwatch.recordTime("asyncB");
    std::cout << "time to calculate with async " << stopwatch.getTimeDifference("asyncA", "asyncB") << " miliseconds" << std::endl;

    return 0;
}
