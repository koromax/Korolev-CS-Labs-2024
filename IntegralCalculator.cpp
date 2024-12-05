#include "IntegralCalculator.h"

#include <cmath>
#include <iomanip>
#include <iostream>

namespace {
const int kMaxPartitions = 100000;
const int kAmountOfFunctions = 5;
const int kColumnWidths[4] = {10, 15, 15, 7};
const double epses[5] = {1e-2, 1e-3, 1e-4, 1e-5, 1e-6};
const char* funcs[kAmountOfFunctions] = {"y=x", "y=sin(22x)", "y=x^4", "y=arctg(x)", "y=cos(x)"};

[[nodiscard]] double F1(const double x) {
    return x;
}

[[nodiscard]] double F2(const double x) {
    return std::sin(22 * x);
}

[[nodiscard]] double F3(const double x) {
    return x * x * x * x;
}

[[nodiscard]] double F4(const double x) {
    return std::atan(x);
}

[[nodiscard]] double F5(const double x) {
    return std::cos(x);
}

[[nodiscard]] double IntegralF1(const double a, const double b) {
    return (b * b - a * a) / 2.0;
}

[[nodiscard]] double IntegralF2(const double a, const double b) {
    return (cos(a * 22.0) - cos(b * 22.0)) / 22.0;
}

[[nodiscard]] double IntegralF3(const double a, const double b) {
    return (b * b * b * b * b - a * a * a * a * a) / 5.0;
}

[[nodiscard]] double IntegralF4(const double a, const double b) {
    return b * atan(b) - a * atan(a) - (log(b * b + 1) - log(a * a + 1)) / 2.0;
}
[[nodiscard]] double IntegralF5(const double a, const double b) {
    return std::sin(b) - std::sin(a);
}

[[nodiscard]] double ReadAFromStdin() {
    std::cout << "Введите значение a: ";
    double a = 0.0;
    std::cin >> a;
    return a;
}

[[nodiscard]] double ReadBFromStdin() {
    std::cout << "Введите значение b: ";
    double b = 0.0;
    std::cin >> b;
    return b;
}

[[nodiscard]] int EpsToPrecisionConversion(const double eps) {
    return static_cast<int>(std::ceil(-(std::log10(eps))));
}

void PrintTable(IntegralCalculator::Result results[4], IntegralCalculator::Method method, double eps) {
    int precision = EpsToPrecisionConversion(eps);
    std::cout << std::fixed << std::setprecision(precision);
    switch (method) {
        case IntegralCalculator::Method::IntRect:
            std::cout << "Riemann sum - Left Rule. ";
            break;
        case IntegralCalculator::Method::IntTrap:
            std::cout << "Riemann sum - Trapezoidal Rule. ";
            break;
    }
    std::cout << "eps = " << eps << '\n';
    std::cout << " ---------------------------------------------------------- " << '\n';
    std::cout << "|  Function  |     Integral    |      IntSum     |    N    |" << '\n';

    for (int i = 0; i < kAmountOfFunctions; ++i) {
        std::cout << "| ---------- + --------------- + --------------- + ------- |" << '\n';
        std::cout << "| " << std::setw(kColumnWidths[0]) << std::left << funcs[i] << " | " << std::setw(kColumnWidths[1]) << std::right
                  << results[i].valueExact << " | " << std::setw(kColumnWidths[2]) << results[i].valueSum << " | " << std::setw(kColumnWidths[3])
                  << results[i].partitions << " |\n";
    }

    std::cout << " ---------------------------------------------------------- " << '\n';
    std::cout << '\n';
}

}  // namespace

namespace IntegralCalculator {
Result IntRect(const F f, const double a, const double b, const double eps) {
    Result result = {};
    result.partitions = 1;
    double dx = (b - a) / result.partitions;
    result.valueSum = f(dx / 2) * dx;

    double prevResult = 0.0;
    while (std::fabs(prevResult - result.valueSum) / 3 > eps) {
        prevResult = result.valueSum;
        result.valueSum = 0.0;
        result.partitions *= 2;

        if (result.partitions > kMaxPartitions) {
            result.valueSum = NAN;
            return result;
        }

        dx = (b - a) / result.partitions;
        for (int i = 0; i < result.partitions; ++i) {
            result.valueSum += f(a + dx / 2 + i * dx);
        }
        result.valueSum *= dx;
    }

    return result;
}

Result IntTrap(const F f, const double a, const double b, const double eps) {
    Result result = {};
    result.partitions = 1;
    result.valueSum = (f(a) + f(b)) / 2;

    double prevResult = 0.0;
    while (std::fabs(prevResult - result.valueSum) / 3 > eps) {
        prevResult = result.valueSum;
        result.valueSum = (f(a) + f(b)) / 2;
        result.partitions *= 2;

        if (result.partitions > kMaxPartitions) {
            result.valueSum = NAN;
            return result;
        }

        double dx = (b - a) / result.partitions;

        for (int i = 1; i < result.partitions; ++i) {
            result.valueSum += f(a + i * dx);
        }
        result.valueSum *= dx;
    }

    return result;
}

void StartMainProgram(int argc, char** argv) {
    double a = 0.0;
    double b = 0.0;

    switch (argc) {
        case 3:
            a = std::atof(argv[1]);
            b = std::atof(argv[2]);
            break;
        case 2:
            a = std::atof(argv[1]);
            b = ReadBFromStdin();
            break;
        default:
            a = ReadAFromStdin();
            b = ReadBFromStdin();
            break;
    }

    const F fefes[kAmountOfFunctions] = {F1, F2, F3, F4, F5};
    const I integrals[kAmountOfFunctions] = {IntegralF1, IntegralF2, IntegralF3, IntegralF4, IntegralF5};

    Result results[kAmountOfFunctions];
    for (double eps : epses) {
        for (int i = 0; i < kAmountOfFunctions; ++i) {
            results[i] = IntRect(fefes[i], a, b, eps);
            results[i].valueExact = integrals[i](a, b);
        }
        PrintTable(results, Method::IntRect, eps);
    }

    for (double eps : epses) {
        for (int i = 0; i < kAmountOfFunctions; ++i) {
            results[i] = IntTrap(fefes[i], a, b, eps);
            results[i].valueExact = integrals[i](a, b);
        }
        PrintTable(results, Method::IntTrap, eps);
    }
}
}  // namespace IntegralCalculator
