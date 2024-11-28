#include "SortingAlgorithms.h"

#include <cmath>
#include <iomanip>
#include <iostream>
#include <random>

namespace {
const int kRandomDistributionUpperLimit = 100;
const int kPrintArrayToStdoutLengthLimit = 50;
void ClearTerminal() {
    std::cout << "\x1B[2J\x1B[H";
}

void PrintArrayMenu() {
    ClearTerminal();
    std::cout << "Выберите тип массива:" << '\n';
    std::cout << "1. Статический массив" << '\n';
    std::cout << "2. Динамический массив" << '\n';
    std::cout << '\n';
}

void Swap(int& x, int& y) {
    int temp = x;
    x = y;
    y = temp;
}

[[nodiscard]] SortingAlgorithms::Array ReadArrayTypeFromStdin() {
    std::cout << "Ваш выбор: ";
    int type = 0;
    std::cin >> type;
    ClearTerminal();
    return static_cast<SortingAlgorithms::Array>(type);
}

[[nodiscard]] size_t ReadArrayLengthFromStdin() {
    std::cout << "Введите желаемую длину массива: ";
    int length = 0;
    std::cin >> length;
    ClearTerminal();
    return static_cast<size_t>(length);
}

void PrintArrayToStdout(const int* a, const size_t& arrayLength) {
    if (arrayLength >= kPrintArrayToStdoutLengthLimit) {
        std::cout << "Массив слишком длинный для вывода\n";
        return;
    }

    for (size_t i = 0; i < arrayLength; ++i) {
        std::cout << a[i] << ' ';
    }
    std::cout << '\n';
}

void PrintSortingResultToStdout(const SortingAlgorithms::Algorithm algorithm, const SortingAlgorithms::Array arrayType, const int* a,
                                const size_t& arrayLength, const size_t& comparisonCount, const size_t& swapCount, const bool isAscending) {
    const char stringSortingMethod[] = "Метод сортировки: ";
    const char stringSortingMethodBubble[] = "Сортировка пузырьком";
    const char stringSortingMethodSelection[] = "Сортировка выбором";
    const char stringSortingOrder[] = "Порядок сортировки: ";
    const char stringSortingOrderAscending[] = "по возрастанию";
    const char stringSortingOrderDescending[] = "по убыванию";
    const char stringSortedArray[] = "Отсортированный массив: ";
    const char stringComparisonCount[] = "Число сравнений: ";
    const char stringSwapCount[] = "Число перестановок: ";

    std::cout << stringSortingMethod;
    switch (algorithm) {
        case SortingAlgorithms::Algorithm::Bubble:
            std::cout << stringSortingMethodBubble << '\t';
            break;
        case SortingAlgorithms::Algorithm::Selection:
            std::cout << stringSortingMethodSelection << '\t';
            break;
    }

    if (arrayType == SortingAlgorithms::Array::Static) {
        std::cout << stringSortingOrder;
        if (isAscending) {
            std::cout << stringSortingOrderAscending << '\n';
        } else {
            std::cout << stringSortingOrderDescending << '\n';
        }

        std::cout << stringSortedArray;
        PrintArrayToStdout(a, arrayLength);
    }

    std::cout << stringComparisonCount << comparisonCount << '\t';
    std::cout << stringSwapCount << swapCount << '\n' << '\n';
}

void GenerateArray(int* a, const size_t arrayLength) {
    std::random_device r{};
    std::default_random_engine randomEngine(r());
    std::uniform_int_distribution distribution(1, kRandomDistributionUpperLimit);

    for (size_t i = 0; i < arrayLength; ++i) {
        a[i] = distribution(randomEngine);
    }
}

void ResetCounters(size_t& comparisonCount, size_t& swapCount) {
    comparisonCount = 0;
    swapCount = 0;
}

[[nodiscard]] bool ContinueExecution() {
    std::cout << "Желаете продолжить выполнение программы? (y/n): ";
    char continueExecution = 'n';
    std::cin >> continueExecution;
    return (continueExecution == 'y' || continueExecution == 'Y');
}
}  // namespace

namespace SortingAlgorithms {
void BubbleSort(int* a, const size_t& length, size_t& comparisonCount, size_t& swapCount, const bool isAscending) {
    bool swapOccurred = true;

    while (swapOccurred) {
        swapOccurred = false;
        for (size_t i = 0; i < length - 1; ++i) {
            if ((isAscending && a[i] > a[i + 1]) || (!isAscending && a[i] < a[i + 1])) {
                Swap(a[i], a[i + 1]);
                swapOccurred = true;

                ++swapCount;
            }

            ++comparisonCount;
        }
    }
}

void SelectionSort(int* a, const size_t& length, size_t& comparisonCount, size_t& swapCount, const bool isAscending) {
    for (size_t i = 0; i < length - 1; ++i) {
        size_t minEl = i;

        for (size_t j = i; j < length; ++j) {
            if ((isAscending && a[j] < a[minEl]) || (!isAscending && a[j] > a[minEl])) {
                minEl = j;
            }

            ++comparisonCount;
        }

        if (i != minEl) {
            Swap(a[i], a[minEl]);
            ++swapCount;
        }
    }
}

void ExecuteStaticArray() {
    const size_t arrayLength = 7;

    int array[arrayLength];
    GenerateArray(array, arrayLength);

    int arrayCopy[arrayLength];
    std::copy(array, array + arrayLength, arrayCopy);

    std::cout << "Сгенерированный массив: ";
    PrintArrayToStdout(array, arrayLength);
    std::cout << '\n';

    size_t comparisonCount = 0;
    size_t swapCount = 0;

    BubbleSort(array, arrayLength, comparisonCount, swapCount, true);
    PrintSortingResultToStdout(Algorithm::Bubble, Array::Static, array, arrayLength, comparisonCount, swapCount, true);

    ResetCounters(swapCount, comparisonCount);
    BubbleSort(array, arrayLength, comparisonCount, swapCount, true);
    PrintSortingResultToStdout(Algorithm::Bubble, Array::Static, array, arrayLength, comparisonCount, swapCount, true);

    ResetCounters(swapCount, comparisonCount);
    BubbleSort(array, arrayLength, comparisonCount, swapCount, false);
    PrintSortingResultToStdout(Algorithm::Bubble, Array::Static, array, arrayLength, comparisonCount, swapCount, false);

    ResetCounters(swapCount, comparisonCount);
    SelectionSort(arrayCopy, arrayLength, comparisonCount, swapCount, true);
    PrintSortingResultToStdout(Algorithm::Selection, Array::Static, arrayCopy, arrayLength, comparisonCount, swapCount, true);

    ResetCounters(swapCount, comparisonCount);
    SelectionSort(arrayCopy, arrayLength, comparisonCount, swapCount, true);
    PrintSortingResultToStdout(Algorithm::Selection, Array::Static, arrayCopy, arrayLength, comparisonCount, swapCount, true);

    ResetCounters(swapCount, comparisonCount);
    SelectionSort(arrayCopy, arrayLength, comparisonCount, swapCount, false);
    PrintSortingResultToStdout(Algorithm::Selection, Array::Static, arrayCopy, arrayLength, comparisonCount, swapCount, false);
}

void ExecuteDynamicArray() {
    size_t vectorLength = ReadArrayLengthFromStdin();
    int* vector = new int[vectorLength];
    GenerateArray(vector, vectorLength);

    int* vectorCopy = new int[vectorLength];
    std::copy(vector, vector + vectorLength, vectorCopy);

    std::cout << "Сгенерированный массив: ";
    PrintArrayToStdout(vector, vectorLength);
    std::cout << '\n';

    size_t comparisonCount = 0;
    size_t swapCount = 0;
    BubbleSort(vector, vectorLength, comparisonCount, swapCount, true);
    PrintSortingResultToStdout(Algorithm::Bubble, Array::Dynamic, vector, vectorLength, comparisonCount, swapCount, true);

    ResetCounters(swapCount, comparisonCount);
    SelectionSort(vectorCopy, vectorLength, comparisonCount, swapCount, true);
    PrintSortingResultToStdout(Algorithm::Selection, Array::Dynamic, vector, vectorLength, comparisonCount, swapCount, true);

    delete[] vector;
    delete[] vectorCopy;
}

void ExecuteArray(SortingAlgorithms::Array arrayType) {
    switch (arrayType) {
        case SortingAlgorithms::Array::Static:
            ExecuteStaticArray();
            break;
        case SortingAlgorithms::Array::Dynamic:
            ExecuteDynamicArray();
            break;
        default:
            std::cout << "Вы не смогли выбрать метод." << '\n';
            break;
    }
}

void StartMainLoop() {
    bool continueExecution = true;
    while (continueExecution) {
        PrintArrayMenu();

        ExecuteArray(ReadArrayTypeFromStdin());

        continueExecution = ContinueExecution();
    }

    std::cout << "Программа завершена. ᓚᘏᗢ" << '\n';
}

}  // namespace SortingAlgorithms
