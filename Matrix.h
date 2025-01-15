#pragma once

namespace Matrix {
template<typename VAL>
struct Matrix {
    int rows{};
    int columns{};
    VAL** matrix{};

    Matrix();
    Matrix(int n, int m);
    Matrix(Matrix&);
    ~Matrix();

    inline VAL& operator()(int x, int y) { return matrix[x][y]; };

    Matrix<VAL>& operator=(const Matrix<VAL>&);
    Matrix<VAL>& operator*=(Matrix<VAL>& m);

    void SetSize(int n, int m);
    void SetToOne();
    void SetElement(int i, int j, VAL value);
    VAL GetElement(int i, int j);
    void SwapRows(int i, int j);
};

template<typename VAL>
Matrix<VAL> operator*(Matrix<VAL>& m1, Matrix<VAL>& m2) {
    Matrix temp(m1);
    return (temp *= m2);
}
// template<typename VAL>
// Matrix<int> operator*(const Matrix<VAL>& m1, const Matrix<VAL>& m2);

}  // namespace Matrix
