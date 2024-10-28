#include "EquationCalculator.h"

#include <cmath>
#include <iomanip>
#include <iostream>

namespace {
int const kMaxIterations = 1e5;

[[nodiscard]] double ReadCoefficientFromStdin() {
    std::cout << "Введите коэффициент k: ";
    double coefficient = 1;
    std::cin >> coefficient;
    return coefficient;
}

[[nodiscard]] double ReadToleranceFromStdin() {
    std::cout << "Введите допустимую погрешность (e.g. 1e-10): ";
    double tolerance = 0;
    std::cin >> tolerance;
    return tolerance;
}

[[nodiscard]] double ReadStartingValueFromStdin() {
    std::cout << "Введите начальное значение x0: ";
    double startingValue = 0;
    std::cin >> startingValue;
    return startingValue;
}

[[nodiscard]] double ReadLowerBoundFromStdin() {
    std::cout << "Введите нижнюю границу l: ";
    double lowerBound = 1;
    std::cin >> lowerBound;
    return lowerBound;
}

[[nodiscard]] double ReadUpperBoundFromStdin() {
    std::cout << "Введите вернюю границу r: ";
    double upperBound = 1;
    std::cin >> upperBound;
    return upperBound;
}

void ClearTerminal() {
    std::cout << "\x1B[2J\x1B[H";
}

void PrintMenu() {
    ClearTerminal();
    std::cout << "Выберите метод решения:" << '\n';
    std::cout << "1. Итерационный метод" << '\n';
    std::cout << "2. Метод Ньютона" << '\n';
    std::cout << "3. Метод половинного деления" << '\n';
    std::cout << '\n';
}

[[nodiscard]] int ToleranceToPrecisionConversion(double tolerance) {
    return static_cast<int>(std::ceil(-(log10(tolerance))));
}

[[nodiscard]] EquationCalculator::EquationMethod ReadEquationMethodFromStdin() {
    std::cout << "Ваш выбор: ";
    int method = 0;
    std::cin >> method;
    ClearTerminal();
    return static_cast<EquationCalculator::EquationMethod>(method);
}

[[nodiscard]] bool ContinueExecution() {
    std::cout << "Желаете продолжить выполнение программы? (y/n): ";
    char continueExecution = 'n';
    std::cin >> continueExecution;
    return (continueExecution == 'y' || continueExecution == 'Y');
}

[[nodiscard]] double CalculateFunction(double x, double k) {
    return x - k * std::cos(x);
}

[[nodiscard]] double CalculateDerivativeFunction(double x, double k) {
    return k * std::sin(x) + 1;
}

[[nodiscard]] bool IsEquationRoot(double x, double k, double tolerance) {
    return std::abs(CalculateFunction(x, k)) <= tolerance;
}

[[nodiscard]] bool IsSignsEqual(double x1, double x2) {
    return (x1 > 0.) == (x2 > 0.);
}

[[nodiscard]] double CalculateMiddleX(double l, double r) {
    return l + (r - l) / 2;
}

void PrintSolution(double x, int iterationCount, int precision) {
    if (std::isnan(x)) {
        std::cout << "Корень не найден." << '\n' << '\n';
    } else {
        std::cout << "Результат выполнения: " << std::fixed << std::setprecision(precision) << x << '\n';
        std::cout << "Выполнено за " << iterationCount << " итераций" << '\n' << '\n';
    }
}
}  // namespace

namespace EquationCalculator {
double CalculateRootIterativeMethod(double k, double tolerance, double x, int& iterationCount) {
    iterationCount = 0;
    double x0 = x - 1;
    while (std::abs(x - x0) > tolerance && iterationCount < kMaxIterations) {
        x0 = x;
        x = x - CalculateFunction(x, k);
        ++iterationCount;
    }

    if (!IsEquationRoot(x, k, tolerance)) {
        return NAN;
    }

    return x;
}

double CalculateRootNewtonsMethod(double k, double tolerance, double x, int& iterationCount) {
    iterationCount = 0;
    double x0 = x - 1;
    while (std::abs(x - x0) > tolerance && iterationCount < kMaxIterations) {
        x0 = x;
        x = x - CalculateFunction(x, k) / CalculateDerivativeFunction(x, k);
        ++iterationCount;
    }

    if (!IsEquationRoot(x, k, tolerance)) {
        return NAN;
    }

    return x;
}

double CalculateRootBisectionMethod(double k, double tolerance, double l, double r, int& iterationCount) {
    iterationCount = 0;

    if (l >= r) {
        return NAN;
    }

    double functionValueL = CalculateFunction(l, k);
    double functionValueR = CalculateFunction(r, k);

    if (IsSignsEqual(functionValueL, functionValueR)) {
        return NAN;
    }

    double x = 0.;
    while (r - l > tolerance && iterationCount < kMaxIterations) {
        x = CalculateMiddleX(l, r);

        if (!IsSignsEqual(CalculateFunction(l, k), CalculateFunction(x, k))) {
            r = x;
        } else {
            l = x;
        }

        ++iterationCount;
    }
    return x;
}

void ExecuteIterativeMethod() {
    double k = ReadCoefficientFromStdin();
    double tolerance = ReadToleranceFromStdin();
    double x0 = ReadStartingValueFromStdin();

    int iterationCount = 0;
    double x = CalculateRootIterativeMethod(k, tolerance, x0, iterationCount);
    PrintSolution(x, iterationCount, ToleranceToPrecisionConversion(tolerance));
}

void ExecuteNewtonsMethod() {
    double k = ReadCoefficientFromStdin();
    double tolerance = ReadToleranceFromStdin();
    double x0 = ReadStartingValueFromStdin();

    int iterationCount = 0;
    double x = CalculateRootNewtonsMethod(k, tolerance, x0, iterationCount);
    PrintSolution(x, iterationCount, ToleranceToPrecisionConversion(tolerance));
}

void ExecuteBisectionMethod() {
    double k = ReadCoefficientFromStdin();
    double tolerance = ReadToleranceFromStdin();
    double l = ReadLowerBoundFromStdin();
    double r = ReadUpperBoundFromStdin();

    if (l >= r) {
        std::cout << "Верняя граница диапозона должна быть больше, чем нижняя граница." << '\n';
        return;
    }

    int iterationCount = 0;
    double x = CalculateRootBisectionMethod(k, tolerance, l, r, iterationCount);
    PrintSolution(x, iterationCount, ToleranceToPrecisionConversion(tolerance));
}

void ExecuteMethod(EquationMethod method) {
    switch (method) {
        case EquationMethod::Iterative:
            ExecuteIterativeMethod();
            break;
        case EquationMethod::Newtons:
            ExecuteNewtonsMethod();
            break;
        case EquationMethod::Bisection:
            ExecuteBisectionMethod();
            break;
        default:
            std::cout << "Некорректно введенное значение. Допустимые значения: (1, 2, 3)" << '\n';
            break;
    }
}

void StartMainLoop() {
    bool continueExecution = true;
    while (continueExecution) {
        PrintMenu();
        ExecuteMethod(static_cast<EquationMethod>(ReadEquationMethodFromStdin()));
        continueExecution = ContinueExecution();
    }

    std::cout << "Программа завершена. ᓚᘏᗢ" << '\n';
}
}  // namespace EquationCalculator
