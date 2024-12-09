#include "MatrixTask.h"
#include "MatrixPrint.h"

#include <cmath>
#include <iomanip>
#include <iostream>

namespace {
[[nodiscard]] int ReadNFromStdin() {
    std::cout << "Enter n (8 <= n <= 15): ";
    int n = 0;
    std::cin >> n;

    if (n < 8 || n > 15) {
        std::cout << "Entered value is outside the allowed range. Defaulted to 10.";
        n = 10;
    }

    return n;
}

[[nodiscard]] int ReadMFromStdin() {
    std::cout << "Enter m (8 <= n <= 15): ";
    int m = 0;
    std::cin >> m;

    if (m < 8 || m > 15) {
        std::cout << "Entered value is outside the allowed range. Defaulted to 10.";
        m = 10;
    }

    return m;
}

[[nodiscard]] int ScientificToNormalPrecision(double precision) {
    if (std::ceil(precision) == std::floor(precision)) {
        return static_cast<int>(precision);
    }
    std::cout << "nerd." << '\n';
    return static_cast<int>(std::ceil(-(log10(precision))));
}

[[nodiscard]] double ReadPrecisionFromStdin() {
    std::cout << "Enter precision (3 <= p <= 8): ";
    double precision = 0;
    std::cin >> precision;

    precision = ScientificToNormalPrecision(precision);

    if (precision < 3 || precision > 8) {
        std::cout << "Entered value is outside the allowed range. Defaulted to 5.";
        precision = 5;
    }

    return precision;
}

[[nodiscard]] double Factorial(int n) {
    double i = 1;
    while (n > 1)
        i *= n--;
    return i;
}

[[nodiscard]] double FormulaTop(int i, int j, int x) {
    return std::pow(x + 1, i + 1) / std::pow(Factorial(j + 1), i + 1);
}

[[nodiscard]] double FormulaBot(int i, int j, int x) {
    return std::pow(-(x + 1), i + 1) / std::pow(Factorial(j + 1), i + 1);
}

void FillMatrix(double** m, int rows, int columns) {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < columns; ++j) {
            if (i == j) {
                continue;
            }

            if (i < j) {
                m[i][j] = FormulaTop(i, j, 1);
            } else {
                m[i][j] = FormulaBot(i, j, 1);
            }
        }
    }
}

[[maybe_unused]] void WeirdOutput(int B[10][10]) {
    bool i;
    std::cout << B << std::endl;
    std::cin >> i;
    std::cout << "  " << B[2] << "  " << B[0] << std::endl;
    #pragma clang diagnostic ignored "-Warray-bounds"
    std::cout << B[0][20] << "  " << *(*B + 22) << "  " << *B[5] << std::endl;
    std::cout << *(*(B + 4)) << "  " << *(B[9] + 7) << std::endl;
    std::cout << *(B + 5) << "  " << *(*&B + 1) << std::endl;
    #pragma clang diagnostic ignored "-Warray-bounds"
    std::cout << B[0][20] << "  " << *(B[0] + 20) << "  " << *B[2] << std::endl;
}
}  // namespace

namespace MatrixTask {
void StartMainProgram(int argc, char** argv) {
    int rows = 10;
    int columns = 10;
    #pragma clang diagnostic ignored "-Wunused-but-set-variable"
    int precision = 5;
    switch (argc) {
        case 4:
            rows = std::atoi(argv[1]);
            columns = std::atoi(argv[2]);
            precision = ScientificToNormalPrecision(std::atof(argv[3]));
            break;
        case 3:
            rows = std::atoi(argv[1]);
            columns = std::atoi(argv[2]);
            precision = ReadPrecisionFromStdin();
            break;
        default:
            rows = ReadNFromStdin();
            columns = ReadMFromStdin();
            precision = ReadPrecisionFromStdin();
            break;
    }

    double **matrix = new double*[rows];
    for (int i = 0; i < rows; ++i) {
        matrix[i] = new double[columns];
    }

    int min = 0;
    rows <= columns ? min = rows : min = columns;
    for (int i = 0; i < min; ++i) {
        matrix[i][i] = 1.;
    }
    FillMatrix(matrix, rows, columns);

    MatrixPrint::PrintMatrix(matrix, precision, rows, columns);

    int B[10][10] = {};
    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 10; ++j) {
            B[i][j] = i * 10 + j;
        }
    }

    int* Bp[10] = {};
    for (int i = 0; i < 10; ++i) {
        Bp[i] = B[i];
    }

    // WeirdOutput(B);

    MatrixPrint::PrintMatrix(Bp, precision, 10, 10);
}
}  // namespace MatrixTask
