#pragma once

#include <stddef.h>

namespace {
enum class lang : bool {
    eng = true,
    rus = false
};
}  // namespace

namespace Dictionary {
struct Word {
    const char* rus;
    const char* eng;
};

struct Dictionary {
    size_t length = 0;
    size_t capacity = 8;
    size_t* wordOrder = new size_t[8];
    Word* dict = new Word[8];
};

void test();
void Interactive();
}  // namespace Dictionary
