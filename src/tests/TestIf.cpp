#include <gtest/gtest.h>
#include <srcSlice.hpp>
#include "TestHelper.h"

TEST(SliceTest, TestWhile) {
    std::string srcmlStr = pathToSrcml("if.c", "/src/tests/samples/if.c");

    try {
        srcSlice sslice(srcmlStr, 0);
        {
            // 変数aのテスト
            assert(sslice.SetContext("if.c", "main", 1));
            auto a_slice = sslice.Find("a").second;

            // def{} のテスト
            ASSERT_EQ(a_slice.def, std::set<unsigned int>({3, 8, 17}));
            // use{} のテスト
            ASSERT_EQ(a_slice.use, std::set<unsigned int>({8, 17}));
            ASSERT_EQ(a_slice.dvars.empty(), true);
            ASSERT_EQ(a_slice.aliases.empty(), true);
            ASSERT_EQ(a_slice.cfunctions.empty(), true);

            // 変数bのテスト
            auto b_slice = sslice.Find("b").second;

            // def{} のテスト
            ASSERT_EQ(b_slice.def, std::set<unsigned int>({4, 12}));
            // use{} のテスト
            ASSERT_EQ(b_slice.use, std::set<unsigned int>({12}));
            ASSERT_EQ(b_slice.dvars.empty(), true);
            ASSERT_EQ(b_slice.aliases.empty(), true);
            ASSERT_EQ(b_slice.cfunctions.empty(), true);
        }
    } catch (SAXError e) {
        FAIL();
    }
}
