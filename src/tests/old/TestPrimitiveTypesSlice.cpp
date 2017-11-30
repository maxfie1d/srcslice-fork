#include <gtest/gtest.h>
#include <core/srcSlice.hpp>
#include "../helpers/TestHelper.hpp"

TEST(SliceTest, TestPrimitiveTypes) {
    std::string srcmlStr = pathToSrcml("testsrcSlice.cpp", "/src/tests/samples/basic/FlatSliceOne.cpp");

    try {
        //Run srcSlice
        srcSlice sslice(srcmlStr, 0);
        /*Test i's slice*/
        {
//            std::cerr << std::endl << "================= TESTING i's SLICE =================" << std::endl;
            assert(sslice.SetContext("testsrcSlice.cpp", "main", 9));
            auto iSlice = sslice.Find("i").second;

            const std::set<unsigned int> defanswer = {11};
            const std::set<unsigned int> useanswer = {1, 2, 3, 5, 7, 12, 13, 15, 16, 17};
            const std::set<std::string> dvarsanswer = {"sum"};
            std::set<CfuncShortData> cfuncanswer(
                    {
                            CfuncShortData("foo", 2),
                            CfuncShortData("fun", 1)
                    }
            );
//            OutputCompare(iSlice.second.def, defanswer);
//            OutputCompare(iSlice.second.use, useanswer);
//            OutputCompare(iSlice.second.dvars, dvarsanswer);
//            OutputCompare(iSlice.second.cfunctions, cfuncanswer);

            testUse(&iSlice, useanswer);
            testCfuncs(&iSlice, cfuncanswer);
            ASSERT_EQ(iSlice.aliases.empty(), true);
            testDvars(&iSlice, dvarsanswer);
//            std::cerr << "================= COMPLETE =================" << std::endl;
        }
        /*test sum's slice*/
        {
//            std::cerr << std::endl << "================= TESTING sum's SLICE =================" << std::endl;
            assert(sslice.SetContext("testsrcSlice.cpp", "main", 10));
            auto sumSlice = sslice.Find("sum").second;

            std::set<unsigned int> defanswer = {10, 17};
            std::set<unsigned int> useanswer = {1, 2, 3, 5, 6, 13, 15, 17};
            std::set<CfuncShortData> cfuncanswer(
                    {
                            CfuncShortData("fun", 1),
                            CfuncShortData("foo", 1)
                    }
            );

//            OutputCompare(sumSlice.second.def, defanswer);
//            OutputCompare(sumSlice.second.use, useanswer);
//            OutputCompare(sumSlice.second.cfunctions, cfuncanswer);

            testDef(&sumSlice, defanswer);
            testUse(&sumSlice, useanswer);
            testCfuncs(&sumSlice, cfuncanswer);
            ASSERT_EQ(sumSlice.aliases.empty(), true);
            testDvars(&sumSlice, std::set<std::string>({"sum"}));
//            std::cerr << "================= COMPLETE =================" << std::endl;
        }
        {
//            std::cerr << std::endl << "================= TESTING z's SLICE =================" << std::endl;
            assert(sslice.SetContext("testsrcSlice.cpp", "fun", 1));
            auto zSlice = sslice.Find("z").second;

            std::set<unsigned int> defanswer = {1};
            std::set<unsigned int> useanswer = {2, 3};

//            OutputCompare(zSlice.second.def, defanswer);
//            OutputCompare(zSlice.second.use, useanswer);

            testDef(&zSlice, defanswer);
            testUse(&zSlice, useanswer);
            assertCfuncsEmpty(&zSlice);
            ASSERT_EQ(zSlice.aliases.empty(), true);
            assertDvarsEmpty(&zSlice);
//            std::cerr << "================= COMPLETE =================" << std::endl;
        }
        {
//            std::cerr << std::endl << "================= TESTING y's SLICE =================" << std::endl;
            assert(sslice.SetContext("testsrcSlice.cpp", "foo", 4));
            auto ySlice = sslice.Find("y").second;

            std::set<unsigned int> defanswer = {5};
            std::set<unsigned int> useanswer = {7};
            std::unordered_set<std::string> aliasanswer = {"i"};

//            OutputCompare(ySlice.second.def, defanswer);
//            OutputCompare(ySlice.second.use, useanswer);

            testDef(&ySlice, defanswer);
            testUse(&ySlice, useanswer);
            ASSERT_EQ(ySlice.aliases, aliasanswer);
            assertCfuncsEmpty(&ySlice);
            assertDvarsEmpty(&ySlice);
//            std::cerr << "================= COMPLETE =================" << std::endl;
        }
        {
//            std::cerr << std::endl << "================= TESTING x's SLICE =================" << std::endl;
            assert(sslice.SetContext("testsrcSlice.cpp", "foo", 4));
            auto xSlice = sslice.Find("x").second;

            std::set<unsigned int> defanswer = {5};
            std::set<unsigned int> useanswer = {1, 2, 3, 6};

            std::set<CfuncShortData> cfuncanswer(
                    {
                            CfuncShortData("fun", 1)
                    }
            );

            std::unordered_set<std::string> aliasanswer = {"sum"};

//            OutputCompare(xSlice.second.def, defanswer);
//            OutputCompare(xSlice.second.use, useanswer);
//            OutputCompare<std::unordered_set<std::string>>(xSlice.second.aliases, aliasanswer);
//            OutputCompare(xSlice.second.cfunctions, cfuncanswer);

            testDef(&xSlice, defanswer);
            testUse(&xSlice, useanswer);
            testCfuncs(&xSlice, cfuncanswer);
            ASSERT_EQ(xSlice.aliases, aliasanswer);
            assertDvarsEmpty(&xSlice);
//            std::cerr << "================= COMPLETE =================" << std::endl;
        }

    } catch (SAXError e) {
        std::cerr << "ERROR: " << e.message << std::endl;
        FAIL();
    }
}
