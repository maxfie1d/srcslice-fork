#include <gtest/gtest.h>
#include <srcSlice.hpp>
#include "TestHelper.hpp"

TEST(SliceTest, TestWhile) {
    std::string srcmlStr = pathToSrcml("while.c", "/src/tests/samples/while.c");

    try {
        srcSlice sslice(srcmlStr, 0);
        assert(sslice.SetContext("while.c", "main", 1));
        {
            // 変数 sum のテスト
            auto sum = sslice.Find("sum").second;

            testDef(&sum, std::set<unsigned int>({3, 8}));
            testUse(&sum, std::set<unsigned int>({8, 12, 14}));
            testDvars(&sum, std::set<std::string>({"sum", "average"}));
            ASSERT_EQ(sum.aliases.empty(), true);
            std::set<CfuncShortData> expected_cfuncs(
                    {
                            CfuncShortData("printf", 2)
                    }
            );
            testCfuncs(&sum, expected_cfuncs);
        }
        {
            // 変数 n のテスト
            auto n_slice = sslice.Find("n").second;

            testDef(&n_slice, std::set<unsigned int>({4, 9}));
            testUse(&n_slice, std::set<unsigned int>({9, 13, 14}));
            testDvars(&n_slice, std::set<std::string>({"n", "average"}));
            ASSERT_EQ(n_slice.aliases.empty(), true);
            std::set<CfuncShortData> expected_cfuncs(
                    {
                            CfuncShortData("printf", 2)
                    }
            );
            testCfuncs(&n_slice, expected_cfuncs);
        }
        {
            // 変数 data のテスト
            auto data_slice = sslice.Find("data").second;

            testDef(&data_slice, std::set<unsigned int>({5, 10}));
            testUse(&data_slice, std::set<unsigned int>({6, 8}));
            testDvars(&data_slice, std::set<std::string>({"sum"}));
            ASSERT_EQ(data_slice.aliases.empty(), true);
            assertCfuncsEmpty(&data_slice);
        }
        {
            // 変数 average のテスト
            auto average_slice = sslice.Find("average").second;

            testDef(&average_slice, std::set<unsigned int>({14}));
            testUse(&average_slice, std::set<unsigned int>({15}));
            assertDvarsEmpty(&average_slice);
            ASSERT_EQ(average_slice.aliases.empty(), true);
            std::set<CfuncShortData> expected_cfuncs(
                    {
                            CfuncShortData("printf", 2)

                    }
            );
            testCfuncs(&average_slice, expected_cfuncs);
        }
    } catch (SAXError e) {
        FAIL();
    }
}
