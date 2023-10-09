#include <iostream>
#include <cmath>
#include "../stopwatch/Stopwatch.h"

double calculatePiSequential(int n) {
    double pi = 0.0;
    for (int k = 0; k < n; k++) {
        double term = 1.0 / (2.0 * k + 1);
        if (k % 2 == 0) {
            pi += term;
        } else {
            pi -= term;
        }
    }
    return pi * 4.0;
}

int main() {
    Stopwatch c;
    c.start();

    int n = std::pow(10, 5);
    
    c.recordTime("10to5s");
    double result = calculatePiSequential(n);
    c.recordTime("10to5e");
    std::cout << "PI approximated with " << n << " terms: " << result << std::endl;
    std::cout << "Time to calculate: " << c.getTimeDifference("10to5s","10to5e") << " ms" << std::endl;
    
    n = std::pow(10, 6);
    c.recordTime("10to6s");
    result = calculatePiSequential(n);
    c.recordTime("10to6e");
    std::cout << "PI approximated with " << n << " terms: " << result << std::endl;
    std::cout << "Time to calculate: " << c.getTimeDifference("10to6s","10to6e") << " ms" << std::endl;

    n = std::pow(10, 7);
    c.recordTime("10to7s");
    result = calculatePiSequential(n);
    c.recordTime("10to7e");
    std::cout << "PI approximated with " << n << " terms: " << result << std::endl;
    std::cout << "Time to calculate: " << c.getTimeDifference("10to7s","10to7e") << " ms" << std::endl;

    return 0;
}
