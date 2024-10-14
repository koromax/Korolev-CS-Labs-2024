#include <cmath>
#include <iomanip>
#include <iostream>
#include <numbers>

namespace {
const int kFirstTask = 1;
const int kSecondTask = 2;
const int kThirdTask = 3;
const int kFourthTask = 4;

const int kProgressionStep = 5;

const int kFirstMemberOfSequence = 1;
const int kNonNegativeASequenceEnd = 8;
const int kNonNegativeASequenceStep = 2;
const int kNegativeASequenceEnd = 9;
const int kNegativeASequenceStep = 3;
const int kNegativeAConstant = 2;

const double kQuarterPi = std::numbers::pi / 4;
const double kTaylorSequenceXEnd = 1.0;
const double kTaylorSequenceXStep = 0.2;

const double kPrecision = 1e-7;
const double kMaxX = 1.0;
const int kFirstIterationToPrint = 3;
const int kSecondIterationToPrint = 5;
const int kThirdIterationToPrint = 10;

const int kDefaultPrecision = 6;
const int kXColumnWidth = 4;
const int kSColumnWidth = 10;
const int kYColumnWidth = 10;
const int kNColumnWidth = 5;
const int kXPrecision = 1;
const int kSPrecision = 6;
const int kYPrecision = 6;
const int kNPrecision = 0;
}  // namespace

void PrintMenu() {
    std::cout << "Выберите задачу:\n\n";
    std::cout << "1. Вычисление суммы натуральных чисел от 1 до n\n";
    std::cout << "2. Вычисление S\n";
    std::cout << "3. Вычисление ряда Тейлора\n";
    std::cout << "4. Вычисление y\n\n";

    std::cout << "0. Завершить исполнение программы\n";
}

void FirstTask() {
    int n = 0;
    int m = 0;

    std::cout << "Задача 1. Сумма натуральных чисел на отрезке от 1 до n, делящихся на 5 и не делящихся на m (m < n)\n";
    std::cout << "Введите n, m (n > 0, m < n): ";
    std::cin >> n;
    if (n <= 0) {
        std::cout << "Введено недопустимое значение n. Прекращение выполнения задачи...\n\n";
        return;
    }

    std::cin >> m;
    if (m <= 0 || m > n) {
        std::cout << "Введенное число не принадлежит отрезку (0, n). Прекращение выполнения задачи...\n\n";
        return;
    }

    int sum = 0;
    for (int i = kProgressionStep; i <= n; i += kProgressionStep) {
        if (i % m != 0) {
            sum += i;
        }
    }

    std::cout << "Сумма натуральных чисел от 1 до " << n << ", делящихся на 5 и не делящихся на " << m << " равняется: " << sum << "\n\n";
}

void SecondTask() {
    std::cout << "Задача 2. Посчитать а\n";
    std::cout << "Введите число a (любое): ";
    double a = 0;
    std::cin >> a;

    double productOfSequence = kFirstMemberOfSequence;
    if (a >= 0) {
        for (int i = kNonNegativeASequenceStep; i <= kNonNegativeASequenceEnd; i += kNonNegativeASequenceStep) {
            productOfSequence *= i * i;
        }
        productOfSequence -= a;
    } else {
        for (int i = kNegativeASequenceStep; i <= kNegativeASequenceEnd; i += kNegativeASequenceStep) {
            productOfSequence *= (i - kNegativeAConstant);
        }
    }
    std::cout << std::fixed << "Результат выполнения задачи: " << productOfSequence << "\n\n";
}

void ThirdTask() {
    std::cout << "Задача 3. Ряд Тейлора\n";
    std::cout << "Вывод программы:\n";
    std::cout << std::fixed;
    std::cout << std::setw(kXColumnWidth) << std::setprecision(kXPrecision) << "x" << ' ';
    std::cout << std::setw(kYColumnWidth) << std::setprecision(kYPrecision) << "Y(x)" << ' ';
    std::cout << std::setw(kSColumnWidth) << std::setprecision(kSPrecision) << "S(x)" << ' ';
    std::cout << std::setw(kNColumnWidth) << std::setprecision(kNPrecision) << "n" << '\n';

    double x = 0.0;
    while (x <= kTaylorSequenceXEnd) {
        double s = kFirstMemberOfSequence;
        double memberOfSequence = std::cos(kQuarterPi) * x;
        s += memberOfSequence;
        int n = 1;
        while (memberOfSequence >= kPrecision) {
            memberOfSequence *= x / n;
            s += memberOfSequence;
            ++n;
        }

        double y = 1 + x * std::exp(x) * std::cos(kQuarterPi);

        std::cout << std::setw(kXColumnWidth) << std::setprecision(kXPrecision) << x << ' ';
        std::cout << std::setw(kYColumnWidth) << std::setprecision(kYPrecision) << y << ' ';
        std::cout << std::setw(kSColumnWidth) << std::setprecision(kSPrecision) << s << ' ';
        std::cout << std::setw(kNColumnWidth) << std::setprecision(kNPrecision) << n << '\n';

        x += kTaylorSequenceXStep;
    }

    std::cout << '\n';
}

void FourthTask() {
    std::cout << std::fixed << std::setprecision(kDefaultPrecision);
    std::cout << "Задача 4. Вычислить y\n";
    std::cout << "Введите x (|x| < 1): ";
    double x = 0;
    std::cin >> x;
    if (std::abs(x) >= kMaxX) {
        std::cout << "Введено недопустимое значение x. Прекращение выполнения задачи...\n\n";
        return;
    }

    std::cout << "Введите n (n > 0): ";
    int n = 0;
    std::cin >> n;
    if (n <= 0) {
        std::cout << "Введено недопустимое значение n. Прекращение выполнения задачи...\n\n";
        return;
    }

    double memberOfSequence = x;
    double y = memberOfSequence;

    for (int i = 2; i < n; ++i) {
        memberOfSequence *= (-1) * x * (i - 1) / i;
        y += memberOfSequence;
        if (i == kFirstIterationToPrint || i == kSecondIterationToPrint || i == kThirdIterationToPrint) {
            std::cout << "При n == " << i << ", y = " << y << "\n";
        }
    }
    std::cout << "При n == " << n << ", y = " << y << "\n";
}

int main(int, char**) {
    std::cout << "\x1B[2J\x1B[H";
    char continueExecution = 'Y';
    while (continueExecution == 'Y' || continueExecution == 'y') {
        PrintMenu();
        int selectedTask = 0;
        std::cin >> selectedTask;
        std::cout << "\x1B[2J\x1B[H";
        switch (selectedTask) {
            case kFirstTask:
                FirstTask();
                break;
            case kSecondTask:
                SecondTask();
                break;
            case kThirdTask:
                ThirdTask();
                break;
            case kFourthTask:
                FourthTask();
                break;
            default:
                std::cout << "Программа завершена.\n";
                return 0;
        }

        std::cout << "Желаете продолжить исполнение программы (y/n)?: ";
        continueExecution = 'n';
        std::cin >> continueExecution;
    }

    std::cout << "Программа закончила исполнение.\n";
    return 0;
}
