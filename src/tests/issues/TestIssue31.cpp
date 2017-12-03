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

            testDefDetail(point_slice, std::set<TestDefUseData>(
                    {
                            TestDefUseData(11, ""),
                            TestDefUseData(13, "x"),
                            TestDefUseData(14, "y")
                    }));

            testUseDetail(point_slice, std::set<TestDefUseData>(
                    {
                            TestDefUseData(16, "x"),
                            TestDefUseData(16, "y")
                    }
            ));
        }
    } catch (SAXError e) {
        FAIL();
    }
}
