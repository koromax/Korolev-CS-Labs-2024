#include "Matrix.h"

#include <cmath>
#include <iostream>
#include <stdexcept>

namespace Matrix {

template<typename VAL>
Matrix<VAL>::Matrix() : rows(1), columns(1) {
    matrix = new VAL*[rows];
    for (int i = 0; i < rows; ++i) {
        matrix[i] = new VAL[columns];
    }
    matrix[0][0] = 0.;
}

template<typename VAL>
Matrix<VAL>::Matrix(int n, int m) : rows(n), columns(m) {
    matrix = new VAL*[rows];
    for (int i = 0; i < rows; ++i) {
        matrix[i] = new VAL[columns];
        for (int j = 0; j < columns; ++j) {
            matrix[i][j] = 0.;
        }
    }
}

template<typename VAL>
Matrix<VAL>::Matrix(Matrix& other) : rows(other.rows), columns(other.columns) {
    matrix = new VAL*[rows];
    for (int i = 0; i < rows; ++i) {
        matrix[i] = new VAL[columns];
        for (int j = 0; j < columns; ++j) {
            matrix[i][j] = other.GetElement(i, j);
        }
    }
}

template<typename VAL>
Matrix<VAL>::~Matrix() {
    for (int i = 0; i < rows; ++i) {
        delete[] matrix[i];
    }
    delete[] matrix;
}

template<typename VAL>
Matrix<VAL>& Matrix<VAL>::operator=(const Matrix<VAL>& other) {
    if (this == &other) {
        return *this;
    }

    if (rows != other.rows || columns != other.columns) {
        this->SetSize(other.rows, other.columns);
        rows = other.rows;
        columns = other.columns;
    }

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < columns; ++j) {
            matrix[i][j] = other.matrix[i][j];
        }
    }
    return *this;
}

template<typename VAL>
Matrix<VAL>& Matrix<VAL>::operator*=(Matrix<VAL>& m) {
    Matrix temp(rows, m.columns);
    for (int i = 0; i < temp.rows; ++i) {
        for (int j = 0; j < temp.columns; ++j) {
            for (int k = 0; k < columns; ++k) {
                temp.matrix[i][j] += (matrix[i][k] * m.matrix[k][j]);
            }
        }
    }
    return (*this = temp);
}

template<typename VAL>
void Matrix<VAL>::SetSize(int n, int m) {
    for (int i = 0; i < rows; ++i) {
        delete[] matrix[i];
    }
    delete[] matrix;

    rows = n;
    columns = m;
    matrix = new VAL*[rows];
    for (int i = 0; i < rows; ++i) {
        matrix[i] = new VAL[columns];
    }
}

template<typename VAL>
void Matrix<VAL>::SetToOne() {
    for (int i = 0; i < std::min(columns, rows); ++i) {
        matrix[i][i] = 1.;
    }
}

template<typename VAL>
void Matrix<VAL>::SetElement(int i, int j, VAL value) {
    matrix[i][j] = value;
}

template<typename VAL>
VAL Matrix<VAL>::GetElement(int i, int j) {
    if (i >= rows || j >= columns) {
        std::cout << "Specified coordinates are outside of matrix bounds" << '\n';
    }
    return matrix[i][j];
}

template<typename VAL>
void Matrix<VAL>::SwapRows(int i, int j) {
    if (i == j || i < 0 || i >= rows || j < 0 || j >= rows) {
        std::cout << "Out of bounds" << '\n';
        return;
    }

    for (int column = 0; column < columns; ++column) {
        VAL temp = matrix[i][column];
        matrix[i][column] = matrix[j][column];
        matrix[j][column] = temp;
    }
}

// template struct Matrix<int>;
template struct Matrix<double>;
}  // namespace Matrix
