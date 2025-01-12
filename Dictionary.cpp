#include "Dictionary.h"

#include <termios.h>
#include <unistd.h>
#include <chrono>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>

/*

        COMMIT COMMENTS:
        - removed redundant stuff
        - removed CompareWords()
        - added loading from file
        - added a lockout for being silly


        TO-DO LIST:
        - complete the required tasks (get removing word and getting translation working)
        - praise the satan
        - do a in-terminal windows and pop-ups???
        - debloat (nah)

*/

namespace {
enum class Action : char {
    AddWord = '1',
    RemoveWord = '2',
    TranslateEngToRus = '3',
    TranslateRusToEng = '4',
    PrintDict = '5',
    DictToFile = '6',
    Exit = '7',
    None
};

void ClearTerminal() {
    std::cout << "\x1B[2J\x1B[H";
}

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

void PrintMenu() {
    ClearTerminal();
    std::cout << std::left;
    std::cout << std::setw(80) << "Dictionary" << '\n';
    std::cout << std::setw(80) << "Choose an action" << '\n' << '\n';
    std::cout << std::setw(80) << "1. Add word to Dictionary" << '\n';
    std::cout << std::setw(80) << "2. Remove word from the Dictionary" << '\n';
    std::cout << std::setw(80) << "3. Translate word from English to Russian" << '\n';
    std::cout << std::setw(80) << "4. Translate word from Russian to English" << '\n';
    std::cout << std::setw(80) << "5. Print Dictionary" << '\n';
    std::cout << std::setw(80) << "6. Save Dictionary to file" << '\n';
    std::cout << std::setw(80) << "7. Exit" << '\n';
}

void ShowPopUp(const Action& action) {
    if (action != Action::DictToFile) {
        std::cout << "Bruh\n";
        return;
    }
    char keystroke = ' ';

    std::cout << "\033[s";  // saving cursor position
    std::cout << "\033[7A";
    std::cout << "\033[20C";
    std::cout << "╔══════════════════════════════════════╗";
    std::cout << "\033[1B";
    std::cout << "\033[40D";
    std::cout << "║                                      ║";
    std::cout << "\033[1B";
    std::cout << "\033[40D";
    std::cout << "║     Saved dictionary to a file.      ║";
    std::cout << "\033[1B";
    std::cout << "\033[40D";
    std::cout << "║                                      ║";
    std::cout << "\033[1B";
    std::cout << "\033[40D";
    std::cout << "║          (Press any button)          ║";
    std::cout << "\033[1B";
    std::cout << "\033[40D";
    std::cout << "╚══════════════════════════════════════╝";
    std::cout << "\033[u";  // restoring cursor position
    fread(&keystroke, 1, 1, stdin);
}

// void ShowPopUp(Action& action, Dictionary::Word& word) {
//     switch (action) {
//         case Action::AddWord:
//             std::cout << "Adding word to dictionary\n";
//             break;
//         case Action::RemoveWord:
//             std::cout << "Removing word from dictionary\n";
//             break;
//         case Action::TranslateEngToRus:
//             std::cout << "Translating " << word.eng << " to Russian\n";
//             break;
//         case Action::TranslateRusToEng:
//             std::cout << "Translating " << word.rus << " to English\n";
//             break;
//         default:
//             std::cout << "Defaulted in ShowPopUp. Investigate\n";
//             break;
//     }
// }

[[nodiscard]] Action ReadSelectionFromStdin() {
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
            return Action::None;
        }
        fread(&keystroke, 1, 1, stdin);
    }
    return static_cast<Action>(keystroke);
}

[[nodiscard]] char* ReadWordFromStdin() {
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

void WriteLockoutIntoFile(const char* fileName = "dict.txt") {
    std::fstream dictOut(fileName, std::ios::trunc | std::ios::out | std::ios::binary);
    if (!dictOut.is_open()) {
        std::cout << "Failed to open file " << fileName << "\n";
        return;
    }
    dictOut.write("-1\n", 3);
    auto currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    dictOut.write(reinterpret_cast<const char*>(&currentTime), sizeof(currentTime));
    dictOut.close();

    std::cout << "Program was preemptively closed at " << std::ctime(&currentTime) << '\n';
}

int LoadDictFromFile(Dictionary::Dictionary& dict, const char* fileName = "dict.txt") {
    std::ifstream dictIn(fileName);
    if (!dictIn.is_open()) {
        std::cout << "Failed to open file " << fileName << "\n";
        return 1;
    }

    char buffer[256];
    if (dictIn.getline(buffer, sizeof(buffer))) {
        int length = std::atoi(buffer);

        // check if lockout is in place
        if (length == -1) {
            auto currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            std::time_t recordedTime;
            dictIn.read(reinterpret_cast<char*>(&recordedTime), sizeof(recordedTime));

            if (currentTime - recordedTime < 60) {
                return 2;
            }
        }

        for (int i = 0; i < length; ++i) {
            Dictionary::Word word;
            if (dictIn.getline(buffer, sizeof(buffer))) {
                word.eng = new char[std::strlen(buffer) + 1];
                std::strcpy(word.eng, buffer);
            }
            if (dictIn.getline(buffer, sizeof(buffer))) {
                word.rus = new char[std::strlen(buffer) + 1];
                std::strcpy(word.rus, buffer);
            }
            AddWord(dict, word);
        }
    }

    dictIn.close();
    return 0;
}

}  // namespace

namespace Dictionary {
void AddWord(Dictionary& dict, const Word& word) {
    if (dict.length == dict.capacity) {
        AllocSpace(dict);
    }

    dict.dict[dict.length] = word;

    size_t i = 0;
    while (i < dict.length && std::strcmp(word.eng, dict.dict[dict.wordOrder[i]].eng) > 0) {
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
    while (i < dict.length && std::strcmp(word.eng, dict.dict[i].eng) != 0) {
        ++i;
    }

    if (i == dict.length) {
        std::cout << "Word not found\n";
        return;
    }

    delete[] dict.dict[i].eng;
    delete[] dict.dict[i].rus;

    --dict.length;
    for (size_t j = i; j < dict.length; ++j) {
        dict.dict[j] = dict.dict[j + 1];
    }

    size_t k = 0;
    while (k < dict.length && std::strcmp(word.eng, dict.dict[dict.wordOrder[k]].eng) != 0) {
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
            if (std::strcmp(text, dict.dict[m].eng) >= 0) {
                L = m;
            } else {
                R = m;
            }
            m = L + (R - L) / 2;
        }

        if (std::strcmp(text, dict.dict[L].eng) != 0) {
            std::cout << "no such word to be found: " << text << '\n';
            return "";
        }

        return dict.dict[L].rus;

    } else {
        for (size_t i = 0; i < dict.length; ++i) {
            if (std::strcmp(text, dict.dict[i].rus) == 0) {
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
    if (dict.length == 0) {
        std::cout << "Dictionary is empty! Aborted writing to file\n";
        return;
    }

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

    ShowPopUp(Action::DictToFile);
}

void test() {
    Dictionary dict;

    Word word1;
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

    switch (LoadDictFromFile(dict)) {
        case 1:
            std::cout << "Failed to load dictionary from file\n";
            break;
        case 2:
            std::cout << "You have been locked out for being silly\n";
            return;
        default:
            break;
    }

    // switching terminal to non-canonical
    struct termios old_tio = {};
    tcgetattr(STDIN_FILENO, &old_tio);
    ConsoleController(false, false);
    char keystroke = ' ';

    while (true) {
        PrintMenu();
        switch (ReadSelectionFromStdin()) {
            case Action::AddWord:
                word.eng = ReadWordFromStdin();
                word.rus = ReadWordFromStdin();
                AddWord(dict, word);
                break;
            case Action::RemoveWord:
                break;
            case Action::TranslateEngToRus:
                word.eng = ReadWordFromStdin();
                std::cout << GetTranslation(dict, word.eng, true);
                break;
            case Action::TranslateRusToEng:
                word.rus = ReadWordFromStdin();
                std::cout << GetTranslation(dict, word.eng, false);
                break;
            case Action::PrintDict:
                PrintDict(dict);
                fread(&keystroke, 1, 1, stdin);
                break;
            case Action::DictToFile:
                WriteDictToFile(dict);
                break;
            case Action::Exit:
                std::cout << "Exiting.\n";
                tcsetattr(STDIN_FILENO, TCSANOW, &old_tio);  // restore terminal settings
                return;
            default:
                ClearTerminal();
                WriteLockoutIntoFile();
                tcsetattr(STDIN_FILENO, TCSANOW, &old_tio);  // restore terminal settings
                return;
        }
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &old_tio);  // restore terminal settings
}

}  // namespace Dictionary
