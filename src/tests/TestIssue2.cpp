#include <gtest/gtest.h>
#include <srcSlice.hpp>
#include "TestHelper.h"

TEST(SliceTest, TestIssue2) {
    std::string srcmlStr = pathToSrcml("issue2.c", "/src/tests/samples/issue2.c");

    try {
        srcSlice sslice(srcmlStr, 0);
        assert(sslice.SetContext("issue2.c", "main", 1));
        {
            // 変数xのテスト
            auto x_slice = sslice.Find("x").second;

            // def{}のテスト
            ASSERT_EQ(x_slice.def, std::set<unsigned int>({2, 3}));
            // use{}のテスト
            ASSERT_EQ(x_slice.use, std::set<unsigned int>({3}));
            // dvars{}のテスト
            ASSERT_EQ(x_slice.dvars, std::unordered_set<std::string>({"x"}));
        }
    } catch (SAXError e) {
        FAIL();
    }
}
