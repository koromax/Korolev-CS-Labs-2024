#include "IntegralCalculator.h"

#include <cmath>
#include <iomanip>
#include <iostream>

namespace {
const int kMaxPartitions = 100000;
const int kIntegralColumnWidth = 17;
const int kIntSumColumnWidth = 17;
const int kNColumnWidth = 9;
const double epses[5] = {1e-2, 1e-3, 1e-4, 1e-5, 1e-6};

[[nodiscard]] double F1(double x) {
    return x;
}

[[nodiscard]] double F2(double x) {
    return std::sin(22 * x);
}

[[nodiscard]] double F3(double x) {
    return x * x * x * x;
}

[[nodiscard]] double F4(double x) {
    return std::atan(x);
}

[[nodiscard]] double IntegralF1(double a, double b) {
    return (b * b - a * a) / 2.0;
}

[[nodiscard]] double IntegralF2(double a, double b) {
    return (cos(a * 22.0) - cos(b * 22.0)) / 22.0;
}

[[nodiscard]] double IntegralF3(double a, double b) {
    return (b * b * b * b * b - a * a * a * a * a) / 5.0;
}

[[nodiscard]] double IntegralF4(double a, double b) {
    return b * atan(b) - a * atan(a) - (log(b * b + 1) - log(a * a + 1)) / 2.0;
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

[[nodiscard]] int EpsToPrecisionConversion(double eps) {
    return static_cast<int>(std::ceil(-(std::log10(eps))));
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
    result.valueSum = f(a) + f(b);

    double prevResult = 0.0;
    while (std::fabs(prevResult - result.valueSum) / 3 > eps) {
        prevResult = result.valueSum;
        result.valueSum = f(a) + f(b);
        result.partitions *= 2;

        if (result.partitions > kMaxPartitions) {
            result.valueSum = NAN;
            return result;
        }

        double dx = (b - a) / result.partitions;

        for (int i = 1; i < result.partitions; ++i) {
            result.valueSum += 2 * f(a + i * dx);
        }
        result.valueSum *= dx / 2;
    }

    return result;
}

void StartMainProgram(int argc, char** argv) {
    double a = 0.0;
    double b = 0.0;

    switch (argc) {
        case 3:
            a = atof(argv[1]);
            b = atof(argv[2]);
            break;
        case 2:
            a = atof(argv[1]);
            b = ReadBFromStdin();
            break;
        default:
            a = ReadAFromStdin();
            b = ReadBFromStdin();
            break;
    }

    F fefes[4] = {F1, F2, F3, F4};
    I integrals[4] = {IntegralF1, IntegralF2, IntegralF3, IntegralF4};

    for (double eps : epses) {
        int precision = EpsToPrecisionConversion(eps);

        std::cout << std::setprecision(precision);
        std::cout << std::fixed << std::right << "Riemann sum - Left Rule. eps = " << eps << '\n';
        Result results[4];
        for (int i = 0; i < 4; ++i) {
            results[i] = IntRect(fefes[i], a, b, eps);
            results[i].valueExact = integrals[i](a, b);
        }
        std::cout << " --------------------------------------------------------- " << '\n';
        std::cout << "|  Function |     Integral    |      IntSum     |    N    |" << '\n';
        std::cout << "| --------- + --------------- + --------------- + ------- |" << '\n';
        std::cout << "|    y=x    |" << std::setprecision(precision) << std::setw(kIntegralColumnWidth) << results[0].valueExact << '|'
                  << std::setw(kIntSumColumnWidth) << results[0].valueSum << '|' << std::setw(kNColumnWidth) << results[0].partitions << "|\n";
        std::cout << "| --------- + --------------- + --------------- + ------- |" << '\n';
        std::cout << "| y=sin(22x)|" << std::setprecision(precision) << std::setw(kIntegralColumnWidth) << results[1].valueExact << '|'
                  << std::setw(kIntSumColumnWidth) << results[1].valueSum << '|' << std::setw(kNColumnWidth) << results[1].partitions << "|\n";
        std::cout << "| --------- + --------------- + --------------- + ------- |" << '\n';
        std::cout << "|   y=x^4   |" << std::setprecision(precision) << std::setw(kIntegralColumnWidth) << results[2].valueExact << '|'
                  << std::setw(kIntSumColumnWidth) << results[2].valueSum << '|' << std::setw(kNColumnWidth) << results[2].partitions << "|\n";
        std::cout << "| --------- + --------------- + --------------- + ------- |" << '\n';
        std::cout << "| y=arctg(x)|" << std::setprecision(precision) << std::setw(kIntegralColumnWidth) << results[3].valueExact << '|'
                  << std::setw(kIntSumColumnWidth) << results[3].valueSum << '|' << std::setw(kNColumnWidth) << results[3].partitions << "|\n";
        std::cout << " --------------------------------------------------------- " << '\n';
        std::cout << '\n';
    }

    for (double eps : epses) {
        int precision = EpsToPrecisionConversion(eps);

        std::cout << std::setprecision(precision);
        std::cout << std::fixed << std::right << "Riemann sum - Trapezoidal Rule. eps = " << eps << '\n';
        Result results[4];
        for (int i = 0; i < 4; ++i) {
            results[i] = IntTrap(fefes[i], a, b, eps);
            results[i].valueExact = integrals[i](a, b);
        }
        std::cout << " --------------------------------------------------------- " << '\n';
        std::cout << "|  Function |     Integral    |      IntSum     |    N    |" << '\n';
        std::cout << "| --------- + --------------- + --------------- + ------- |" << '\n';
        std::cout << "|    y=x    |" << std::setprecision(precision) << std::setw(kIntegralColumnWidth) << results[0].valueExact << '|'
                  << std::setw(kIntSumColumnWidth) << results[0].valueSum << '|' << std::setw(kNColumnWidth) << results[0].partitions << "|\n";
        std::cout << "| --------- + --------------- + --------------- + ------- |" << '\n';
        std::cout << "| y=sin(22x)|" << std::setprecision(precision) << std::setw(kIntegralColumnWidth) << results[1].valueExact << '|'
                  << std::setw(kIntSumColumnWidth) << results[1].valueSum << '|' << std::setw(kNColumnWidth) << results[1].partitions << "|\n";
        std::cout << "| --------- + --------------- + --------------- + ------- |" << '\n';
        std::cout << "|   y=x^4   |" << std::setprecision(precision) << std::setw(kIntegralColumnWidth) << results[2].valueExact << '|'
                  << std::setw(kIntSumColumnWidth) << results[2].valueSum << '|' << std::setw(kNColumnWidth) << results[2].partitions << "|\n";
        std::cout << "| --------- + --------------- + --------------- + ------- |" << '\n';
        std::cout << "| y=arctg(x)|" << std::setprecision(precision) << std::setw(kIntegralColumnWidth) << results[3].valueExact << '|'
                  << std::setw(kIntSumColumnWidth) << results[3].valueSum << '|' << std::setw(kNColumnWidth) << results[3].partitions << "|\n";
        std::cout << " --------------------------------------------------------- " << '\n';
        std::cout << '\n';
    }
}
}  // namespace IntegralCalculator
