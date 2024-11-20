#pragma once

#include <stddef.h>

namespace SortingAlgorithms {
enum class Array : int {
    Static = 1,
    Dynamic
};

enum class Algorithm : int {
    Bubble = 1,
    Selection
};

void BubbleSort(int* a, const size_t& length, size_t& comparisonCount, size_t& swapCount, bool isAscending);
void SelectionSort(int* a, const size_t& length, size_t& comparisonCount, size_t& swapCount, bool isAscending);

void ExecuteStaticArray();
void ExecuteDynamicArray();

void ExecuteArray(SortingAlgorithms::Array arrayType);

void StartMainLoop();
}  // namespace SortingAlgorithms
