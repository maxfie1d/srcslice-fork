#include <gtest/gtest.h>
#include <srcSlice.hpp>
#include "TestHelper.hpp"

TEST(SliceTest, TestIssue16) {
    std::string srcmlStr = pathToSrcml("issue16.c", "/src/tests/samples/issue16.c");

    try {
        srcSlice sslice(srcmlStr, 0);
        {
            assert(sslice.SetContext("issue16.c", "main", 1));
            auto a_slice = sslice.Find("a").second;

            testDef(a_slice, std::set<unsigned int>({3, 4}));
            testUse(a_slice, std::set<unsigned int>({}));
        }
    } catch (SAXError e) {
        FAIL();
    }
}
