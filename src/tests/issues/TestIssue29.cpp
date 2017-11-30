#include <gtest/gtest.h>
#include <core/srcSlice.hpp>
#include "../helpers/TestHelper.hpp"

TEST(SliceTest, TestIssue29) {
    std::string srcmlStr = pathToSrcml("global_def_use_chain.c", "/src/tests/samples/global_def_use_chain.c");

    try {
        srcSlice sslice(srcmlStr, 0);
        {
            assert(sslice.SetContext("global_def_use_chain.c", "main", 8));
            auto g_slice = sslice.Find("g").second;

            // 関数呼び出し時に変数の使用が伝播するなら
            // use{}に{3, 5}が現れるはず
            testUse(g_slice, std::set<unsigned int>({3, 5, 11}));
        }
    } catch (SAXError e) {
        FAIL();
    }
}
