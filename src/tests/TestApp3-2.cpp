#include <gtest/gtest.h>
#include <srcSlice.hpp>
#include "TestHelper.hpp"

TEST(SliceTest, TestApp3_2) {
    std::string srcmlStr = pathToSrcml("app3-2.c", "/src/tests/samples/app3-2.c");

    try {
        srcSlice sslice(srcmlStr, 0);
        {
// 変数aのテスト
            assert(sslice.SetContext("app3-2.c", "main", 8));
            auto a_slice = sslice.Find("a").second;
            auto ax_slice = sslice.Find("xa");


            ASSERT_EQ(ax_slice.first, false);

//            // def{} のテスト
//            ASSERT_EQ(a_slice.def, std::set<unsigned int>({6, 7}));
//            // use{} のテスト
//            ASSERT_EQ(a_slice.use, std::set<unsigned int>({7, 9}));
        }

    } catch (SAXError e) {
        FAIL();
    }
}

