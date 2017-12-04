#include <gtest/gtest.h>
#include <core/srcSlice.hpp>
#include "../helpers/TestHelper.hpp"

TEST(SliceTest, TestIssue36) {
    std::string srcmlStr = pathToSrcml("issue36.c", "/src/tests/samples/issue36.c");

    try {
        srcSlice sslice(srcmlStr, 0);
        assert(sslice.SetContext("issue36.c", "main", 9));
        {
            auto g_point_slice = sslice.Find("g_point").second;

            testUseDetail(g_point_slice, std::set<TestDefUseData>(
                    {
                            TestDefUseData(13, "x"),
                            TestDefUseData(13, "y"),
                            TestDefUseData(22, "x"),
                            TestDefUseData(22, "y")
                    }
            ));
        }
    } catch (SAXError e) {
        FAIL();
    }
}
