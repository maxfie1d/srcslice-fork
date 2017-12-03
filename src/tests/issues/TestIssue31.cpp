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

            testDef(point_slice, std::set<unsigned int>({11, 13, 14}));

            for (auto a : point_slice->def) {
                std::cout << a.to_string() << std::endl;
            }
        }
    } catch (SAXError e) {
        FAIL();
    }
}
