#include <gtest/gtest.h>
#include <core/srcSlice.hpp>
#include "../helpers/TestHelper.hpp"

TEST(SliceTest, TestIssue31) {
    std::string srcmlStr = pathToSrcml("issue31.c", "/src/tests/samples/issue31_struct_members.c");

    try {
        srcSlice sslice(srcmlStr, 0);
        assert(sslice.SetContext("issue31.c", "main", 9));
        {
            auto point_slice = sslice.Find("point").second;
        }
    } catch (SAXError e) {
        FAIL();
    }
}

