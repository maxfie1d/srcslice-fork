#pragma  once

#include <string>
#include <iostream>
#include <SliceProfile.hpp>

std::string readFileAsStr(const char *filename);

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
