#pragma  once

#include <string>
#include <iostream>
#include <SliceProfile.hpp>

std::string readFileAsStr(const char *filename);

std::string StringToSrcML(const char *file_name, const std::string str);

/**
 * スライス結果の比較をコンソールに出力します
 * @tparam T
 * @param lhsSet
 * @param rhsSet
 */
template<typename T>
void OutputCompare(const T &lhsSet, const T &rhsSet) {
    std::cerr << "{";
    for (auto i : lhsSet) {
        std::cerr << i << ",";
    }
    std::cerr << "} == {";
    for (auto i : rhsSet) {
        std::cerr << i << ",";
    }
    std::cerr << "}" << std::endl;
}

typedef std::unordered_set<std::pair<std::string, unsigned int>, NameLineNumberPairHash> CFuncSet;

void OutputCompare(const CFuncSet &lhsSet, const CFuncSet &rhsSet);

/**
 * 相対パスを解決します
 * @param path
 * @return
 */
std::string resolvePath(std::string path);
