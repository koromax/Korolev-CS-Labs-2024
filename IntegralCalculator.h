#pragma once

namespace IntegralCalculator {
typedef double (*F)(const double);
typedef double (*I)(const double, const double);
enum class Method : int {
    IntRect = 1,
    IntTrap,
};

struct Result {
    double valueSum;
    double valueExact;
    int partitions;
};

[[nodiscard]] Result IntRect(const F f, const double a, const double b, const double eps);
[[nodiscard]] Result IntTrap(const F f, const double a, const double b, const double eps);
void StartMainProgram(int argc, char** argv);
}  // namespace IntegralCalculator
