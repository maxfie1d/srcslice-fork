#include <gtest/gtest.h>
#include <srcSlice.hpp>
#include "TestHelper.hpp"

TEST(SliceTest, TestApp1) {
    std::string srcmlStr = pathToSrcml("app1.c", "/src/tests/samples/app1.c");

    try {
        srcSlice sslice(srcmlStr, 0);
        {
            // 変数aのテスト
            assert(sslice.SetContext("app1.c", "main", 3));
            auto a_slice = sslice.Find("a").second;

            // def{} のテスト
            ASSERT_EQ(a_slice.def, std::set<unsigned int>({6, 7}));
            // use{} のテスト
            ASSERT_EQ(a_slice.use, std::set<unsigned int>({7, 9}));

            ASSERT_EQ(a_slice.dvars.empty(), true);
            ASSERT_EQ(a_slice.aliases.empty(), true);

            // cfuncs{} のテスト
            CFuncSet expected_cfuncs;
            expected_cfuncs.insert(std::make_pair("printf", 2));
            ASSERT_EQ(a_slice.cfunctions, expected_cfuncs);
        }

    } catch (SAXError e) {
        FAIL();
    }
}
