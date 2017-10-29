#include "TestHelper.h"
#include <fstream>
#include <srcml.h>

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
    //TrimFromEnd(ch, size);
    return std::string(ch);
}

typedef std::unordered_set<std::pair<std::string, unsigned int>, NameLineNumberPairHash> CFuncSet;

void OutputCompare(const CFuncSet &lhsSet, const CFuncSet &rhsSet) {
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
