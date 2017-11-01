#include <gtest/gtest.h>
#include <srcSlice.hpp>
#include "TestHelper.h"

TEST(SliceTest, TestWhile) {
    std::string srcmlStr = pathToSrcml("while.c", "/src/tests/samples/while.c");

    try {
        srcSlice sslice(srcmlStr, 0);
        assert(sslice.SetContext("while.c", "main", 1));
        {
            // 変数 sum のテスト
            auto sum = sslice.Find("sum").second;

            // def{} のテスト
            ASSERT_EQ(sum.def, std::set<unsigned int>({3, 8}));
            // use{} のテスト
            // TODO: use{}に8が含まれていない
            ASSERT_EQ(sum.use, std::set<unsigned int>({12, 14}));
            ASSERT_EQ(sum.dvars, std::unordered_set<std::string>({"average"}));
            ASSERT_EQ(sum.aliases.empty(), true);
            CFuncSet expected_cfuncs;
            expected_cfuncs.insert(std::make_pair("printf", 2));
            ASSERT_EQ(sum.cfunctions, expected_cfuncs);
        }
        {
            // 変数 n のテスト
            auto n = sslice.Find("n").second;

            // def{} のテスト
            ASSERT_EQ(n.def, std::set<unsigned int>({4, 9}));
            // use{} のテスト
            // TODO: use{}に9が含まれていない
            ASSERT_EQ(n.use, std::set<unsigned int>({13, 14}));
            ASSERT_EQ(n.dvars, std::unordered_set<std::string>({"average"}));
            ASSERT_EQ(n.aliases.empty(), true);
            CFuncSet expected_cfuncs;
            expected_cfuncs.insert(std::make_pair("printf", 2));
            ASSERT_EQ(n.cfunctions, expected_cfuncs);
        }
        {
            // 変数 data のテスト
            auto data = sslice.Find("data").second;

            // def{} のテスト
            ASSERT_EQ(data.def, std::set<unsigned int>({5, 10}));
            // use{} のテスト
            // TODO: use{}に9が含まれていない
            ASSERT_EQ(data.use, std::set<unsigned int>({6, 8}));
            ASSERT_EQ(data.dvars, std::unordered_set<std::string>({"sum"}));
            ASSERT_EQ(data.aliases.empty(), true);
            ASSERT_EQ(data.cfunctions.empty(), true);
        }
        {
            // 変数 average のテスト
            auto average = sslice.Find("average").second;

            // def{} のテスト
            ASSERT_EQ(average.def, std::set<unsigned int>({14}));
            // use{} のテスト
            // TODO: use{}に9が含まれていない
            ASSERT_EQ(average.use, std::set<unsigned int>({15}));
            ASSERT_EQ(average.dvars.empty(), true);
            ASSERT_EQ(average.aliases.empty(), true);
            CFuncSet expected_cfuncs;
            expected_cfuncs.insert(std::make_pair("printf", 2));
            ASSERT_EQ(average.cfunctions, expected_cfuncs);
        }
    } catch (SAXError e) {
        FAIL();
    }
}
