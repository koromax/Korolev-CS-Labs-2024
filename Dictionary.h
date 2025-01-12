#pragma once

#include <stddef.h>

namespace Dictionary {
struct Word {
    char* rus;
    char* eng;
};

struct Dictionary {
    size_t length = 0;
    size_t capacity = 8;
    size_t* wordOrder = new size_t[capacity];
    Word* dict = new Word[capacity];
};

void AddWord(Dictionary& dict, const Word& word);
void RemoveWord(Dictionary& dict, const Word& word);
char* GetTranslation(Dictionary& dict, const char* text, const bool& ENGtoRU);
void PrintDict(Dictionary& dict);
void WriteDictToFile(Dictionary& dict, const char* fileName);

void test();
void Interactive();
}  // namespace Dictionary
