#include <gtest/gtest.h>
#include <srcSlice.hpp>
#include "../helpers/TestHelper.hpp"

TEST(SliceTest, TestExpr) {
    std::string srcmlStr = pathToSrcml("testsrcSlice.cpp", "/src/tests/samples/basic/ExprSlice.cpp");

    try {
        //Run srcSlice
        srcSlice sslice(srcmlStr, 0);
        /*test sum's slice*/

//        std::cerr << std::endl << "================= TESTING var's SLICE =================" << std::endl;
        {
            assert(sslice.SetContext("testsrcSlice.cpp", "main", 1));
            auto varSlice = sslice.Find("var").second;
            std::set<unsigned int> defanswer = {9};
            std::set<unsigned int> useanswer = {9, 10};

//            OutputCompare(varSlice.second.def, defanswer);
//            OutputCompare(varSlice.second.use, useanswer);

            testDef(&varSlice, defanswer);
            testUse(&varSlice, useanswer);
            assertCfuncsEmpty(&varSlice);
            ASSERT_EQ(varSlice.aliases.empty(), true);
            assertDvarsEmpty(&varSlice);
        }
//        std::cerr << "=================COMPLETE =================" << std::endl;
//        std::cerr << std::endl << "================= TESTING temp2's SLICE =================" << std::endl;
        {
            assert(sslice.SetContext("testsrcSlice.cpp", "main", 1));
            auto temp2Slice = sslice.Find("temp2").second;
            const std::unordered_set<std::string> dvarsanswer = {"var1"};
            std::set<unsigned int> defanswer = {2};
            std::set<unsigned int> useanswer = {3, 6, 10, 13};
            std::set<CfuncShortData> cfuncanswer(
                    {
                            CfuncShortData("foo", 1)
                    }
            );

//            OutputCompare(temp2Slice.second.def, defanswer);
//            OutputCompare(temp2Slice.second.use, useanswer);
//            OutputCompare(temp2Slice.second.dvars, dvarsanswer);
//            OutputCompare(temp2Slice.second.cfunctions, cfuncanswer);
            testDef(&temp2Slice, defanswer);
            testUse(&temp2Slice, useanswer);
            testCfuncs(&temp2Slice, cfuncanswer);
            ASSERT_EQ(temp2Slice.aliases.empty(), true);
            testDvars(&temp2Slice, std::set<std::string>({"var1"}));
        }
//        std::cerr << "=================COMPLETE =================" << std::endl;
//        std::cerr << std::endl << "================= TESTING temp's SLICE =================" << std::endl;
        {
            assert(sslice.SetContext("testsrcSlice.cpp", "main", 1));
            auto tempSlice = sslice.Find("temp").second;
            const std::set<std::string> dvarsanswer = {"var1"};
            std::set<unsigned int> defanswer = {2};
            std::set<unsigned int> useanswer = {3, 5, 6, 8, 9, 13};
            std::set<CfuncShortData> cfuncanswer(
                    {
                            CfuncShortData("foo", 1)
                    }
            );
//            OutputCompare(tempSlice.second.def, defanswer);
//            OutputCompare(tempSlice.second.use, useanswer);
//            OutputCompare(tempSlice.second.dvars, dvarsanswer);
//            OutputCompare(tempSlice.second.cfunctions, cfuncanswer);

            testDef(&tempSlice, defanswer);
            testUse(&tempSlice, useanswer);
            testCfuncs(&tempSlice, cfuncanswer);
            ASSERT_EQ(tempSlice.aliases.empty(), true);
            testDvars(&tempSlice, dvarsanswer);
        }
//        std::cerr << "================= COMPLETE =================" << std::endl;
//        std::cerr << std::endl << "================= TESTING var1's SLICE =================" << std::endl;
        {
            assert(sslice.SetContext("testsrcSlice.cpp", "main", 1));
            auto var1Slice = sslice.Find("var1").second;
            std::set<unsigned int> defanswer = {2, 3};
            std::set<unsigned int> useanswer = {3, 5, 6, 8, 10};
            std::set<CfuncShortData> cfuncanswer(
                    {
                            CfuncShortData("foo", 2)

                    });

//            OutputCompare(var1Slice.second.def, defanswer);
//            OutputCompare(var1Slice.second.use, useanswer);
//            OutputCompare(var1Slice.second.cfunctions, cfuncanswer);

            testDef(&var1Slice, defanswer);
            testUse(&var1Slice, useanswer);
            testCfuncs(&var1Slice, cfuncanswer);
            ASSERT_EQ(var1Slice.aliases.empty(), true);
            testDvars(&var1Slice, std::set<std::string>({"var1"}));
        }
//        std::cerr << "================= COMPLETE =================" << std::endl;

    } catch (SAXError e) {
        std::cerr << "ERROR: " << e.message << std::endl;
        FAIL();
    }
}
