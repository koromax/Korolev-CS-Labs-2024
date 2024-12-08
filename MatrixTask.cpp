#include "MatrixTask.h"
#include "Matrix.h"
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

[[nodiscard]] int ReadPrecisionFromStdin() {
    std::cout << "Enter precision (3 <= p <= 8): ";
    int precision = 0;
    std::cin >> precision;

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

template<typename VAL>
[[nodiscard]] double FormulaTop(int i, int j, int x) {
    return std::pow(x + 1, i + 1) / std::pow(Factorial(j + 1), i + 1);
}

template<typename VAL>
[[nodiscard]] double FormulaBot(int i, int j, int x) {
    return std::pow(-(x + 1), i + 1) / std::pow(Factorial(j + 1), i + 1);
}

template<typename VAL>
void FillMatrix(Matrix::Matrix<VAL>& m) {
    for (int i = 0; i < m.rows; ++i) {
        for (int j = 0; j < m.columns; ++j) {
            if (i == j) {
                continue;
            }

            if (i < j) {
                m.SetElement(i, j, FormulaTop<VAL>(i, j, 1));
            } else {
                m.SetElement(i, j, FormulaBot<VAL>(i, j, 1));
            }
        }
    }
}

void WeirdOutput(int B[10][10]) {
    std::cout << B << "  " << B[0] << "  " << B[2] << std::endl;
    std::cout << B[0][0] << "  " << **B << "  " << *B[0] << std::endl;
    std::cout << *(*(B + 1)) << "  " << *B[1] << std::endl;
    std::cout << *(B[0] + 1) << "  " << *(*B + 1) << std::endl;
#pragma clang diagnostic ignored "-Warray-bounds"
    std::cout << B[0][20] << "  " << *(B[0] + 20) << "  " << *B[2] << std::endl;
}
}  // namespace

namespace MatrixTask {
void StartMainProgram(int argc, char** argv) {
    int n = 10;
    int m = 10;
    int precision = 5;
    switch (argc) {
        case 4:
            n = std::atoi(argv[1]);
            m = std::atoi(argv[2]);
            precision = std::atoi(argv[3]);
            break;
        case 3:
            n = std::atoi(argv[1]);
            m = std::atoi(argv[2]);
            precision = ReadPrecisionFromStdin();
            break;
        default:
            n = ReadNFromStdin();
            m = ReadMFromStdin();
            precision = ReadPrecisionFromStdin();
            break;
    }

    using VAL = double;
    Matrix::Matrix<VAL> matrix;
    matrix.SetSize(n, m);

    int dem = 0;
    n <= m ? dem = n : dem = m;
    for (int i = 0; i < dem; ++i) {
        matrix.SetElement(i, i, 1.);
    }
    FillMatrix(matrix);

    MatrixPrint::PrintMatrix(matrix, precision);

    matrix.~Matrix();

    int B[10][10] = {};
    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 10; ++j) {
            B[i][j] = i * 10 + j;
        }
    }

    WeirdOutput(B);
}
}  // namespace MatrixTask
