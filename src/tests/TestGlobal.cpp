#include <gtest/gtest.h>
#include <srcSlice.hpp>
#include "TestHelper.h"


TEST(SliceTest, TestGlobal) {
    std::string srcmlStr = pathToSrcml("global.c", "/src/tests/samples/global.c");

    try {
        srcSlice sslice(srcmlStr, 0);
        // srcSlice::setContext の意味は
        // 「このファイル」の「この関数」の (どれどれの変数) についてチェックするという意味らしい
        assert(sslice.SetContext("global.c", "f", 2));
        {
            // グローバル変数gのテスト
            auto g_slice = sslice.Find("g").second;

            for (auto d: g_slice.def) {
                std::cout << "def: " << d << std::endl;
            }

            for (auto u: g_slice.use) {
                std::cout << "use: " << u << std::endl;
            }
//            // def{}のテスト
//            ASSERT_EQ(g_slice.def, std::set<unsigned int>({5}));
        }

    } catch (SAXError e) {
        FAIL();
    }
}
