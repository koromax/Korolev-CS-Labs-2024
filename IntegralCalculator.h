#pragma once

namespace IntegralCalculator {
typedef double (*F)(double);
typedef double (*I)(double, double);
enum class Method : int {
    IntRect = 1,
    IncTram,
};

struct Result {
    double valueSum;
    double valueExact;
    int partitions;
};

Result IntRect(const F f, const double a, const double b, const double eps);
Result IntTrap(const F f, const double a, const double b, const double eps);
void StartMainProgram(int argc, char** argv);
}  // namespace IntegralCalculator
