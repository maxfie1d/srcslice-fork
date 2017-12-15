#include <gtest/gtest.h>
#include <core/srcSlice.hpp>
#include "../helpers/TestHelper.hpp"

TEST(SliceTest, TestIssue42) {
    std::string srcmlStr = pathToSrcml("issue42.c", "/src/tests/samples/issue42.c");

    try {
        srcSlice sslice(srcmlStr, 0);
        {
            assert(sslice.SetContext("issue42.c", "main", 1));

            // 制御テーブルのテスト
            auto begin_it = sslice.dictionary.controlTable.begin();
            {
                auto control_data = begin_it->second;
                ControlRange expected(12, 23);
                ASSERT_EQ(control_data.controlRange, expected);

                auto a_slice = sslice.Find("a").second;
                ASSERT_EQ(control_data.vars, std::set<std::string>({a_slice->computeVariableId()}));
            }
            {
                auto control_data = (++begin_it)->second;
                ControlRange expected(15, 19, 22);
                ASSERT_EQ(control_data.controlRange, expected);

                auto c_slice = sslice.Find("c").second;
                ASSERT_EQ(control_data.vars, std::set<std::string>({c_slice->computeVariableId()}));
            }
        }
    } catch (SAXError e) {
        FAIL();
    }
}
