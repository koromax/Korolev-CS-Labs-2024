#include "Dictionary.h"

#include <termios.h>
#include <unistd.h>
#include <cstring>
#include <fstream>
#include <iostream>

/*

        COMMIT COMMENTS:
        - rewrote to work in non-canonical mode
        - ititialising dict with .capacity=8 instead of =0 ()
        - everything else i forgot


        TO-DO LIST:
        - add loading from file
        - complete the required tasks
        - praise the satan
        - add a lockout for being silly
        - remove CompareWords()?
        - do a in-terminal windows and pop-ups???
        - debloat


*/

namespace {
enum class Action : char {
    AddWord = '1',
    RemoveWord = '2',
    TranslateEngToRus = '3',
    TranslateRusToEng = '4',
    PrintDict = '5',
    DictToFile = '6',
    Exit = '7'
};

void ClearStdin() {
    while (std::getchar() != '\n')
        ;
}

void ConsoleController(bool isCanon, bool isEchoOn) {
    struct termios currentState = {};
    tcgetattr(STDIN_FILENO, &currentState);
    struct termios newState = currentState;
    if (isCanon) {
        newState.c_lflag |= (ICANON);
    } else {
        newState.c_lflag &= (~ICANON);
    }
    if (isEchoOn) {
        newState.c_lflag |= (ECHO);
    } else {
        newState.c_lflag &= (~ECHO);
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &newState);
}

/*

    Non-canonical essentials

    struct termios old_tio = {};
    tcgetattr(STDIN_FILENO, &old_tio);
    struct termios new_tio = old_tio;
    new_tio.c_lflag &= (~ICANON);
    new_tio.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);

    char keystroke = ' ';
    while (fread(&keystroke, 1, 1, stdin))

    tcsetattr(STDIN_FILENO, TCSANOW, &old_tio);

*/

void ClearTerminal() {
    std::cout << "\x1B[2J\x1B[H";
}

void itoa(int n, int numberLength, char*& a) {
    for (int i = numberLength - 1; i >= 0; --i) {
        a[i] = static_cast<char>(static_cast<int>(n / std::pow(10, i)) % 10 + 48);
    }
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
    ClearTerminal();
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
    char keystroke = ' ';
    fread(&keystroke, 1, 1, stdin);
    size_t strikes = 0;
    while (keystroke < 49 || keystroke > 55) {
        ++strikes;
        if (strikes == 1) {
            std::cout << "Press a button with a number in range 1 - 7\n";
        } else if (strikes == 4) {
            std::cout << "Okay, you are being silly now. I also can do silly stuff. Try pressing another button\n";
        } else if (strikes == 5) {
            // write a locked out trigger into file
            ClearTerminal();
            return Action::Exit;
        }
        fread(&keystroke, 1, 1, stdin);
    }
    return static_cast<Action>(keystroke);
}

int CompareWords(const char* wordA, const char* wordB) {
    return std::strcmp(wordA, wordB);
}

char* ReadWordFromStdin() {
    ConsoleController(true, true);
    char buffer[256];
    std::cout << "Enter word: ";
    std::cin >> buffer;

    char* word = new char[std::strlen(buffer) + 1];
    std::strcpy(word, buffer);

    ClearStdin();
    ConsoleController(false, false);
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

const char* GetTranslation(Dictionary& dict, const char* text, const bool& ENGtoRU) {
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
    std::cout << "eng -- rus : " << dict.length << " entries\n";
    for (size_t i = 0; i < dict.length; ++i) {
        std::cout << dict.dict[dict.wordOrder[i]].eng << ' ' << dict.dict[dict.wordOrder[i]].rus << '\n';
        // std::cout << dict.dict[i].rus << ' ' << dict.dict[i].eng << '\n';
    }
}

void WriteDictToFile(Dictionary& dict, const char* fileName = "dict.txt") {
    std::fstream dictOut;
    dictOut.open(fileName, std::ios::trunc | std::ios::out);
    if (!dictOut.is_open()) {
        std::cout << "Failed to open file " << fileName << "\n";
        return;
    }

    int numberLength = static_cast<int>(std::ceil(std::log10(dict.length)));
    char* writableLength = new char[numberLength];
    itoa(dict.length, numberLength, writableLength);
    std::cout << writableLength << '\n';
    dictOut.write(writableLength, numberLength);
    dictOut.write("\n", 1);
    for (size_t i = 0; i < dict.length; ++i) {
        dictOut.write(dict.dict[dict.wordOrder[i]].eng, std::strlen(dict.dict[dict.wordOrder[i]].eng));
        dictOut.write("\n", 1);
        dictOut.write(dict.dict[dict.wordOrder[i]].rus, std::strlen(dict.dict[dict.wordOrder[i]].rus));
        dictOut.write("\n", 1);
    }
    dictOut.close();
}

// doesn't work. don't commit
// void LoadDictFromFile(Dictionary& dict, const char* fileName = "dict.txt") {
//     std::fstream dictIn;
//     dictIn.open(fileName, std::ios::in);
//     if (!dictIn.is_open()) {
//         std::cout << "Failed to open file " << fileName << "\n";
//         return;
//     }
//     int length = 0;
//     dictIn.
// }

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
    // switching terminal to non-canonical
    struct termios old_tio = {};
    tcgetattr(STDIN_FILENO, &old_tio);
    ConsoleController(false, false);

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
                word = {.rus = "", .eng = ""};
                word.eng = ReadWordFromStdin();
                std::cout << GetTranslation(dict, word.eng, true);
                break;
            case Action::TranslateRusToEng:
                word = {.rus = "", .eng = ""};
                word.rus = ReadWordFromStdin();
                std::cout << GetTranslation(dict, word.eng, false);
                break;
            case Action::PrintDict:
                PrintDict(dict);
                break;
            case Action::DictToFile:
                WriteDictToFile(dict);
                break;
            case Action::Exit:
                std::cout << "Exiting.\n";
                tcsetattr(STDIN_FILENO, TCSANOW, &old_tio);  // restore terminal settings
                return;
        }
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &old_tio);  // restore terminal settings
}

}  // namespace Dictionary
