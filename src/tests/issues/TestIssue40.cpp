#include <gtest/gtest.h>
#include <core/srcSlice.hpp>
#include "../helpers/TestHelper.hpp"

TEST(SliceTest, TestIssue40) {
    std::string srcmlStr = pathToSrcml("issue40.c", "/src/tests/samples/issue40.c");

    try {
        srcSlice sslice(srcmlStr, 0);
        {
            assert(sslice.SetContext("issue40.c", "main", 6));
            auto o_slice = sslice.Find("o").second;

            testUseDetail(o_slice, std::set<TestDefUseData>(
                    {
                            TestDefUseData(10, "x"),
                            TestDefUseData(11, "x"),
                    }));
        }
    } catch (SAXError e) {
        FAIL();
    }
}
