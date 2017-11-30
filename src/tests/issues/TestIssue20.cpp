#include <gtest/gtest.h>
#include <srcSlice.hpp>
#include "../helpers/TestHelper.hpp"

TEST(SliceTest, TestIssue20) {
    std::string srcmlStr = pathToSrcml("issue20.c", "/src/tests/samples/issue20.c");

    try {
        srcSlice sslice(srcmlStr, 0);
        {
            assert(sslice.SetContext("issue20.c", "main", 3));

            // 変数xはextern宣言なので、追加されずにスキップされる
            // ことを確かめる
            auto x_slice = sslice.Find("x");
            ASSERT_EQ(x_slice.first, false);
        }
    } catch (SAXError e) {
        FAIL();
    }
}
