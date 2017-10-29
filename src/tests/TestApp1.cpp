#include <gtest/gtest.h>
#include <srcSlice.hpp>
#include "TestHelper.h"

TEST(SliceTest, TestApp1) {
    std::string srcmlStr = pathToSrcml("app1.c", "/src/tests/samples/app1.c");

    try {
        srcSlice sslice(srcmlStr, 0);
        {
            // 変数aのテスト
            assert(sslice.SetContext("app1.c", "main", 3));
            auto a_slice = sslice.Find("a").second;

            ASSERT_EQ(a_slice.def, std::set<unsigned int>({6, 7}));
        }

    } catch (SAXError e) {
        FAIL();
    }
}
