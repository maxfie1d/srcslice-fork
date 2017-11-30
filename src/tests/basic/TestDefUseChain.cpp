#include <gtest/gtest.h>
#include <core/srcSlice.hpp>
#include "../helpers/TestHelper.hpp"

TEST(SliceTest, TestDefUseChain) {
    std::string srcmlStr = pathToSrcml("def_use_chain.c", "/src/tests/samples/def_use_chain.c");

    try {
        srcSlice sslice(srcmlStr, 0);
        {
            assert(sslice.SetContext("def_use_chain.c", "main", 6));
            auto a_slice = sslice.Find("a").second;

            // 関数呼び出し時に変数の使用が伝播するなら
            // use{}に{1, 13}が現れるはず
            testUse(a_slice, std::set<unsigned int>({1, 3, 11}));
        }
    } catch (SAXError e) {
        FAIL();
    }
}
