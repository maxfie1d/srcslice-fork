#pragma  once

#include <string>
#include <iostream>
#include <SliceProfile.hpp>
#include <gtest/gtest.h>

/**
 * テスト用に仕方なく作った構造体
 * 呼び出された関数名と引数のインデックスを保持する
 */
struct CfuncShortData {
    std::string calledFunctionName;
    unsigned short argIndex;

    CfuncShortData(std::string calledFunctionName, unsigned short argIndex) {
        this->calledFunctionName = calledFunctionName;
        this->argIndex = argIndex;
    }

    bool operator<(const CfuncShortData &other) const {
        if (this->calledFunctionName == other.calledFunctionName) {
            return this->argIndex < other.argIndex;
        } else {
            return this->calledFunctionName < other.calledFunctionName;
        }
    }

    bool operator==(const CfuncShortData &other) const {
        return this->calledFunctionName == other.calledFunctionName
               && this->argIndex == other.argIndex;
    }
};

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
    // NOTE: 2017.10.29 出力が邪魔なのでスキップしている
    return;

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

void OutputCompare(const std::set<CfuncShortData> &lhsSet, const std::set<CfuncShortData> &rhsSet);

/**
 * 相対パスを解決します
 * @param path
 * @return
 */
std::string resolvePath(std::string path);


/**
 * ファイル名からsrcML形式に文字列まで一気に処理する
 * @param fileName
 * @param path
 * @return
 */
std::string pathToSrcml(const char *fileName, std::string path);

void testDef(SliceProfile *sp, std::set<unsigned int> expectedDefLines);

void testUse(SliceProfile *sp, std::set<unsigned int> expectedUseLines);

void assertDvarsEmpty(SliceProfile *sp);

void assertCfuncsEmpty(SliceProfile *sp);

void testCfuncs(SliceProfile *sp, std::set<CfuncShortData> expectedCfuncs);
