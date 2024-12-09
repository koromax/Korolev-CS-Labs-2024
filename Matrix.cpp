#include "Matrix.h"

#include <cmath>
#include <iostream>
#include <stdexcept>

namespace Matrix {

template<typename VAL>
Matrix<VAL>::Matrix(int n, int m) : rows(n), columns(n) {
    matrix = new VAL*[rows];
    for (int i = 0; i < rows; ++i) {
        matrix[i] = new VAL[columns];
    }
    matrix[0][0] = 0.;
}

template<typename VAL>
Matrix<VAL>::~Matrix() {
    for (int i = 0; i < rows; ++i) {
        delete[] matrix[i];
    }
    delete[] matrix;
}

template<typename VAL>
void Matrix<VAL>::SetElement(int i, int j, VAL value) {
    matrix[i][j] = value;
};

template<typename VAL>
VAL Matrix<VAL>::GetElement(int i, int j) {
    if (i >= rows || j >= columns) {
        std::cout << "Specified coordinates are outside of matrix bounds" << '\n';
    }
    return matrix[i][j];
}

template struct Matrix<int>;
template struct Matrix<double>;

}  // namespace Matrix
