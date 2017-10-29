#include "TestHelper.h"
#include <fstream>
#include <iostream>

std::string readFileAsStr(const char *filename) {
    std::ifstream stream(filename);
    if (stream.fail()) {
        std::cerr << "ファイルの読み込みに失敗しました" << std::endl;
        return NULL;
    } else {
        std::string str((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
        return str;
    }
}
