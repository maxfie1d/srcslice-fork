#include <gtest/gtest.h>
#include <core/srcSlice.hpp>
#include "../helpers/TestHelper.hpp"

TEST(SliceTest, TestIssue31_Local) {
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

TEST(SliceTest, TestIssue31_Global) {
    std::string srcmlStr = pathToSrcml("issue31.c", "/src/tests/samples/issue31_global_struct_members.c");

    try {
        srcSlice sslice(srcmlStr, 0);
        assert(sslice.SetContext("issue31.c", "main", 17));
        {
            auto g_point_slice = sslice.Find("g_point").second;

            testDefDetail(g_point_slice, std::set<TestDefUseData>(
                    {
                            TestDefUseData(9, ""),
                            TestDefUseData(13, "x"),
                            TestDefUseData(14, "y"),
                            TestDefUseData(19, "x"),
                            TestDefUseData(20, "y"),
                            TestDefUseData(22, "x"),
                            TestDefUseData(22, "y"),
                    }
            ));

            testUseDetail(g_point_slice, std::set<TestDefUseData>(
                    {
                            TestDefUseData(24, "x"),
                            TestDefUseData(24, "y")
                    }
            ));
        }
    } catch (SAXError e) {
        FAIL();
    }
}
