#pragma  once

#include <string>
#include <iostream>
#include <core/SliceProfile.hpp>
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

std::string StringToSrcML(const char *file_name, const std::string str);

/**
 * ファイル名からsrcML形式に文字列まで一気に処理する
 * @param fileName
 * @param path
 * @return
 */
std::string pathToSrcml(const char *fileName, std::string path);

void testDef(SliceProfile *sp, std::set<unsigned int> expectedDefLines);

struct TestDefUseData {
    unsigned int line_number;
    std::string member_name;

    TestDefUseData(unsigned int line_number) {
        this->line_number = line_number;
    }

    TestDefUseData(unsigned int line_number, std::string member_name) {
        this->line_number = line_number;
        this->member_name = member_name;
    }

    bool operator<(const TestDefUseData &other) const {
        if (this->line_number == other.line_number) {
            return this->member_name < other.member_name;
        } else {
            return this->line_number < other.line_number;
        }
    }

    bool operator==(const TestDefUseData &other) const {
        return this->line_number == other.line_number
               && this->member_name == other.member_name;
    }
};

void testDefDetail(SliceProfile *sp, std::set<TestDefUseData> expected);

void testUse(SliceProfile *sp, std::set<unsigned int> expectedUseLines);

void testUseDetail(SliceProfile *sp, std::set<TestDefUseData> expected);

void testDvars(SliceProfile *sp, std::set<std::string> expectedDvars);

void testCfuncs(SliceProfile *sp, std::set<CfuncShortData> expectedCfuncs);

void assertDefEmpty(SliceProfile *sp);

void assertUseEmpty(SliceProfile *sp);

void assertDvarsEmpty(SliceProfile *sp);

void assertCfuncsEmpty(SliceProfile *sp);
