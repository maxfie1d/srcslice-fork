#include <gtest/gtest.h>
#include <core/srcSlice.hpp>
#include "../helpers/TestHelper.hpp"

TEST(SliceTest, TestIssue32) {
    std::string srcmlStr = pathToSrcml("issue32.c", "/src/tests/samples/issue32.c");

    try {
        srcSlice sslice(srcmlStr, 0);
        assert(sslice.SetContext("issue32.c", "main", 9));
        {
            auto g_slice = sslice.Find("g").second;

            testDef(g_slice, std::set<unsigned int>({1, 5, 10, 12}));
        }
    } catch (SAXError e) {
        FAIL();
    }
}
