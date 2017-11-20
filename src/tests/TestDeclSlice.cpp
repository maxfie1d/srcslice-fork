#include <gtest/gtest.h>
#include <srcSlice.hpp>
#include "TestHelper.hpp"

TEST(SliceTest, TestDecl) {
    std::string srcmlStr = pathToSrcml("testsrcSlice.cpp", "/src/tests/samples/basic/DeclSlice.cpp");

    try {
        //Run srcSlice
        srcSlice sslice(srcmlStr, 0);
        std::cerr << std::endl << "================= TESTING x's SLICE =================" << std::endl;
        {
            assert(sslice.SetContext("testsrcSlice.cpp", "main", 1));
            auto xSlice = sslice.Find("x");
            const std::unordered_set<std::string> dvarsanswer = {"y"};
            std::set<unsigned int> defanswer = {2};
            std::set<unsigned int> useanswer = {4};

            OutputCompare(xSlice.second.def, defanswer);
            OutputCompare(xSlice.second.use, useanswer);
            OutputCompare(xSlice.second.dvars, dvarsanswer);

            ASSERT_EQ(xSlice.second.def, defanswer);
            ASSERT_EQ(xSlice.second.use, useanswer);
            ASSERT_EQ(xSlice.second.cfunctions.empty(), true);
            ASSERT_EQ(xSlice.second.aliases.empty(), true);
            ASSERT_EQ(xSlice.second.dvars, dvarsanswer);
        }
        std::cerr << "================= COMPLETE =================" << std::endl;
        std::cerr << std::endl << "================= TESTING b's SLICE =================" << std::endl;
        {
            assert(sslice.SetContext("testsrcSlice.cpp", "main", 1));
            auto bSlice = sslice.Find("b");
            const std::unordered_set<std::string> dvarsanswer = {"y"};
            std::set<unsigned int> defanswer = {3};
            std::set<unsigned int> useanswer = {4};

            OutputCompare(bSlice.second.def, defanswer);
            OutputCompare(bSlice.second.use, useanswer);
            OutputCompare(bSlice.second.dvars, dvarsanswer);

            ASSERT_EQ(bSlice.second.def, defanswer);
            ASSERT_EQ(bSlice.second.use, useanswer);
            ASSERT_EQ(bSlice.second.cfunctions.empty(), true);
            ASSERT_EQ(bSlice.second.aliases.empty(), true);
            ASSERT_EQ(bSlice.second.dvars, dvarsanswer);
        }
        std::cerr << "================= COMPLETE =================" << std::endl;
        std::cerr << std::endl << "================= TESTING y's SLICE =================" << std::endl;
        {
            assert(sslice.SetContext("testsrcSlice.cpp", "main", 1));
            auto ySlice = sslice.Find("y");
            std::set<unsigned int> defanswer = {4};

            OutputCompare(ySlice.second.def, defanswer);

            ASSERT_EQ(ySlice.second.def, defanswer);
            ASSERT_EQ(ySlice.second.use.empty(), true);
            ASSERT_EQ(ySlice.second.cfunctions.empty(), true);
            ASSERT_EQ(ySlice.second.aliases.empty(), true);
            ASSERT_EQ(ySlice.second.dvars.empty(), true);
        }
        std::cerr << "================= COMPLETE =================" << std::endl;
        std::cerr << std::endl << "================= TESTING str1's SLICE =================" << std::endl;
        {
            assert(sslice.SetContext("testsrcSlice.cpp", "main", 1));
            auto str1Slice = sslice.Find("str1");
            std::set<unsigned int> defanswer = {5};

            OutputCompare(str1Slice.second.def, defanswer);

            ASSERT_EQ(str1Slice.second.def, defanswer);
            ASSERT_EQ(str1Slice.second.use.empty(), true);
            ASSERT_EQ(str1Slice.second.cfunctions.empty(), true);
            ASSERT_EQ(str1Slice.second.aliases.empty(), true);
            ASSERT_EQ(str1Slice.second.dvars.empty(), true);
        }
        std::cerr << "================= COMPLETE =================" << std::endl;
        std::cerr << std::endl << "================= TESTING str2's SLICE =================" << std::endl;
        {
            assert(sslice.SetContext("testsrcSlice.cpp", "main", 1));
            auto str2Slice = sslice.Find("str2");
            std::set<unsigned int> defanswer = {5};

            OutputCompare(str2Slice.second.def, defanswer);

            ASSERT_EQ(str2Slice.second.def, defanswer);
            ASSERT_EQ(str2Slice.second.use.empty(), true);
            ASSERT_EQ(str2Slice.second.cfunctions.empty(), true);
            ASSERT_EQ(str2Slice.second.aliases.empty(), true);
            ASSERT_EQ(str2Slice.second.dvars.empty(), true);
        }
        std::cerr << "================= COMPLETE =================" << std::endl;
        std::cerr << std::endl << "================= TESTING str3's SLICE =================" << std::endl;
        {
            assert(sslice.SetContext("testsrcSlice.cpp", "main", 1));
            auto str3Slice = sslice.Find("str3");
            std::set<unsigned int> defanswer = {5};

            OutputCompare(str3Slice.second.def, defanswer);

            ASSERT_EQ(str3Slice.second.def, defanswer);
            ASSERT_EQ(str3Slice.second.use.empty(), true);
            ASSERT_EQ(str3Slice.second.cfunctions.empty(), true);
            ASSERT_EQ(str3Slice.second.aliases.empty(), true);
            ASSERT_EQ(str3Slice.second.dvars.empty(), true);
        }
        std::cerr << "================= COMPLETE =================" << std::endl;
        std::cerr << std::endl << "================= TESTING str4's SLICE =================" << std::endl;
        {
            assert(sslice.SetContext("testsrcSlice.cpp", "main", 1));
            auto str4Slice = sslice.Find("str4");
            std::set<unsigned int> defanswer = {6};
            std::set<unsigned int> useanswer = {8};

            OutputCompare(str4Slice.second.def, defanswer);
            OutputCompare(str4Slice.second.use, useanswer);

            ASSERT_EQ(str4Slice.second.def, defanswer);
            ASSERT_EQ(str4Slice.second.use, useanswer);
            ASSERT_EQ(str4Slice.second.cfunctions.empty(), true);
            ASSERT_EQ(str4Slice.second.aliases.empty(), true);
            ASSERT_EQ(str4Slice.second.dvars.empty(), true);
        }
        std::cerr << "================= COMPLETE =================" << std::endl;
        std::cerr << std::endl << "================= TESTING str5's SLICE =================" << std::endl;
        {
            assert(sslice.SetContext("testsrcSlice.cpp", "main", 1));
            auto str5Slice = sslice.Find("str5");
            std::set<unsigned int> defanswer = {8};
            std::unordered_set<std::string> aliasanswer = {"str4"};

            OutputCompare(str5Slice.second.def, defanswer);
            OutputCompare<std::unordered_set<std::string>>(str5Slice.second.aliases, aliasanswer);

            ASSERT_EQ(str5Slice.second.def, defanswer);
            ASSERT_EQ(str5Slice.second.use.empty(), true);
            ASSERT_EQ(str5Slice.second.cfunctions.empty(), true);
            ASSERT_EQ(str5Slice.second.aliases, aliasanswer);
            ASSERT_EQ(str5Slice.second.dvars.empty(), true);
        }
        std::cerr << "================= COMPLETE =================" << std::endl;
        std::cerr << std::endl << "================= TESTING str6's SLICE =================" << std::endl;
        {
            assert(sslice.SetContext("testsrcSlice.cpp", "main", 1));
            auto str6Slice = sslice.Find("str6");
            std::set<unsigned int> defanswer = {6};

            OutputCompare(str6Slice.second.def, defanswer);

            ASSERT_EQ(str6Slice.second.def, defanswer);
            ASSERT_EQ(str6Slice.second.use.empty(), true);
            ASSERT_EQ(str6Slice.second.cfunctions.empty(), true);
            ASSERT_EQ(str6Slice.second.aliases.empty(), true);
            ASSERT_EQ(str6Slice.second.dvars.empty(), true);
        }
        std::cerr << "================= COMPLETE =================" << std::endl;
        std::cerr << std::endl << "================= TESTING mp1's SLICE =================" << std::endl;
        {
            assert(sslice.SetContext("testsrcSlice.cpp", "main", 1));
            auto mp1Slice = sslice.Find("mp1");
            std::set<unsigned int> defanswer = {7};

            OutputCompare(mp1Slice.second.def, defanswer);

            ASSERT_EQ(mp1Slice.second.def, defanswer);
            ASSERT_EQ(mp1Slice.second.use.empty(), true);
            ASSERT_EQ(mp1Slice.second.cfunctions.empty(), true);
            ASSERT_EQ(mp1Slice.second.aliases.empty(), true);
            ASSERT_EQ(mp1Slice.second.dvars.empty(), true);
        }
        std::cerr << "================= COMPLETE =================" << std::endl;

    } catch (SAXError e) {
        std::cerr << "ERROR: " << e.message << std::endl;
        FAIL();
    }
}
