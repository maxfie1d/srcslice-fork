#include <gtest/gtest.h>
#include <srcSlice.hpp>
#include "../helpers/TestHelper.hpp"

TEST(SliceTest, TestIf) {
    std::string srcmlStr = pathToSrcml("if.c", "/src/tests/samples/if.c");

    try {
        srcSlice sslice(srcmlStr, 0);
        {
            // 変数aのテスト
            assert(sslice.SetContext("if.c", "main", 1));
            auto a_slice = sslice.Find("a").second;

            testDef(a_slice, std::set<unsigned int>({3, 8, 17}));
            assertUseEmpty(a_slice);
            assertDvarsEmpty(a_slice);
            ASSERT_EQ(a_slice->aliases.empty(), true);
            assertCfuncsEmpty(a_slice);

            // 変数bのテスト
            auto b_slice = sslice.Find("b").second;

            testDef(b_slice, std::set<unsigned int>({4, 12}));
            assertUseEmpty(b_slice);
            assertDvarsEmpty(b_slice);
            ASSERT_EQ(b_slice->aliases.empty(), true);
            assertCfuncsEmpty(b_slice);
        }
    } catch (SAXError e) {
        FAIL();
    }
}
