#include "helpers/FileHelper.h"

std::string resolvePath(std::string path) {
    // CLion からだと、なぜか std::getenv が機能しない
    const char *srcslice_root = std::getenv("SRCSLICE_ROOT");
    const char *base = srcslice_root != NULL ? srcslice_root : "/home/naoto/github/srcslice-fork";
    if (base) {
        std::string resolved = std::string(base) + path;
        return resolved;
    } else {
        throw "環境変数 SRCSLICE_ROOT が設定されていません";
    }
}

std::string readFileAsStr(const char *filename) {
    std::ifstream stream(filename);
    if (stream.fail()) {
        std::cerr << "ファイルの読み込みに失敗しました: " << filename << std::endl;
        return NULL;
    } else {
        std::string str((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
        return str;
    }
}
