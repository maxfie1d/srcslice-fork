#include <gtest/gtest.h>
#include <srcSlice.hpp>
#include "../helpers/TestHelper.hpp"

TEST(SliceTest, TestApp1) {
    std::string srcmlStr = pathToSrcml("app1.c", "/src/tests/samples/app1.c");

    try {
        srcSlice sslice(srcmlStr, 0);
        {
            // 変数aのテスト
            assert(sslice.SetContext("app1.c", "main", 3));
            auto a_slice = sslice.Find("a").second;

            testDef(a_slice, std::set<unsigned int>({6, 7}));
            testUse(a_slice, std::set<unsigned int>({9}));
            assertDvarsEmpty(a_slice);

            std::set<CfuncShortData> expected_cfuncs(
                    {
                            CfuncShortData("printf", 2)
                    });
            testCfuncs(a_slice, expected_cfuncs);
        }
    } catch (SAXError e) {
        FAIL();
    }
}
