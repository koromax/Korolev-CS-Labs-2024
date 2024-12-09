#pragma once

namespace Matrix {
template<typename VAL>
struct Matrix {
    int rows{};
    int columns{};
    VAL** matrix{};

    Matrix(int n, int m);
    ~Matrix();

    void SetElement(int i, int j, VAL value);
    VAL GetElement(int i, int j);
};
}  // namespace Matrix
