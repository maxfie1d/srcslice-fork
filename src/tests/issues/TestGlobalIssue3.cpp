#include <gtest/gtest.h>
#include <core/srcSlice.hpp>
#include "../helpers/TestHelper.hpp"


TEST(SliceTest, TestGlobalIssue3) {
    std::string srcmlStr = pathToSrcml("global.c", "/src/tests/samples/global_issue3.c");

    try {
        srcSlice sslice(srcmlStr, 0);
        // srcSlice::setContext の意味は
        // 「このファイル」の「この関数」の (どれどれの変数) についてチェックするという意味らしい
        assert(sslice.SetContext("global.c", "f", 2));
        {
            // グローバル変数gのテスト
            auto g_slice = sslice.Find("g").second;
            // グローバルスコープかチェック
            ASSERT_EQ(g_slice->function, "__GLOBAL__");
            testDef(g_slice, std::set<unsigned int>({1, 5}));
            testUse(g_slice, std::set<unsigned int>({4, 13}));
            testDvars(g_slice, std::set<std::string>({"b", "c"}));
            ASSERT_EQ(g_slice->aliases.empty(), true);
            assertCfuncsEmpty(g_slice);
        }

        assert(sslice.SetContext("global.c", "main", 8));
        {
            // 変数 b @ main()のテスト
            auto b_slice = sslice.Find("b").second;
            testDef(b_slice, std::set<unsigned int>({11, 13}));
            assertUseEmpty(b_slice);
            assertDvarsEmpty(b_slice);
            ASSERT_EQ(b_slice->aliases.empty(), true);
            assertCfuncsEmpty(b_slice);
        }
        {
            // 変数 a @ main()のテスト
            auto a_slice = sslice.Find("a").second;
            testDef(a_slice, std::set<unsigned int>({10, 12}));
            testUse(a_slice, std::set<unsigned int>({2, 4, 12}));
            testDvars(a_slice, std::set<std::string>({"a", "c"}));
            ASSERT_EQ(a_slice->aliases.empty(), true);

            std::set<CfuncShortData> expected_cfuncs(
                    {
                            CfuncShortData("f", 1)
                    });
            testCfuncs(a_slice, expected_cfuncs);
        }

        assert(sslice.SetContext("global.c", "f", 2));
        {
            // 変数 c @ f()のテスト
            auto c_slice = sslice.Find("c").second;
            testDef(c_slice, std::set<unsigned int>({4}));
            testUse(c_slice, std::set<unsigned int>({5, 6}));
            testDvars(c_slice, std::set<std::string>({"g"}));
            ASSERT_EQ(c_slice->aliases.empty(), true);
            assertCfuncsEmpty(c_slice);
        }
        {
            // 変数 x @ f()のテスト
            auto c_slice = sslice.Find("x").second;
            testDef(c_slice, std::set<unsigned int>({2}));
            testUse(c_slice, std::set<unsigned int>({4}));
            testDvars(c_slice, std::set<std::string>({"c"}));
            ASSERT_EQ(c_slice->aliases.empty(), true);
            assertCfuncsEmpty(c_slice);
        }
        {
            // Issue#26でグローバル変数のIDが一致しないという問題があったため
            // テストする
            auto c_slice = sslice.dictionary.variableTable.findSliceProfile("global.c", "f", "c");
            std::string id1 = c_slice->dvars.begin()->variableId;
            auto g_slice = sslice.dictionary.variableTable.findGlobalVariableSliceProfileByName("g");
            std::string id2 = g_slice->computeVariableId();
            ASSERT_EQ(id1, id2);
        }
    } catch (SAXError e) {
        FAIL();
    }
}
