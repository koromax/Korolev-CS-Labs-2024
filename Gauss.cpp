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
}  // namespace

namespace Gauss {
void GaussianElimination(Matrix::Matrix<double>& A, Matrix::Matrix<double>& B) {
    // lower triangle
    for (int main_row = 0; main_row < A.rows; ++main_row) {
        // dividing row by first element
        double k = A.GetElement(main_row, main_row);
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
}

void StartMainProgramm() {
    int n = 3;
    int m = 3;

    Matrix::Matrix<double> A(n, m);
    Matrix::Matrix<double> B(n, m);
    B.SetToOne();

    double setmatrice[9] = {2, 5, 7, 3, 9, 15, 5, 16, 20};
    for (int i = 0; i < A.rows; ++i) {
        for (int j = 0; j < A.columns; ++j) {
            A.SetElement(i, j, setmatrice[i * 3 + j]);
        }
    }

    Matrix::Matrix<double> checkA = A;
    GaussianElimination(A, B);
    RemoveFakeMinuses(A);
    RemoveFakeMinuses(B);
    Matrix::Matrix<double> AB = checkA * B;
    RemoveFakeMinuses(AB);
    MatrixPrint::PrintGaussianEliminatonWorkflow(checkA, B, AB, 3);
}
}  // namespace Gauss
