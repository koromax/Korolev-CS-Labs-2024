#include "Dictionary.h"

#include <cstring>
#include <fstream>
#include <iostream>

namespace {
enum class Action : int {
    AddWord = 1,
    RemoveWord,
    TranslateEngToRus,
    TranslateRusToEng,
    PrintDict,
    DictToFile,
    Exit
};

void ClearTerminal() {
    std::cout << "\x1B[2J\x1B[H";
}

void AllocSpace(Dictionary::Dictionary& dict) {
    size_t newDictSize = dict.capacity << 1;

    Dictionary::Word* bufferDict = new Dictionary::Word[newDictSize];
    size_t* bufferWordOrder = new size_t[newDictSize];

    std::copy(dict.dict, dict.dict + dict.length, bufferDict);
    std::copy(dict.wordOrder, dict.wordOrder + dict.length, bufferWordOrder);

    Dictionary::Word* oldDict = dict.dict;
    size_t* oldWordOrder = dict.wordOrder;

    dict.dict = bufferDict;
    dict.wordOrder = bufferWordOrder;
    dict.capacity = newDictSize;

    delete[] oldDict;
    delete[] oldWordOrder;
}

[[maybe_unused]] void PrintMenu() {
    std::cout << "Dictionary" << '\n';
    std::cout << "Choose an action" << '\n' << '\n';
    std::cout << "1. Add word to Dictionary" << '\n';
    std::cout << "2. Remove word from the Dictionary" << '\n';
    std::cout << "3. Translate word from English to Russian" << '\n';
    std::cout << "4. Translate word from Russian to English" << '\n';
    std::cout << "5. Print Dictionary" << '\n';
    std::cout << "6. Save Dictionary to file" << '\n';
    std::cout << "7. Exit" << '\n';
}

[[maybe_unused]] Action ReadSelectionFromStdin() {
    int sel = 0;
    std::cin >> sel;
    return static_cast<Action>(sel);
}

int CompareWords(const char* wordA, const char* wordB) {
    return std::strcmp(wordA, wordB);
}

char* ReadWordFromStdin() {
    char buffer[256];
    std::cout << "Enter word: ";
    std::cin >> buffer;

    char* word = new char[std::strlen(buffer) + 1];
    std::strcpy(word, buffer);

    return word;
}
}  // namespace

namespace Dictionary {
void AddWord(Dictionary& dict, const Word& word) {
    if (dict.length == dict.capacity) {
        AllocSpace(dict);
    }

    dict.dict[dict.length] = word;

    size_t i = 0;
    while (i < dict.length && CompareWords(word.eng, dict.dict[dict.wordOrder[i]].eng) > 0) {
        ++i;
    }

    for (size_t j = dict.length; j > i; --j) {
        dict.wordOrder[j] = dict.wordOrder[j - 1];
    }

    dict.wordOrder[i] = dict.length;
    ++dict.length;
}

void RemoveWord(Dictionary& dict, const Word& word) {
    size_t i = 0;
    while (i < dict.length && CompareWords(word.eng, dict.dict[i].eng) != 0) {
        ++i;
    }

    if (i == dict.length) {
        std::cout << "Word not found\n";
        return;
    }

    delete[] dict.dict[i].eng;
    delete[] dict.dict[i].rus;

    for (size_t j = i; j < dict.length - 1; ++j) {
        dict.dict[j] = dict.dict[j + 1];
    }

    --dict.length;

    size_t k = 0;
    while (k < dict.length && CompareWords(word.eng, dict.dict[dict.wordOrder[k]].eng) != 0) {
        ++k;
    }

    for (size_t j = k; j < dict.length; ++j) {
        dict.wordOrder[j] = dict.wordOrder[j + 1];
    }
}

const char* GetTranslation(Dictionary& dict, const char* text, bool& ENGtoRU) {
    size_t L = 0;
    size_t R = dict.length;

    if (ENGtoRU) {
        size_t m = L + (R - L) / 2;
        while (R - L > 1) {
            if (CompareWords(text, dict.dict[m].eng) >= 0) {
                L = m;
            } else {
                R = m;
            }
            m = L + (R - L) / 2;
        }

        if (CompareWords(text, dict.dict[L].eng) != 0) {
            std::cout << "no such word to be found: " << text << '\n';
            return "";
        }

        return dict.dict[L].rus;

    } else {
        for (size_t i = 0; i < dict.length; ++i) {
            if (CompareWords(text, dict.dict[i].rus) == 0) {
                return dict.dict[i].eng;
            }
        }
        std::cout << "no such word to be found: " << text << '\n';
        return "";
    }
}

void PrintDict(Dictionary& dict) {
    for (size_t i = 0; i < dict.length; ++i) {
        std::cout << dict.dict[i].rus << ' ' << dict.dict[i].eng << '\n';
    }
}

void WriteDictToFile(Dictionary& dict, const char* fileName = "dict.txt") {
    std::fstream dictOut;
    dictOut.open(fileName, std::ios::trunc | std::ios::out);
    if (!dictOut.is_open()) {
        std::cout << "Failed to open file " << fileName << "\n";
        return;
    }

    size_t numberLength = static_cast<size_t>(std::ceil(std::log10(dict.length)));
    char writableLength[numberLength];
    std::sprintf(writableLength, "%d", dict.length);
    dictOut.write(writableLength, numberLength);
    for (size_t i = 0; i < dict.length; ++i) {
        dictOut.write(dict.dict[i].eng, std::strlen(dict.dict[i].eng));
        dictOut.write(" --- ", 5);
        dictOut.write(dict.dict[i].rus, std::strlen(dict.dict[i].rus));
        dictOut.write("\n", 1);
    }
    dictOut.close();
}

void LoadDictFromFile(Dictionary& dict, const char* fileName = "dict.txt") {
    std::fstream dictIn;
    dictIn.open(fileName, std::ios::in);
    if (!dictIn.is_open()) {
        std::cout << "Failed to open file " << fileName << "\n";
        return;
    }
    int length = 0;
    dictIn.
}

void test() {
    Dictionary dict;

    Word word1 = {.rus = "", .eng = ""};
    word1.eng = ReadWordFromStdin();
    word1.rus = ReadWordFromStdin();
    AddWord(dict, word1);

    // AddWord(dict, {.rus = "dead inside", .eng = "me at 2AM"});
    // AddWord(dict, {.rus = "sobaka", .eng = "dog"});
    // AddWord(dict, {.rus = "koshka", .eng = "cat"});
    // AddWord(dict, {.rus = "penguin", .eng = "penguin"});
    // AddWord(dict, {.rus = "ryba", .eng = "fish"});
    // AddWord(dict, {.rus = "crack'o'deal", .eng = "croc"});
    // RemoveWord(dict, {.rus = "dead inside", .eng = "me at 2AM"});
    PrintDict(dict);

    // std::cout << GetTranslation(dict, "penguin", false);
    // std::cout << GetTranslation(dict, "penguin", true);

    // WriteDictToFile(dict);
}

void Interactive() {
    Dictionary dict;
    Word word;

    while (true) {
        PrintMenu();
        switch (ReadSelectionFromStdin()) {
            case Action::AddWord:
                word = {.rus = "", .eng = ""};
                word.eng = ReadWordFromStdin();
                word.rus = ReadWordFromStdin();
                AddWord(dict, word);
                break;
            case Action::RemoveWord:

                break;
            case Action::TranslateEngToRus:
                break;
            case Action::TranslateRusToEng:
                break;
            case Action::PrintDict:
                PrintDict(dict);
                break;
            case Action::DictToFile:
                WriteDictToFile(dict);
                break;
            case Action::Exit:
                std::cout << "Exiting.\n";
                return;
        }
    }
}

}  // namespace Dictionary
