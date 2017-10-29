#include <gtest/gtest.h>
#include <srcSlice.hpp>
#include "TestFlatSlice.hpp"
#include "TestHelper.h"

TEST(SliceTest, TestPrimitiveTypes) {
    std::string resolvedPath = resolvePath(std::string("/src/tests/samples/basic/FlatSliceOne.cpp"));
    std::string srcStr = readFileAsStr(resolvedPath.c_str());
    std::string srcmlStr = StringToSrcML("testsrcSlice.cpp", srcStr);

    try {
        //Run srcSlice
        srcSlice sslice(srcmlStr, 0);
        /*Test i's slice*/
        {
            std::cerr << std::endl << "================= TESTING i's SLICE =================" << std::endl;
            assert(sslice.SetContext("testsrcSlice.cpp", "main", 9));
            auto iSlice = sslice.Find("i");

            const std::set<unsigned int> defanswer = {11};
            const std::set<unsigned int> useanswer = {1, 2, 3, 5, 7, 12, 13, 15, 16, 17};
            const std::unordered_set<std::string> dvarsanswer = {"sum"};
            CFuncSet cfuncanswer;
            cfuncanswer.insert(std::make_pair("foo", 2));
            cfuncanswer.insert(std::make_pair("fun", 1));

            OutputCompare(iSlice.second.def, defanswer);
            OutputCompare(iSlice.second.use, useanswer);
            OutputCompare(iSlice.second.dvars, dvarsanswer);
            OutputCompare(iSlice.second.cfunctions, cfuncanswer);

            ASSERT_EQ(iSlice.second.use, useanswer);
            ASSERT_EQ(iSlice.second.cfunctions, cfuncanswer);
            ASSERT_EQ(iSlice.second.aliases.empty(), true);
            ASSERT_EQ(iSlice.second.dvars, dvarsanswer);
            std::cerr << "================= COMPLETE =================" << std::endl;
        }
        /*test sum's slice*/
        {
            std::cerr << std::endl << "================= TESTING sum's SLICE =================" << std::endl;
            assert(sslice.SetContext("testsrcSlice.cpp", "main", 10));
            auto sumSlice = sslice.Find("sum");

            std::set<unsigned int> defanswer = {10, 17};
            std::set<unsigned int> useanswer = {1, 2, 3, 5, 6, 13, 15};
            CFuncSet cfuncanswer;
            cfuncanswer.insert(std::make_pair("fun", 1));
            cfuncanswer.insert(std::make_pair("foo", 1));

            OutputCompare(sumSlice.second.def, defanswer);
            OutputCompare(sumSlice.second.use, useanswer);
            OutputCompare(sumSlice.second.cfunctions, cfuncanswer);

            ASSERT_EQ(sumSlice.second.def, defanswer);
            ASSERT_EQ(sumSlice.second.use, useanswer);
            ASSERT_EQ(sumSlice.second.cfunctions, cfuncanswer);
            ASSERT_EQ(sumSlice.second.aliases.empty(), true);
            ASSERT_EQ(sumSlice.second.dvars.empty(), true);
            std::cerr << "================= COMPLETE =================" << std::endl;
        }
        {
            std::cerr << std::endl << "================= TESTING z's SLICE =================" << std::endl;
            assert(sslice.SetContext("testsrcSlice.cpp", "fun", 1));
            auto zSlice = sslice.Find("z");

            std::set<unsigned int> defanswer = {1};
            std::set<unsigned int> useanswer = {2, 3};

            OutputCompare(zSlice.second.def, defanswer);
            OutputCompare(zSlice.second.use, useanswer);

            ASSERT_EQ(zSlice.second.def, defanswer);
            ASSERT_EQ(zSlice.second.use, useanswer);
            ASSERT_EQ(zSlice.second.cfunctions.empty(), true);
            ASSERT_EQ(zSlice.second.aliases.empty(), true);
            ASSERT_EQ(zSlice.second.dvars.empty(), true);
            std::cerr << "================= COMPLETE =================" << std::endl;
        }
        {
            std::cerr << std::endl << "================= TESTING y's SLICE =================" << std::endl;
            assert(sslice.SetContext("testsrcSlice.cpp", "foo", 4));
            auto ySlice = sslice.Find("y");

            std::set<unsigned int> defanswer = {5};
            std::set<unsigned int> useanswer = {7};
            std::unordered_set<std::string> aliasanswer = {"i"};

            OutputCompare(ySlice.second.def, defanswer);
            OutputCompare(ySlice.second.use, useanswer);

            ASSERT_EQ(ySlice.second.def, defanswer);
            ASSERT_EQ(ySlice.second.use, useanswer);
            ASSERT_EQ(ySlice.second.aliases, aliasanswer);
            ASSERT_EQ(ySlice.second.cfunctions.empty(), true);
            ASSERT_EQ(ySlice.second.dvars.empty(), true);
            std::cerr << "================= COMPLETE =================" << std::endl;
        }
        {
            std::cerr << std::endl << "================= TESTING x's SLICE =================" << std::endl;
            assert(sslice.SetContext("testsrcSlice.cpp", "foo", 4));
            auto xSlice = sslice.Find("x");

            std::set<unsigned int> defanswer = {5};
            std::set<unsigned int> useanswer = {1, 2, 3, 6};

            CFuncSet cfuncanswer;
            cfuncanswer.insert(std::make_pair("fun", 1));

            std::unordered_set<std::string> aliasanswer = {"sum"};

            OutputCompare(xSlice.second.def, defanswer);
            OutputCompare(xSlice.second.use, useanswer);
            OutputCompare<std::unordered_set<std::string>>(xSlice.second.aliases, aliasanswer);
            OutputCompare(xSlice.second.cfunctions, cfuncanswer);
            ASSERT_EQ(xSlice.second.def, defanswer);
            ASSERT_EQ(xSlice.second.use, useanswer);
            ASSERT_EQ(xSlice.second.cfunctions, cfuncanswer);
            ASSERT_EQ(xSlice.second.aliases, aliasanswer);
            ASSERT_EQ(xSlice.second.dvars.empty(), true);
            std::cerr << "================= COMPLETE =================" << std::endl;
        }

    } catch (SAXError e) {
        std::cerr << "ERROR: " << e.message << std::endl;
    }
}
