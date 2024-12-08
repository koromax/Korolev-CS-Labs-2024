#pragma once

#include "Matrix.h"

namespace MatrixPrint {
template<typename VAL>
void PrintMatrixFull(Matrix::Matrix<VAL>& m, int precision);

template<typename VAL>
void PrintMatrix(Matrix::Matrix<VAL>& m, int precision);

template<typename VAL>
void Printer(int firstColumn, Matrix::Matrix<VAL>& m, int precision, int width);
}  // namespace MatrixPrint
