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
    size_t capacity = 0;
    size_t* wordOrder = nullptr;
    Word* dict = nullptr;
};

void test();
void Interactive();
}  // namespace Dictionary
