#include <gtest/gtest.h>
#include <srcSlice.hpp>
#include "TestFlatSlice.hpp"
#include "TestHelper.h"

TEST(SliceTest, TestPrimitiveTypes) {
    std::string srcStr = readFileAsStr("../../src/tests/samples/basic/FlatSliceOne.cpp");
    std::string srcmlStr = StringToSrcML("testsrcSlice.cpp", FlatSlicePrograms::FlatSliceOne());

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

            assert(iSlice.second.use == useanswer);
            assert(iSlice.second.cfunctions == cfuncanswer);
            assert(iSlice.second.aliases.empty());
            assert(iSlice.second.dvars == dvarsanswer);
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

            assert(sumSlice.second.def == defanswer);
            assert(sumSlice.second.use == useanswer);
            assert(sumSlice.second.cfunctions == cfuncanswer);
            assert(sumSlice.second.aliases.empty());
            assert(sumSlice.second.dvars.empty());
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

            assert(zSlice.second.def == defanswer);
            assert(zSlice.second.use == useanswer);
            assert(zSlice.second.cfunctions.empty());
            assert(zSlice.second.aliases.empty());
            assert(zSlice.second.dvars.empty());
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

            assert(ySlice.second.def == defanswer);
            assert(ySlice.second.use == useanswer);
            assert(ySlice.second.aliases == aliasanswer);
            assert(ySlice.second.cfunctions.empty());
            assert(ySlice.second.dvars.empty());
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
            assert(xSlice.second.def == defanswer);
            assert(xSlice.second.use == useanswer);
            assert(xSlice.second.cfunctions == cfuncanswer);
            assert(xSlice.second.aliases == aliasanswer);
            assert(xSlice.second.dvars.empty());
            std::cerr << "================= COMPLETE =================" << std::endl;
        }

    } catch (SAXError e) {
        std::cerr << "ERROR: " << e.message << std::endl;
    }
}
