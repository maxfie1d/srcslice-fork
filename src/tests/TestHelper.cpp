#include "TestHelper.h"
#include <fstream>
#include <srcml.h>

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

/// <summary>
/// srcml の出力結果の末尾に不明な文字が含まれる
/// ことがあるのでそれを除く関数
/// Utility function that trims from the right of a string. For now it's just solving a weird issue with srcML
/// and garbage text ending up at the end of the cstring it returns.
/// </summary>
inline char *TrimFromEnd(char *s, size_t len) {
    for (int i = len - 1; i > 0; --i) {
        if (s[i] != '>') {
            s[i] = 0;
        } else {
            return s;
        }
    }
    return nullptr;
}

std::string StringToSrcML(const char *file_name, const std::string str) {
    struct srcml_archive *archive;
    struct srcml_unit *unit;
    size_t size = 0;
    char *ch = new char[str.size()];
    archive = srcml_archive_create();
    srcml_archive_enable_option(archive, SRCML_OPTION_POSITION);
    srcml_archive_write_open_memory(archive, &ch, &size);
    unit = srcml_unit_create(archive);
    srcml_unit_set_language(unit, SRCML_LANGUAGE_CXX);
    srcml_unit_set_filename(unit, file_name);
    srcml_unit_parse_memory(unit, str.c_str(), str.size());
    // srcml_archive_write_unit(archive, unit);
    srcml_write_unit(archive, unit);
    srcml_unit_free(unit);
    srcml_archive_close(archive);
    srcml_archive_free(archive);
    TrimFromEnd(ch, size);
    return std::string(ch);
}

typedef std::unordered_set<std::pair<std::string, unsigned int>, NameLineNumberPairHash> CFuncSet;

void OutputCompare(const CFuncSet &lhsSet, const CFuncSet &rhsSet) {
    // NOTE: 2017.10.29 出力が邪魔なのでスキップしている
    return;

    std::cerr << "cfuncs: {";
    for (auto i : lhsSet) {
        std::cerr << "{" << i.first << "," << i.second << "},";
    }
    std::cerr << "} == {";
    for (auto i : rhsSet) {
        std::cerr << "{" << i.first << "," << i.second << "},";
    }
    std::cerr << "}" << std::endl;
}

std::string resolvePath(std::string path) {
    // CLion からだと、なぜか std::getenv が機能しない
    const char *srcslice_root = std::getenv("SRCSLICE_ROOT");
    const char *base = srcslice_root != NULL ? srcslice_root : "/home/n-isida/github/srcslice-fork";
    if (base) {
        std::string resolved = std::string(base) + path;
        return resolved;
    } else {
        throw "環境変数 SRCSLICE_ROOT が設定されていません";
    }
}

std::string pathToSrcml(const char *fileName, std::string path) {
    std::string resolvedPath = resolvePath(path);
    std::string srcStr = readFileAsStr(resolvedPath.c_str());
    std::string srcmlStr = StringToSrcML(fileName, srcStr);
    return srcmlStr;
}
