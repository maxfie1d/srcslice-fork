#include <gtest/gtest.h>
#include <srcSlice.hpp>
#include "TestHelper.hpp"


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
            ASSERT_EQ(g_slice.function, "__GLOBAL__");
            ASSERT_EQ(g_slice.def, std::set<unsigned int>({1, 5}));
            ASSERT_EQ(g_slice.use, std::set<unsigned int>({4, 13}));
            ASSERT_EQ(g_slice.dvars, std::unordered_set<std::string>({"b", "c"}));
            ASSERT_EQ(g_slice.aliases.empty(), true);
            ASSERT_EQ(g_slice.cfunctions.empty(), true);
        }

        assert(sslice.SetContext("global.c", "main", 8));
        {
            // 変数 b @ main()のテスト
            auto b_slice = sslice.Find("b").second;
            ASSERT_EQ(b_slice.def, std::set<unsigned int>({11, 13}));
            ASSERT_EQ(b_slice.use.empty(), true);
            ASSERT_EQ(b_slice.dvars.empty(), true);
            ASSERT_EQ(b_slice.aliases.empty(), true);
            ASSERT_EQ(b_slice.cfunctions.empty(), true);
        }
        {
            // 変数 a @ main()のテスト
            auto a_slice = sslice.Find("a").second;
            ASSERT_EQ(a_slice.def, std::set<unsigned int>({10, 12}));
            ASSERT_EQ(a_slice.use, std::set<unsigned int>({12}));
            ASSERT_EQ(a_slice.dvars, std::unordered_set<std::string>({"a"}));
            ASSERT_EQ(a_slice.aliases.empty(), true);

            CFuncSet expected_cfuncs;
            expected_cfuncs.insert(std::make_pair("f", 1));
            ASSERT_EQ(a_slice.cfunctions, expected_cfuncs);
        }

        assert(sslice.SetContext("global.c", "f", 2));
        {
            // 変数 c @ f()のテスト
            auto c_slice = sslice.Find("c").second;
            ASSERT_EQ(c_slice.def, std::set<unsigned int>({4}));
            ASSERT_EQ(c_slice.use, std::set<unsigned int>({5, 6}));
            ASSERT_EQ(c_slice.dvars, std::unordered_set<std::string>({"g"}));
            ASSERT_EQ(c_slice.aliases.empty(), true);
            ASSERT_EQ(c_slice.cfunctions.empty(), true);
        }
        {
            // 変数 x @ f()のテスト
            auto c_slice = sslice.Find("x").second;
            ASSERT_EQ(c_slice.def, std::set<unsigned int>({2}));
            ASSERT_EQ(c_slice.use, std::set<unsigned int>({4}));
            ASSERT_EQ(c_slice.dvars, std::unordered_set<std::string>({"c"}));
            ASSERT_EQ(c_slice.aliases.empty(), true);
            ASSERT_EQ(c_slice.cfunctions.empty(), true);
        }
    } catch (SAXError e) {
        FAIL();
    }
}
