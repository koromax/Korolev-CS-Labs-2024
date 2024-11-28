#include "Encryptor.h"
#include "Vector.h"

#include <termios.h>
#include <unistd.h>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>

namespace {
const int kLinesPerPage = 5;
const int kAsciiCodesAmount = 128;
const int kLastNonPrintingCharacterAsciiCode = 32;
const int kBackspaceCharAsciiCode = 127;
const int kBufferLength = 1024;
const int kCharColumnWidth = 5;
const int kCodeColumnWidth = 7;
const int kOccurrenceColumnWidth = 11;
const int kVariationsColumnWidth = 12;
const char kDefaultEncryptedFileName[14] = "encrypted.txt";
const char kDefaultDecryptedFileName[14] = "decrypted.txt";

struct fileName {
    const char* originalText = nullptr;
    const char* codeText = nullptr;
    const char* encryptedText = kDefaultEncryptedFileName;
    const char* decryptedText = kDefaultDecryptedFileName;
};

void ClearTerminal() {
    std::cout << "\x1B[2J\x1B[H";
}

int IdentifyFiles(int argc, char** argv, fileName& fileName) {
    const char namedArgOriginalText[] = "--original-text=";
    const char namedArgCodeText[] = "--code-text=";
    const char namedArgEncryptedText[] = "--encrypted-text=";
    const char namedArgDecryptedText[] = "--decrypted-text=";

    if (argc < 3) {
        std::cout << "Not enough arguments (Given " << argc - 1 << ", expected at least 2)\n";
        return 1;
    }

    int filesFound = 0;

    for (int i = 1; i < argc; ++i) {
        if (strstr(argv[i], namedArgOriginalText)) {
            fileName.originalText = std::strchr(argv[i], '=') + 1;

            ++filesFound;
        } else if (strstr(argv[i], namedArgCodeText)) {
            fileName.codeText = std::strchr(argv[i], '=') + 1;

            ++filesFound;
        } else if (strstr(argv[i], namedArgEncryptedText)) {
            fileName.encryptedText = std::strchr(argv[i], '=') + 1;

            ++filesFound;
        } else if (strstr(argv[i], namedArgDecryptedText)) {
            fileName.decryptedText = std::strchr(argv[i], '=') + 1;

            ++filesFound;
        }
    }

    if (filesFound != argc - 1) {
        // if not all files identified, assume non-named files to be in order
        for (int i = 1; i < argc; ++i) {
            char* fileNameTemp = std::strchr(argv[i], '=') + 1;
            if (fileNameTemp == fileName.originalText || fileNameTemp == fileName.codeText || fileNameTemp == fileName.encryptedText ||
                fileNameTemp == fileName.decryptedText) {
                continue;
            }

            if (fileName.originalText == nullptr) {
                fileName.originalText = argv[i];

                ++filesFound;
            } else if (fileName.codeText == nullptr) {
                fileName.codeText = argv[i];

                ++filesFound;
            } else if (fileName.encryptedText == nullptr) {
                fileName.encryptedText = argv[i];

                ++filesFound;
            } else if (fileName.decryptedText == nullptr) {
                fileName.decryptedText = argv[i];

                ++filesFound;
            }
        }
    }

    // if (originalTextFileName == nullptr || codeTextFileName == nullptr) {
    //     std::cout << "The program failed to identify original-text and code-text files" << '\n';
    //     std::cout << "Both original-text file and code-text file must be passed as arguments." << '\n';
    //     return 2;
    // }

    if (filesFound != argc - 1) {
        std::cout << "something funny has happened.\n";
        return 3;
    }

    // std::cout << "All files identified such as:\n";
    // std::cout << "Original text: " << fileName.originalText << '\n';
    // std::cout << "Code text: " << fileName.codeText << '\n';
    // std::cout << "Encrypted text: " << fileName.encryptedText << '\n';
    // std::cout << "Decrypted text: " << fileName.decryptedText << '\n';
    return 0;
}

void CountCodeSum(Vector::VecInt& codeKeys, char buffer[], size_t bufferLength, int& wordCodeSum) {
    for (size_t i = 0; i < bufferLength; ++i) {
        if (ispunct(buffer[i]) || isspace(buffer[i]) || buffer[i] == '\0') {
            if (wordCodeSum != 0) {
                Vector::PushBack(codeKeys, wordCodeSum);
            }
            wordCodeSum = 0;
            continue;
        }

        wordCodeSum += static_cast<int>(buffer[i]);
    }
}

[[maybe_unused]] void PrintVector(const Vector::VecInt& vector) {
    for (size_t i = 0; i < vector.size; ++i) {
        std::cout << Vector::GetElement(vector, i) << ' ';
    }

    std::cout << std::endl;
}

void GetSortedSymbolOrder(Vector::VecInt* v, int* sortedOrder) {
    bool used[kAsciiCodesAmount] = {};
    for (int i = 0; i < kAsciiCodesAmount; ++i) {
        int maxEl = 0;
        while (used[maxEl]) {
            ++maxEl;
        }

        for (int j = 0; j < kAsciiCodesAmount; ++j) {
            if (!used[j] && v[j].size > v[maxEl].size) {
                maxEl = j;
            }
        }

        sortedOrder[i] = maxEl;
        used[maxEl] = true;
    }
}

[[nodiscard]] size_t CountPages(const size_t uniqueSymbolCount) {
    return uniqueSymbolCount / kLinesPerPage + (uniqueSymbolCount % kLinesPerPage != 0);
}

void PrintSymbol(const int symbolCode) {
    const char* codeToChar[kLastNonPrintingCharacterAsciiCode] = {"NUL", "SOH", "STX", "ETX", "EOT", "ENQ", "ACK", "BEL", "BS",  "TAB", "LF",
                                                                  "VT",  "FF",  "CR",  "SO",  "SI",  "DLE", "DC1", "DC2", "DC3", "DC4", "NAK",
                                                                  "SYN", "ETB", "CAN", "EM",  "SUB", "ESC", "FS",  "GS",  "RS",  "US"};
    const char* backspaceChar = "BS";
    if (symbolCode == kBackspaceCharAsciiCode) {
        std::cout << backspaceChar;
        return;
    } else if (symbolCode < kLastNonPrintingCharacterAsciiCode) {
        std::cout << codeToChar[symbolCode];
        return;
    }

    std::cout << static_cast<char>(symbolCode);
}

void PrintStatPage(size_t pageNumber, const Vector::VecInt* symbolAliases, const int* sortedOrder, const size_t originalTextSymbolCount,
                   const size_t codeBookWordCount, const size_t uniqueSymbolCount, const size_t* symbolCount) {
    ClearTerminal();

    const char textTextLengthFirstPart[] = " Text length: ";
    const char textTextLengthLastPart[] = " symbols";
    const char textCodebookLengthFirstPart[] = " Codebook length: ";
    const char textCodebookLengthLastPart[] = " words";

    const char textShowFirstPage[] = "(3): First page ";
    const char textShowPrevPage[] = "(4): Previous page ";
    const char textSearchSymbol[] = "(5): Search symbol ";
    const char textSearchCode[] = "(%): Search by code ";
    const char textShowNextPage[] = "(6): Next page ";
    const char textShowLastPage[] = "(7): Last page ";
    const char textExitStats[] = "(0): Exit ";

    const char textShowFirstPageBlank[] = "(x): ----- ---- ";
    const char textShowPrevPageBlank[] = "(x): -------- ---- ";
    const char textShowNextPageBlank[] = "(x): ---- ---- ";
    const char textShowLastPageBlank[] = "(x): ---- ---- ";

    std::cout << " ----------------------------------------------------------------- ";
    std::cout << textTextLengthFirstPart << originalTextSymbolCount << textTextLengthLastPart << '\n';
    std::cout << "| Symbol | ASCII code | In-text occurrence | Amount of variations |";
    std::cout << textCodebookLengthFirstPart << codeBookWordCount << textCodebookLengthLastPart << '\n';

    for (size_t i = pageNumber * kLinesPerPage; i < (pageNumber + 1) * kLinesPerPage; ++i) {
        if (i == uniqueSymbolCount) {
            break;
        }

        std::cout << "| ------ + ---------- + ------------------ + -------------------- |";
        std::cout << '\n';

        std::cout << "|   " << std::setw(kCharColumnWidth) << std::left;
        PrintSymbol(sortedOrder[i]);
        std::cout << "|     " << std::setw(kCodeColumnWidth) << std::left << sortedOrder[i] << "|         " << std::setw(kOccurrenceColumnWidth)
                  << std::left << symbolCount[sortedOrder[i]] << "|          " << std::setw(kVariationsColumnWidth) << std::left
                  << symbolAliases[sortedOrder[i]].size << '|';

        if (i % kLinesPerPage == 0) {
            std::cout << " Page : " << pageNumber + 1 << " / " << CountPages(uniqueSymbolCount);
        }
        std::cout << '\n';
    }
    std::cout << " ----------------------------------------------------------------- " << '\n';

    if (pageNumber == 0) {
        std::cout << textShowFirstPageBlank << ' ';
        std::cout << textShowPrevPageBlank << ' ';
    } else {
        std::cout << textShowFirstPage << ' ';
        std::cout << textShowPrevPage << ' ';
    }
    std::cout << textSearchSymbol << ' ';
    if (pageNumber == CountPages(uniqueSymbolCount) - 1) {
        std::cout << textShowNextPageBlank << ' ';
        std::cout << textShowLastPageBlank << ' ';
    } else {
        std::cout << textShowNextPage << ' ';
        std::cout << textShowLastPage << ' ';
    }
    std::cout << textExitStats << '\n';
    std::cout << "                                     " << textSearchCode << '\n';
}

void PrintStatString(const int symbolCode, const Vector::VecInt* symbolAliases, const size_t* symbolCount) {
    if (symbolCode >= kAsciiCodesAmount || symbolCode < 0) {
        std::cout << "The symbol you entered is outside the operational scope of the program." << '\n';
        return;
    }

    const char textShowSelectedPage[] = "(5): Search symbol ";
    const char textSearchCode[] = "(%): Search by code ";
    const char textExitStats[] = "(0): Exit ";

    const char textShowFirstPageReturn[] = "(3): Return to first page ";
    const char textShowPrevPageBlank[] = "(x): -------- ---- ";
    const char textShowNextPageBlank[] = "(x): ---- ---- ";
    const char textShowLastPageBlank[] = "(x): ---- ---- ";

    ClearTerminal();

    std::cout << " ----------------------------------------------------------------- ";
    std::cout << '\n';
    std::cout << "| Symbol | ASCII code | In-text occurrence | Amount of variations |";
    std::cout << '\n';

    std::cout << "| ------ + ---------- + ------------------ + -------------------- |";
    std::cout << '\n';

    std::cout << "|   " << std::setw(kCharColumnWidth) << std::left;
    PrintSymbol(symbolCode);
    std::cout << "|     " << std::setw(kCodeColumnWidth) << std::left << symbolCode << "|         " << std::setw(kOccurrenceColumnWidth) << std::left
              << symbolCount[symbolCode] << "|          " << std::setw(kVariationsColumnWidth) << std::left << symbolAliases[symbolCode].size << '|';

    std::cout << '\n';

    std::cout << " ----------------------------------------------------------------- " << '\n';

    std::cout << textShowFirstPageReturn << ' ';
    std::cout << textShowPrevPageBlank << ' ';
    std::cout << textShowSelectedPage << ' ';
    std::cout << textShowNextPageBlank << ' ';
    std::cout << textShowLastPageBlank << ' ';
    std::cout << textExitStats << '\n';
    std::cout << "                                               " << textSearchCode << '\n';
}

void ShowStatistics(const Vector::VecInt* symbolAliases, const int* sortedOrder, const size_t originalTextSymbolCount,
                    const size_t codeBookWordCount, const size_t uniqueSymbolCount, const size_t* symbolCount) {
    struct termios old_tio = {};
    tcgetattr(STDIN_FILENO, &old_tio);
    struct termios new_tio = old_tio;
    new_tio.c_lflag &= (~ICANON);
    new_tio.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);

    size_t currentPage = 0;
    bool symbolMode = false;
    PrintStatPage(currentPage, symbolAliases, sortedOrder, originalTextSymbolCount, codeBookWordCount, uniqueSymbolCount, symbolCount);

    char keystroke = ' ';
    while (fread(&keystroke, 1, 1, stdin)) {
        int code = 0;
        switch (keystroke) {
            case '3':
                if (currentPage != 0 || symbolMode) {
                    currentPage = 0;
                    PrintStatPage(currentPage, symbolAliases, sortedOrder, originalTextSymbolCount, codeBookWordCount, uniqueSymbolCount, symbolCount);
                    symbolMode = false;
                }
                break;
            case '4':
                if (currentPage != 0 && !symbolMode) {
                    PrintStatPage(--currentPage, symbolAliases, sortedOrder, originalTextSymbolCount, codeBookWordCount, uniqueSymbolCount, symbolCount);
                }
                break;
            case '5':
                std::cout << "Waiting for symbol to search... ";
                fread(&keystroke, 1, 1, stdin);
                PrintStatString(static_cast<int>(keystroke), symbolAliases, symbolCount);
                symbolMode = true;
                break;
            case '%':
                tcsetattr(STDIN_FILENO, TCSANOW, &old_tio);
                std::cout << "Enter code to search: ";
                std::cin >> code;
                PrintStatString(code, symbolAliases, symbolCount);
                symbolMode = true;
                tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);
                break;
            case '6':
                if (currentPage != CountPages(uniqueSymbolCount) - 1 && !symbolMode) {
                    PrintStatPage(++currentPage, symbolAliases, sortedOrder, originalTextSymbolCount, codeBookWordCount, uniqueSymbolCount, symbolCount);
                }
                break;
            case '7':
                if (currentPage != CountPages(uniqueSymbolCount) - 1 && !symbolMode) {
                    currentPage = CountPages(uniqueSymbolCount) - 1;
                    PrintStatPage(currentPage, symbolAliases, sortedOrder, originalTextSymbolCount, codeBookWordCount, uniqueSymbolCount, symbolCount);
                }
                break;
            case '0':
                std::cout << "Exited." << '\n';
                tcsetattr(STDIN_FILENO, TCSANOW, &old_tio);
                return;
            default:
                break;
        }
    }
}
}  // namespace

namespace Encryptor {
[[nodiscard]] char Encryptor(char originalChar, int key) {
    int finalCharCode = static_cast<int>(originalChar);
    finalCharCode += key;
    finalCharCode %= kAsciiCodesAmount;

    return static_cast<char>(finalCharCode);
}

[[nodiscard]] char Decryptor(char finalChar, int key) {
    int originalCharCode = static_cast<int>(finalChar);
    originalCharCode -= key;
    originalCharCode = (kAsciiCodesAmount + (originalCharCode % kAsciiCodesAmount)) % kAsciiCodesAmount;

    return static_cast<char>(originalCharCode);
}

void CollectStatistics(fileName fileName, Vector::VecInt* symbolAliases, size_t& uniqueSymbolCount) {
    std::ifstream encryptedText;
    encryptedText.open(fileName.encryptedText, std::ios::binary);
    std::ifstream decryptedText;
    decryptedText.open(fileName.decryptedText, std::ios::binary);

    if (!encryptedText.is_open()) {
        std::cout << fileName.encryptedText << " failed to open." << '\n';
        return;
    }
    if (!decryptedText.is_open()) {
        std::cout << fileName.decryptedText << " failed to open." << '\n';
        return;
    }

    for (size_t i = 0; i < kAsciiCodesAmount; ++i) {
        symbolAliases[i] = Vector::CreateVector();
    }

    char encryptedBuffer[kBufferLength] = {};
    char decryptedBuffer[kBufferLength] = {};

    encryptedText.seekg(0, std::ios::beg);
    decryptedText.seekg(0, std::ios::beg);

    while (!encryptedText.eof()) {
        encryptedText.read(encryptedBuffer, kBufferLength);
        decryptedText.read(decryptedBuffer, kBufferLength);
        size_t amountOfSymbolsRead = decryptedText.gcount();
        for (size_t i = 0; i < amountOfSymbolsRead; ++i) {
            int encryptedSymbolCode = static_cast<int>(encryptedBuffer[i]);
            int decryptedSymbolCode = static_cast<int>(decryptedBuffer[i]);

            if (std::find(symbolAliases[decryptedSymbolCode].vector,
                          symbolAliases[decryptedSymbolCode].vector + symbolAliases[decryptedSymbolCode].size,
                          encryptedSymbolCode) == symbolAliases[decryptedSymbolCode].vector + symbolAliases[decryptedSymbolCode].size) {
                if (symbolAliases[decryptedSymbolCode].size == 0) {
                    ++uniqueSymbolCount;
                }
                Vector::PushBack(symbolAliases[decryptedSymbolCode], encryptedSymbolCode);
            }
        }
    }
}

void StartMainProgram(int argc, char** argv) {
    fileName fileName;

    if (IdentifyFiles(argc, argv, fileName)) {
        return;
    }

    std::ifstream codeText;
    codeText.open(fileName.codeText, std::ios::binary);

    if (!codeText.is_open()) {
        std::cout << fileName.codeText << " failed to open." << '\n';
        return;
    }

    Vector::VecInt codeKeys = Vector::CreateVector();

    char buffer[kBufferLength] = {};
    int staringCodeSum = 0;
    while (!codeText.eof()) {
        codeText.read(buffer, kBufferLength);
        size_t amountOfSymbolsRead = codeText.gcount();
        CountCodeSum(codeKeys, buffer, amountOfSymbolsRead, staringCodeSum);
    }
    if (staringCodeSum != 0) {
        Vector::PushBack(codeKeys, staringCodeSum);
    }

    size_t codeBookWordCount = codeKeys.size;

    codeText.close();

    std::ifstream originalText;
    originalText.open(fileName.originalText, std::ios::binary);
    std::fstream encryptedText;
    encryptedText.open(fileName.encryptedText, std::ios::trunc | std::ios::binary | std::ios::in | std::ios::out);

    if (!originalText.is_open()) {
        std::cout << fileName.originalText << " failed to open." << '\n';
        return;
    }
    if (!encryptedText.is_open()) {
        std::cout << fileName.encryptedText << " failed to open." << '\n';
        return;
    }

    size_t originalTextSymbolCount = 0;
    size_t symbolCount[kAsciiCodesAmount] = {};
    while (!originalText.eof()) {
        originalText.read(buffer, kBufferLength);
        size_t amountOfSymbolsRead = originalText.gcount();
        for (size_t i = 0; i < amountOfSymbolsRead; ++i) {
            ++symbolCount[static_cast<int>(buffer[i])];
            int code = Vector::GetElement(codeKeys, i % codeKeys.size);
            char newChar = Encryptor(buffer[i], code);
            const char* writeWhat = &newChar;
            encryptedText.write(writeWhat, 1);
        }
        originalTextSymbolCount += amountOfSymbolsRead;
    }
    std::cout << '\n';

    originalText.close();

    std::ofstream decryptedText;
    decryptedText.open(fileName.decryptedText, std::ios::trunc | std::ios::binary | std::ios::out);

    if (!decryptedText.is_open()) {
        std::cout << fileName.decryptedText << " failed to open." << '\n';
        return;
    }

    encryptedText.seekg(0, std::ios::beg);
    while (!encryptedText.eof()) {
        encryptedText.read(buffer, kBufferLength);
        size_t amountOfSymbolsRead = encryptedText.gcount();
        for (size_t i = 0; i < amountOfSymbolsRead; ++i) {
            int code = Vector::GetElement(codeKeys, i % codeKeys.size);
            char newChar = Decryptor(buffer[i], code);
            const char* writeWhat = &newChar;
            decryptedText.write(writeWhat, 1);
        }
    }

    encryptedText.close();
    decryptedText.close();

    Vector::VecInt symbolStatsRaw[kAsciiCodesAmount];
    size_t uniqueSymbolCount = 0;
    CollectStatistics(fileName, symbolStatsRaw, uniqueSymbolCount);

    int sortedOrder[kAsciiCodesAmount] = {};
    GetSortedSymbolOrder(symbolStatsRaw, sortedOrder);

    ShowStatistics(symbolStatsRaw, sortedOrder, originalTextSymbolCount, codeBookWordCount, uniqueSymbolCount, symbolCount);
}
}  // namespace Encryptor
