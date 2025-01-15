#include "Gauss.h"
#include "Matrix.h"
#include "MatrixPrint.h"

#include <iostream>

namespace {
void RemoveFakeMinuses(Matrix::Matrix<double>& m, double eps = 1e-10) {
    for (int i = 0; i < m.columns; i++) {
        for (int j = 0; j < m.rows; j++) {
            if (std::abs(m.GetElement(i, j)) < eps) {
                m.SetElement(i, j, 0.);
            }
        }
    }
}

void InitExampleMatrice(Matrix::Matrix<double>& A) {
    A.SetSize(3, 3);
    double setmatrice[9] = {2, 5, 7, 3, 9, 15, 5, 16, 20};
    for (int i = 0; i < A.rows; ++i) {
        for (int j = 0; j < A.columns; ++j) {
            A.SetElement(i, j, setmatrice[i * 3 + j]);
        }
    }
}
}  // namespace

namespace Gauss {
bool GaussianElimination(Matrix::Matrix<double>& A, Matrix::Matrix<double>& B, const bool showSteps = false) {
    // lower triangle
    for (int main_row = 0; main_row < A.rows; ++main_row) {
        // dividing row by first element
        double k = A.GetElement(main_row, main_row);
        if (k == 0) {
            for (int row = main_row + 1; row < A.rows; ++row) {
                k = A.GetElement(row, main_row);
                if (k != 0) {
                    A.SwapRows(main_row, row);
                    B.SwapRows(main_row, row);
                    break;
                }
            }
            if (k == 0) {
                return false;
            }
        }
        for (int column = 0; column < A.columns; ++column) {
            A.SetElement(main_row, column, A.GetElement(main_row, column) / k);
            B.SetElement(main_row, column, B.GetElement(main_row, column) / k);
        }

        // subtracting
        for (int row = main_row + 1; row < A.rows; ++row) {
            double k = A.GetElement(row, main_row);

            for (int column = 0; column < A.columns; ++column) {
                A.SetElement(row, column, A.GetElement(row, column) - k * A.GetElement(main_row, column));
                B.SetElement(row, column, B.GetElement(row, column) - k * B.GetElement(main_row, column));
            }
        }
    }

    if (showSteps) {
        MatrixPrint::PrintMatrix(A, 3);
        MatrixPrint::PrintMatrix(B, 3);
    }

    // upper triangle
    for (int main_row = A.rows - 1; main_row > 0; --main_row) {
        for (int row = main_row - 1; row >= 0; --row) {
            double k = A.GetElement(row, main_row);

            for (int column = 0; column < A.columns; ++column) {
                A.SetElement(row, column, A.GetElement(row, column) - k * A.GetElement(main_row, column));
                B.SetElement(row, column, B.GetElement(row, column) - k * B.GetElement(main_row, column));
            }
        }
    }

    if (showSteps) {
        MatrixPrint::PrintMatrix(A, 3);
        MatrixPrint::PrintMatrix(B, 3);
    }

    return true;
}

void StartMainProgramm() {
    std::cout << "Run algorithm with matrice from doc (Y) or user input (N)? : ";
    char c = 'N';
    std::cin >> c;

    Matrix::Matrix<double> A;
    if (c == 'N' || c == 'n') {
        int n = 0;
        std::cout << "Enter matrix dimension: ";
        std::cin >> n;
        A.SetSize(n, n);

        std::cout << "Enter " << n * n << " values to set matrice (left-to-right, top-to-bottom): ";
        double val = 0.;
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                std::cin >> val;
                A.SetElement(i, j, val);
            }
        }
    } else {
        InitExampleMatrice(A);
    }

    Matrix::Matrix<double> B(A.rows, A.columns);
    B.SetToOne();

    Matrix::Matrix<double> checkA = A;
    if (!GaussianElimination(A, B, true)) {
        std::cout << "Matrix doesn't have an inverse" << '\n';
        return;
    }
    RemoveFakeMinuses(A);
    RemoveFakeMinuses(B);
    Matrix::Matrix<double> AB = checkA * B;
    RemoveFakeMinuses(AB);
    MatrixPrint::PrintGaussianEliminatonCheck(checkA, B, AB, 3);
}
}  // namespace Gauss
