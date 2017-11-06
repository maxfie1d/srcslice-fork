#include <gtest/gtest.h>
#include <srcSlice.hpp>
#include "TestHelper.h"

TEST(SliceTest, TestReflexionTest) {
    std::string srcmlStr = pathToSrcml("reflexion-test.c", "/src/tests/samples/reflexion-test.c");

    try {
        srcSlice sslice(srcmlStr, 0);
        {
            // 変数aのテスト
            assert(sslice.SetContext("reflexion-test.c", "main", 1));
        }
    } catch (SAXError e) {
        FAIL();
    }
}
