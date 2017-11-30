#include <gtest/gtest.h>
#include <core/srcSlice.hpp>
#include "../helpers/TestHelper.hpp"

TEST(SliceTest, TestConstTypes) {
    std::string srcmlStr = pathToSrcml("const_types_issue13.c", "/src/tests/samples/const_types_issue13.c");

    try {
        srcSlice sslice(srcmlStr, 0);
        {
            assert(sslice.SetContext("const_types_issue13.c", "f", 1));
            auto a_slice = sslice.Find("a").second;
            ASSERT_EQ(a_slice->variableType, "const int");

            auto b_slice = sslice.Find("b").second;
            ASSERT_EQ(b_slice->variableType, "const int");
        }
    } catch (SAXError e) {
        FAIL();
    }
}
