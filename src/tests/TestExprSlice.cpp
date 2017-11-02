#include <gtest/gtest.h>
#include <srcSlice.hpp>
#include "TestHelper.h"

TEST(SliceTest, TestExpr) {
    std::string srcmlStr = pathToSrcml("testsrcSlice.cpp", "/src/tests/samples/basic/ExprSlice.cpp");

    try {
        //Run srcSlice
        srcSlice sslice(srcmlStr, 0);
        /*test sum's slice*/

        std::cerr << std::endl << "================= TESTING var's SLICE =================" << std::endl;
        {
            assert(sslice.SetContext("testsrcSlice.cpp", "main", 1));
            auto varSlice = sslice.Find("var");
            std::set<unsigned int> defanswer = {9};
            std::set<unsigned int> useanswer = {9, 10};

            OutputCompare(varSlice.second.def, defanswer);
            OutputCompare(varSlice.second.use, useanswer);

            ASSERT_EQ(varSlice.second.def, defanswer);
            ASSERT_EQ(varSlice.second.use, useanswer);
            ASSERT_EQ(varSlice.second.cfunctions.empty(), true);
            ASSERT_EQ(varSlice.second.aliases.empty(), true);
            ASSERT_EQ(varSlice.second.dvars.empty(), true);
        }
        std::cerr << "=================COMPLETE =================" << std::endl;
        std::cerr << std::endl << "================= TESTING temp2's SLICE =================" << std::endl;
        {
            assert(sslice.SetContext("testsrcSlice.cpp", "main", 1));
            auto temp2Slice = sslice.Find("temp2");
            const std::unordered_set<std::string> dvarsanswer = {"var1"};
            std::set<unsigned int> defanswer = {2};
            std::set<unsigned int> useanswer = {3, 6, 10, 13};
            CFuncSet cfuncanswer;
            cfuncanswer.insert(std::make_pair("foo", 1));

            OutputCompare(temp2Slice.second.def, defanswer);
            OutputCompare(temp2Slice.second.use, useanswer);
            OutputCompare(temp2Slice.second.dvars, dvarsanswer);
            OutputCompare(temp2Slice.second.cfunctions, cfuncanswer);
            ASSERT_EQ(temp2Slice.second.def, defanswer);
            ASSERT_EQ(temp2Slice.second.use, useanswer);
            ASSERT_EQ(temp2Slice.second.cfunctions, cfuncanswer);
            ASSERT_EQ(temp2Slice.second.aliases.empty(), true);
            ASSERT_EQ(temp2Slice.second.dvars, dvarsanswer);
        }
        std::cerr << "=================COMPLETE =================" << std::endl;
        std::cerr << std::endl << "================= TESTING temp's SLICE =================" << std::endl;
        {
            assert(sslice.SetContext("testsrcSlice.cpp", "main", 1));
            auto tempSlice = sslice.Find("temp");
            const std::unordered_set<std::string> dvarsanswer = {"var1"};
            std::set<unsigned int> defanswer = {2};
            std::set<unsigned int> useanswer = {3, 5, 6, 8, 9, 13};
            CFuncSet cfuncanswer;
            cfuncanswer.insert(std::make_pair("foo", 1));

            OutputCompare(tempSlice.second.def, defanswer);
            OutputCompare(tempSlice.second.use, useanswer);
            OutputCompare(tempSlice.second.dvars, dvarsanswer);
            OutputCompare(tempSlice.second.cfunctions, cfuncanswer);

            ASSERT_EQ(tempSlice.second.def, defanswer);
            ASSERT_EQ(tempSlice.second.use, useanswer);
            ASSERT_EQ(tempSlice.second.cfunctions, cfuncanswer);
            ASSERT_EQ(tempSlice.second.aliases.empty(), true);
            ASSERT_EQ(tempSlice.second.dvars, dvarsanswer);
        }
        std::cerr << "================= COMPLETE =================" << std::endl;
        std::cerr << std::endl << "================= TESTING var1's SLICE =================" << std::endl;
        {
            assert(sslice.SetContext("testsrcSlice.cpp", "main", 1));
            auto var1Slice = sslice.Find("var1");
            std::set<unsigned int> defanswer = {2, 3};
            std::set<unsigned int> useanswer = {3, 5, 6, 8, 10};
            CFuncSet cfuncanswer;
            cfuncanswer.insert(std::make_pair("foo", 2));

            OutputCompare(var1Slice.second.def, defanswer);
            OutputCompare(var1Slice.second.use, useanswer);
            OutputCompare(var1Slice.second.cfunctions, cfuncanswer);

            ASSERT_EQ(var1Slice.second.def, defanswer);
            ASSERT_EQ(var1Slice.second.use, useanswer);
            ASSERT_EQ(var1Slice.second.cfunctions, cfuncanswer);
            ASSERT_EQ(var1Slice.second.aliases.empty(), true);
            ASSERT_EQ(var1Slice.second.dvars, std::unordered_set<std::string>({"var1"}));
        }
        std::cerr << "================= COMPLETE =================" << std::endl;

    } catch (SAXError e) {
        std::cerr << "ERROR: " << e.message << std::endl;
        FAIL();
    }
}
