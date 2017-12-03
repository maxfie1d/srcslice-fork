#include "../helpers/TestHelper.hpp"
#include <fstream>
#include <srcml.h>
#include <helpers/functional.hpp>
#include <helpers/srcMLHelper.h>
#include <helpers/FileHelper.h>


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

std::string pathToSrcml(const char *fileName, std::string path) {
    std::string resolvedPath = resolvePath(path);
    std::string srcStr = readFileAsStr(resolvedPath.c_str());
    std::string srcmlStr = StringToSrcML(fileName, srcStr);

    // ヘッダファイルが上位に来るように再構成する
    return reconstructSrcMLStringForSrcSlice(srcmlStr);
}

void testDef(SliceProfile *sp, std::set<unsigned int> expectedDefLines) {
    auto defLines = set_transform<DefUseData, unsigned int>(sp->def, [](DefUseData dd) {
        return dd.programPoint.lineNumber;
    });

    ASSERT_EQ(defLines, expectedDefLines);
}

void testDefDetail(SliceProfile *sp, std::set<TestDefUseData> expected) {
    auto a = set_transform<DefUseData, TestDefUseData>(sp->def, [](DefUseData dd) {
        return TestDefUseData(dd.programPoint.lineNumber, dd.member_name);
    });

    ASSERT_EQ(a, expected);
}

void testUse(SliceProfile *sp, std::set<unsigned int> expectedUseLines) {
    auto useLines = set_transform<DefUseData, unsigned int>
            (sp->use,
             [](DefUseData dd) {
                 return dd.programPoint.lineNumber;
             });

    ASSERT_EQ(useLines, expectedUseLines);
}

void testUseDetail(SliceProfile *sp, std::set<TestDefUseData> expected) {
    auto a = set_transform<DefUseData, TestDefUseData>(sp->use, [](DefUseData dd) {
        return TestDefUseData(dd.programPoint.lineNumber, dd.member_name);
    });
    ASSERT_EQ(a, expected);
}

void assertDefEmpty(SliceProfile *sp) {
    ASSERT_EQ(sp->def.empty(), true);
}

void assertUseEmpty(SliceProfile *sp) {
    ASSERT_EQ(sp->use.empty(), true);
}

void assertDvarsEmpty(SliceProfile *sp) {
    ASSERT_EQ(sp->dvars.empty(), true);
}

void assertCfuncsEmpty(SliceProfile *sp) {
    ASSERT_EQ(sp->cfunctions.empty(), true);
}

void testDvars(SliceProfile *sp, std::set<std::string> expectedDvars) {
    auto actualDvars = set_transform<DvarData, std::string>(sp->dvars, [](DvarData dd) {
        return dd.variableName;
    });

    ASSERT_EQ(actualDvars, expectedDvars);
}

void testCfuncs(SliceProfile *sp, std::set<CfuncShortData> expectedCfuncs) {
    std::set<CfuncShortData> actualCfuncs = set_transform<CFuncData, CfuncShortData>
            (sp->cfunctions, [](CFuncData cd) {
                return CfuncShortData(cd.calledFunctionName, cd.argIndenx);
            });

    ASSERT_EQ(actualCfuncs, expectedCfuncs);
}

