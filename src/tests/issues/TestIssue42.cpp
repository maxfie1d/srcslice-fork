#include <gtest/gtest.h>
#include <core/srcSlice.hpp>
#include "../helpers/TestHelper.hpp"

TEST(SliceTest, TestIssue42) {
    std::string srcmlStr = pathToSrcml("issue42.c", "/src/tests/samples/issue42.c");

    try {
        srcSlice sslice(srcmlStr, 0);
        {
            assert(sslice.SetContext("issue42.c", "main", 1));
            sslice.dictionary.controlTable.dump();
        }
    } catch (SAXError e) {
        FAIL();
    }
}
