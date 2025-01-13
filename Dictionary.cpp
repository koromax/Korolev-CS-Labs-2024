#include "Dictionary.h"

#include <termios.h>
#include <unistd.h>
#include <chrono>
#include <cmath>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>

/*

        GRADED ON 2025-01-13

        COMMIT COMMENTS:
        - added DeleteDict()

*/

namespace {
const int kDecimalSystem = 10;
const int kZeroASCIICode = 48;
const int kTerminalWidth = 80;
const int kDictSavedPopUpHeight = 6;
const int kTranslationPopUpHeight = 7;
const int kTranslationWordWidth = 34;
const int kDictWordsPerPage = 5;
const int kPrintDictPopUpHeight = 7;
const int kPrintDictEntriesCountWidth = 8;
const int kPrintDictWordWidth = 18;
const int kPrintDictWordNumberWidth = 3;
const int kPrintDictWordVerticalOffset = 3;

const int kOneASCIICode = 49;
const int kSevenASCIICode = 55;

const int kBufferSize = 256;
const int kTimeoutSeconds = 60;
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
    while (std::getchar() != '\n') {}
}

void WaitForKey() {
    char keystroke = 'c';
    while (keystroke != ' ') {
        fread(&keystroke, 1, 1, stdin);
    }  // (Press SPACE to continue)
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
        a[-i + 1] = static_cast<char>(static_cast<int>(n / std::pow(kDecimalSystem, i)) % kDecimalSystem + kZeroASCIICode);
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
    std::cout << std::setw(kTerminalWidth) << "Dictionary" << '\n';
    std::cout << std::setw(kTerminalWidth) << "Choose an action" << '\n' << '\n';
    std::cout << std::setw(kTerminalWidth) << "1. Add word to Dictionary" << '\n';
    std::cout << std::setw(kTerminalWidth) << "2. Remove word from the Dictionary" << '\n';
    std::cout << std::setw(kTerminalWidth) << "3. Translate word from English to Russian" << '\n';
    std::cout << std::setw(kTerminalWidth) << "4. Translate word from Russian to English" << '\n';
    std::cout << std::setw(kTerminalWidth) << "5. Print Dictionary" << '\n';
    std::cout << std::setw(kTerminalWidth) << "6. Save Dictionary to file" << '\n';
    std::cout << std::setw(kTerminalWidth) << "7. Exit" << '\n';
    std::cout << "\033[s";
}

void ShowRemoveWordError() {
    const char* SavedDictMessage[kDictSavedPopUpHeight] = {"╔══════════════════════════════════════╗", "║                                      ║",
                                                           "║           Word not found.            ║", "║                                      ║",
                                                           "║      (Press SPACE to continue)       ║", "╚══════════════════════════════════════╝"};

    std::cout << "\033[s";  // saving cursor position
    std::cout << "\033[3;21H";
    for (auto& i : SavedDictMessage) {
        std::cout << i;
        std::cout << "\033[1B";
        std::cout << "\033[40D";
    }

    std::cout << "\033[u";  // restoring cursor position
    WaitForKey();
}

void ShowSavedPopUp(const Action& action) {
    if (action != Action::DictToFile) {
        std::cout << "Bruh\n";
        return;
    }

    const char* SavedDictMessage[kDictSavedPopUpHeight] = {"╔══════════════════════════════════════╗", "║                                      ║",
                                                           "║     Saved dictionary to a file.      ║", "║                                      ║",
                                                           "║      (Press SPACE to continue)       ║", "╚══════════════════════════════════════╝"};

    std::cout << "\033[s";  // saving cursor position
    std::cout << "\033[3;21H";
    for (auto& i : SavedDictMessage) {
        std::cout << i;
        std::cout << "\033[1B";
        std::cout << "\033[40D";
    }

    std::cout << "\033[u";  // restoring cursor position
    WaitForKey();
}

[[nodiscard]] char* GetTranslation(Dictionary::Dictionary& dict, const char* text, const bool& ENGtoRU) {
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
            return nullptr;
        }

        return dict.dict[L].rus;

    } else {
        for (size_t i = 0; i < dict.length; ++i) {
            if (std::strcmp(text, dict.dict[i].rus) == 0) {
                return dict.dict[i].eng;
            }
        }
        return nullptr;
    }
}

void ShowDictionaryFill(Dictionary::Dictionary& dict, const size_t firstWord = 0) {
    std::cout << "\033[s";  // saving cursor position
    for (size_t i = firstWord; i < firstWord + kDictWordsPerPage; ++i) {
        if (i == dict.length) {
            break;
        }

        int line = static_cast<int>(kPrintDictWordVerticalOffset + i - firstWord);
        std::cout << "\033[" << line << ";13H";
        std::cout << std::setw(kPrintDictWordWidth) << std::right << dict.dict[dict.wordOrder[i]].eng;
        std::cout << " -- ";
        std::cout << std::setw(kPrintDictWordWidth) << std::left << dict.dict[dict.wordOrder[i]].rus;
        std::cout << "\033[" << line << ";56H";
        std::cout << std::setw(kPrintDictWordNumberWidth) << std::left << i + 1;
    }
    std::cout << "\033[u";  // restoring cursor position
}

[[nodiscard]] Action ReadSelectionFromStdin() {
    char keystroke = ' ';
    fread(&keystroke, 1, 1, stdin);
    size_t strikes = 0;
    while (keystroke < kOneASCIICode || keystroke > kSevenASCIICode) {
        ++strikes;
        if (strikes == 1) {
            std::cout << "Press a button with a number in range 1 - 7\n";
        } else if (strikes == 3) {
            std::cout << "Okay, you are being silly now. I also can do silly stuff. Try pressing another button\n";
        } else if (strikes == 4) {
            return Action::None;
        }
        fread(&keystroke, 1, 1, stdin);
    }
    return static_cast<Action>(keystroke);
}

[[nodiscard]] char* ReadWordFromStdin(const bool& IsEng) {
    ConsoleController(true, true);
    char buffer[kBufferSize];
    if (IsEng) {
        std::cout << "Enter english word: ";
    } else {
        std::cout << "Enter russian word: ";
    }
    std::cin >> buffer;

    size_t length = std::strlen(buffer);
    char* word = new char[length + 1];
    std::strncpy(word, buffer, length);
    word[length] = '\0';

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
    char timeBuffer[sizeof(currentTime)];
    std::memcpy(timeBuffer, &currentTime, sizeof(currentTime));
    dictOut.write(timeBuffer, sizeof(timeBuffer));
    dictOut.close();

    std::cout << "Program was preemptively closed at " << std::ctime(&currentTime) << '\n';
}

[[nodiscard]] int LoadDictFromFile(Dictionary::Dictionary& dict, const char* fileName = "dict.txt") {
    std::ifstream dictIn(fileName);
    if (!dictIn.is_open()) {
        std::cout << "Failed to open file " << fileName << "\n";
        return 1;
    }

    char buffer[kBufferSize];
    if (dictIn.getline(buffer, sizeof(buffer))) {
        int length = std::atoi(buffer);

        // check if lockout is in place
        if (length == -1) {
            auto currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            std::time_t recordedTime = 0;
            char timeBuffer[sizeof(recordedTime)];
            dictIn.read(timeBuffer, sizeof(timeBuffer));
            std::memcpy(&recordedTime, timeBuffer, sizeof(recordedTime));

            if (currentTime - recordedTime < kTimeoutSeconds) {
                return 2;
            }
        }

        for (int i = 0; i < length; ++i) {
            Dictionary::Word word{};
            if (dictIn.getline(buffer, sizeof(buffer))) {
                size_t length = std::strlen(buffer);
                word.eng = new char[length + 1];
                std::strncpy(word.eng, buffer, length);
                word.eng[length] = '\0';
            }
            if (dictIn.getline(buffer, sizeof(buffer))) {
                size_t length = std::strlen(buffer);
                word.rus = new char[length + 1];
                std::strncpy(word.rus, buffer, length);
                word.rus[length] = '\0';
            }
            AddWord(dict, word);
        }
    }

    dictIn.close();
    return 0;
}

[[maybe_unused]] void DeleteDict(Dictionary::Dictionary& dict) {
    for (size_t i = 0; i < dict.length; ++i) {
        delete[] dict.dict[i].eng;
        delete[] dict.dict[i].rus;
    }
    dict.length = 0;
    delete[] dict.dict;
    delete[] dict.wordOrder;
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

void RemoveWord(Dictionary& dict, const char* word) {
    size_t i = 0;
    while (i < dict.length && std::strcmp(word, dict.dict[i].eng) && std::strcmp(word, dict.dict[i].rus) != 0) {
        ++i;
    }

    if (i == dict.length) {
        ShowRemoveWordError();
        return;
    }

    Word wordToRemove = dict.dict[i];

    delete[] dict.dict[i].eng;
    delete[] dict.dict[i].rus;

    --dict.length;
    for (size_t j = i; j < dict.length; ++j) {
        dict.dict[j] = dict.dict[j + 1];
    }

    size_t k = 0;
    while (k < dict.length && std::strcmp(wordToRemove.eng, dict.dict[dict.wordOrder[k]].eng) != 0) {
        ++k;
    }

    for (size_t j = k; j < dict.length; ++j) {
        dict.wordOrder[j] = dict.wordOrder[j + 1];
    }
}

void ShowTranslation(Word& word) {
    char WordNotFound[] = "NOT FOUND";
    if (word.eng == nullptr) {
        word.eng = WordNotFound;
    } else if (word.rus == nullptr) {
        word.rus = WordNotFound;
    }

    const char* TranslationMessage[kTranslationPopUpHeight] = {"╔══════════════════════════════════════╗", "║  English:                            ║",
                                                               "║                                      ║", "║  Russian:                            ║",
                                                               "║                                      ║", "║       (Press SPACE to continue)      ║",
                                                               "╚══════════════════════════════════════╝"};

    std::cout << "\033[s";  // saving cursor position
    std::cout << "\033[3;19H";
    for (auto& i : TranslationMessage) {
        std::cout << i;
        std::cout << "\033[1B";
        std::cout << "\033[40D";
    }

    // writing word.eng
    std::cout << "\033[5;20H";
    std::cout << "  " << std::setw(kTranslationWordWidth) << std::right << word.eng;

    // writing word.rus
    std::cout << "\033[7;20H";
    std::cout << "  " << std::setw(kTranslationWordWidth) << std::right << word.rus;

    std::cout << "\033[u";  // restoring cursor position
    WaitForKey();
}

void ShowDictionary(Dictionary& dict) {
    const char* PrintDictBorder[kPrintDictPopUpHeight] = {
        "╔═══════════════════════ Dictionary ═══════════════════════╗", "║                                          ║               ║",
        "║                                          ║      Total    ║", "║                                          ║     entries:  ║",
        "║                                          ║               ║", "║                                          ║               ║",
        "╚═════════════════ (Press SPACE to close) ═════════════════╝"};

    // printing border
    std::cout << "\033[s";  // saving cursor position
    std::cout << "\033[2;11H";
    for (auto& i : PrintDictBorder) {
        std::cout << i;
        std::cout << "\033[1B";
        std::cout << "\033[60D";
    }

    std::cout << "\033[6;60H";
    std::cout << std::setw(kPrintDictEntriesCountWidth) << std::right << dict.length;
    std::cout << "\033[u";  // restoring cursor position

    if (dict.length == 0) {
        std::cout << "\033[4;16H";
        std::cout << "Dictionary is empty!";
        std::cout << "\033[1B";
        std::cout << "\033[20D";
        std::cout << "Try adding some words first :)";
        std::cout << "\033[u";  // restoring cursor position
        WaitForKey();
        return;
    }

    ShowDictionaryFill(dict);

    size_t firstWord = 0;
    char keystroke = 'c';
    char buf[3] = "  ";
    while (fread(&keystroke, 1, 1, stdin)) {
        if (keystroke == ' ') {
            break;
        }

        buf[0] = buf[1];
        buf[1] = buf[2];
        buf[2] = keystroke;

        if (buf[0] == '\033' && buf[2] == 'B' && firstWord + kDictWordsPerPage < dict.length) {
            ShowDictionaryFill(dict, ++firstWord);
        } else if (buf[0] == '\033' && buf[2] == 'A' && firstWord > 0) {
            ShowDictionaryFill(dict, --firstWord);
        }
    }

    std::cout << "\033[u";  // restoring cursor position
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
    itoa(static_cast<int>(dict.length), numberLength, writableLength);
    dictOut.write(writableLength, numberLength);
    dictOut.write("\n", 1);
    for (size_t i = 0; i < dict.length; ++i) {
        dictOut.write(dict.dict[dict.wordOrder[i]].eng, static_cast<long>(std::strlen(dict.dict[dict.wordOrder[i]].eng)));
        dictOut.write("\n", 1);
        dictOut.write(dict.dict[dict.wordOrder[i]].rus, static_cast<long>(std::strlen(dict.dict[dict.wordOrder[i]].rus)));
        dictOut.write("\n", 1);
    }
    dictOut.close();

    ShowSavedPopUp(Action::DictToFile);
}

void Interactive() {
    Dictionary dict{};
    Word word{};

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

    while (true) {
        PrintMenu();
        switch (ReadSelectionFromStdin()) {
            case Action::AddWord:
                word.eng = ReadWordFromStdin(true);
                word.rus = ReadWordFromStdin(false);
                AddWord(dict, word);
                break;
            case Action::RemoveWord:
                word.eng = ReadWordFromStdin(true);
                RemoveWord(dict, word.eng);
                break;
            case Action::TranslateEngToRus:
                word.eng = ReadWordFromStdin(true);
                word.rus = GetTranslation(dict, word.eng, true);
                ShowTranslation(word);
                break;
            case Action::TranslateRusToEng:
                word.rus = ReadWordFromStdin(false);
                word.eng = GetTranslation(dict, word.rus, false);
                ShowTranslation(word);
                break;
            case Action::PrintDict:
                ShowDictionary(dict);
                break;
            case Action::DictToFile:
                WriteDictToFile(dict);
                break;
            case Action::Exit:
                DeleteDict(dict);
                tcsetattr(STDIN_FILENO, TCSANOW, &old_tio);  // restore terminal settings
                return;
            default:
                ClearTerminal();
                WriteLockoutIntoFile();
                tcsetattr(STDIN_FILENO, TCSANOW, &old_tio);  // restore terminal settings
                return;
        }
    }
}

}  // namespace Dictionary
