#pragma once

namespace EquationCalculator {
enum class EquationMethod : int {
    Iterative = 1,
    Newtons,
    Bisection,
};

[[nodiscard]] double CalculateRootIterativeMethod(double k, double tolerance, double x0, int& iterationCount);
[[nodiscard]] double CalculateRootNewtonsMethod(double k, double tolerance, double x0, int& iterationCount);
[[nodiscard]] double CalculateRootBisectionMethod(double k, double tolerance, double l, double r, int& iterationCount);

void ExecuteIterativeMethod();
void ExecuteNewtonsMethod();
void ExecuteBisectionMethod();

void ExecuteMethod(EquationMethod method);
void StartMainLoop();
}  // namespace EquationCalculator
