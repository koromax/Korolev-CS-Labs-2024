#pragma once

namespace MatrixPrint {

template<typename VAL>
void PrintMatrix(VAL** m, int precision, int rows, int columns);

template<typename VAL>
void Printer(int firstColumn, VAL** m, int precision, int width, int rows, int columns);
}  // namespace MatrixPrint
