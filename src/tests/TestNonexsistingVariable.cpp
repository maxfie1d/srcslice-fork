#include <gtest/gtest.h>
#include <srcSlice.hpp>
#include "TestHelper.hpp"

TEST(SliceTest, TestNonexistingVariable) {
    std::string srcmlStr = pathToSrcml("nonexisting_variable", "/src/tests/samples/nonexisting_variable.c");

    try {
        srcSlice sslice(srcmlStr, 0);
        {
            assert(sslice.SetContext("nonexisting_variable", "main", 8));
            // xaという存在しない変数が含まれないことをテストする
            auto ax_slice = sslice.Find("xa");
            ASSERT_EQ(ax_slice.first, false);
        }
    } catch (SAXError e) {
        FAIL();
    }
}
