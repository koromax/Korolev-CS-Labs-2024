#pragma once

namespace Matrix {
template<typename VAL>
struct Matrix {
    int rows{};
    int columns{};
    VAL** matrix{};

    Matrix();
    ~Matrix();

    void SetSize(int n, int m);
    void SetElement(int i, int j, VAL value);
    VAL GetElement(int i, int j);
};
}  // namespace Matrix
