#include "Vector.h"

#include <algorithm>
#include <iostream>
#include <stdexcept>

namespace {
const size_t kDefaultVectorSize = 8;
const size_t kGrowthFactor = 2;

void ResizeVector(Vector::VecInt& vector) {
    if (!vector.vector) {
        throw std::runtime_error("invalid vector");
    }

    size_t newVectorSize = vector.capacity * kGrowthFactor;
    int* buffer = new int[newVectorSize];

    std::copy(vector.vector, vector.vector + vector.size, buffer);

    int* oldVector = vector.vector;
    vector.vector = buffer;

    vector.capacity = newVectorSize;

    delete[] oldVector;
}
}  // namespace

namespace Vector {
VecInt CreateVector() {
    int* array = new int[kDefaultVectorSize];

    return {.size = 0, .capacity = kDefaultVectorSize, .vector = array};
}

void DeleteVector(VecInt& vector) {
    vector.size = 0;
    vector.capacity = 0;

    delete[] vector.vector;

    vector.vector = nullptr;
}

int& GetElement(VecInt& vector, size_t index) {
    if (index >= vector.size) {
        throw std::runtime_error("index out of bound");
    }

    if (!vector.vector) {
        throw std::runtime_error("invalid vector");
    }

    return vector.vector[index];
}

int GetElement(const VecInt& vector, size_t index) {
    if (index >= vector.size) {
        throw std::runtime_error("index out of bound");
    }

    if (!vector.vector) {
        throw std::runtime_error("invalid vector");
    }

    return vector.vector[index];
}

void PushBack(VecInt& vector, int element) {
    if (!vector.vector) {
        throw std::runtime_error("invalid vector");
    }

    if (vector.size >= vector.capacity) {
        ResizeVector(vector);
    }

    vector.vector[vector.size++] = element;
}
}  // namespace Vector
