#pragma once

#include "Matrix.h"

namespace MatrixPrint {
template<typename VAL>
void PrintMatrix(Matrix::Matrix<VAL>& m, int precision);

template<typename VAL>
void Printer(int firstColumn, Matrix::Matrix<VAL>& m, int precision, int width, bool);

template<typename VAL>
void PrintGaussianEliminatonWorkflow(Matrix::Matrix<VAL>&, Matrix::Matrix<VAL>&, Matrix::Matrix<VAL>&, int);
}  // namespace MatrixPrint
