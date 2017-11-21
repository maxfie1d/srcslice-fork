#include <gtest/gtest.h>
#include <srcSlice.hpp>
#include "TestHelper.hpp"

TEST(SliceTest, TestIssue2) {
    std::string srcmlStr = pathToSrcml("issue2.c", "/src/tests/samples/issue2.c");

    try {
        srcSlice sslice(srcmlStr, 0);
        assert(sslice.SetContext("issue2.c", "main", 1));
        {
            // 変数xのテスト
            auto x_slice = sslice.Find("x").second;

            testDef(&x_slice, std::set<unsigned int>({2, 3}));
            testUse(&x_slice, std::set<unsigned int>({3}));
            testDvars(&x_slice, std::set<std::string>({"x"}));
        }
    } catch (SAXError e) {
        FAIL();
    }
}
