#include <gtest/gtest.h>
#include <srcSlice.hpp>
#include "../helpers/TestHelper.hpp"

TEST(SliceTest, TestArrayType) {
    std::string srcmlStr = pathToSrcml("array_type_issue14.c", "/src/tests/samples/array_type_issue14.c");

    try {
        srcSlice sslice(srcmlStr, 0);
        {
            assert(sslice.SetContext("array_type_issue14.c", "f", 1));
            auto a_slice = sslice.Find("a[]").second;
            ASSERT_EQ(a_slice->variableName, "a[]");

            auto b_slice = sslice.Find("b[]").second;
            ASSERT_EQ(b_slice->variableName, "b[]");
        }
    } catch (SAXError e) {
        FAIL();
    }
}
