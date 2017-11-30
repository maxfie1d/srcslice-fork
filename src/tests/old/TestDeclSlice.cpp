#include <gtest/gtest.h>
#include <srcSlice.hpp>
#include "../helpers/TestHelper.hpp"

TEST(SliceTest, TestDecl) {
    std::string srcmlStr = pathToSrcml("testsrcSlice.cpp", "/src/tests/samples/basic/DeclSlice.cpp");

    try {
        //Run srcSlice
        srcSlice sslice(srcmlStr, 0);
//        std::cerr << std::endl << "================= TESTING x's SLICE =================" << std::endl;
        {
            assert(sslice.SetContext("testsrcSlice.cpp", "main", 1));
            auto xSlice = sslice.Find("x").second;
            const std::set<std::string> dvarsanswer = {"y"};
            std::set<unsigned int> defanswer = {2};
            std::set<unsigned int> useanswer = {4};

//            OutputCompare(xSlice.second.def, defanswer);
//            OutputCompare(xSlice.second.use, useanswer);
//            OutputCompare(xSlice.second.dvars, dvarsanswer);

            testDef(&xSlice, defanswer);
            testUse(&xSlice, useanswer);
            assertCfuncsEmpty(&xSlice);
            ASSERT_EQ(xSlice.aliases.empty(), true);
            testDvars(&xSlice, dvarsanswer);
        }
//        std::cerr << "================= COMPLETE =================" << std::endl;
//        std::cerr << std::endl << "================= TESTING b's SLICE =================" << std::endl;
        {
            assert(sslice.SetContext("testsrcSlice.cpp", "main", 1));
            auto bSlice = sslice.Find("b").second;
            const std::set<std::string> dvarsanswer = {"y"};
            std::set<unsigned int> defanswer = {3};
            std::set<unsigned int> useanswer = {4};

//            OutputCompare(bSlice.second.def, defanswer);
//            OutputCompare(bSlice.second.use, useanswer);
//            OutputCompare(bSlice.second.dvars, dvarsanswer);

            testDef(&bSlice, defanswer);
            testUse(&bSlice, useanswer);
            assertCfuncsEmpty(&bSlice);
            ASSERT_EQ(bSlice.aliases.empty(), true);
            testDvars(&bSlice, dvarsanswer);
        }
//        std::cerr << "================= COMPLETE =================" << std::endl;
//        std::cerr << std::endl << "================= TESTING y's SLICE =================" << std::endl;
        {
            assert(sslice.SetContext("testsrcSlice.cpp", "main", 1));
            auto ySlice = sslice.Find("y").second;
            std::set<unsigned int> defanswer = {4};

//            OutputCompare(ySlice.second.def, defanswer);

            testDef(&ySlice, defanswer);
            assertUseEmpty(&ySlice);
            assertCfuncsEmpty(&ySlice);
            ASSERT_EQ(ySlice.aliases.empty(), true);
            assertDvarsEmpty(&ySlice);
        }
//        std::cerr << "================= COMPLETE =================" << std::endl;
//        std::cerr << std::endl << "================= TESTING str1's SLICE =================" << std::endl;
        {
            assert(sslice.SetContext("testsrcSlice.cpp", "main", 1));
            auto str1Slice = sslice.Find("str1").second;
            std::set<unsigned int> defanswer = {5};

//            OutputCompare(str1Slice.second.def, defanswer);

            testDef(&str1Slice, defanswer);
            assertUseEmpty(&str1Slice);
            assertCfuncsEmpty(&str1Slice);
            ASSERT_EQ(str1Slice.aliases.empty(), true);
            assertDvarsEmpty(&str1Slice);
        }
//        std::cerr << "================= COMPLETE =================" << std::endl;
//        std::cerr << std::endl << "================= TESTING str2's SLICE =================" << std::endl;
        {
            assert(sslice.SetContext("testsrcSlice.cpp", "main", 1));
            auto str2Slice = sslice.Find("str2").second;
            std::set<unsigned int> defanswer = {5};

//            OutputCompare(str2Slice.second.def, defanswer);

            testDef(&str2Slice, defanswer);
            assertUseEmpty(&str2Slice);
            assertCfuncsEmpty(&str2Slice);
            ASSERT_EQ(str2Slice.aliases.empty(), true);
            assertDvarsEmpty(&str2Slice);
        }
//        std::cerr << "================= COMPLETE =================" << std::endl;
//        std::cerr << std::endl << "================= TESTING str3's SLICE =================" << std::endl;
        {
            assert(sslice.SetContext("testsrcSlice.cpp", "main", 1));
            auto str3Slice = sslice.Find("str3").second;
            std::set<unsigned int> defanswer = {5};

//            OutputCompare(str3Slice.second.def, defanswer);

            testDef(&str3Slice, defanswer);
            assertUseEmpty(&str3Slice);
            assertCfuncsEmpty(&str3Slice);
            ASSERT_EQ(str3Slice.aliases.empty(), true);
            assertDvarsEmpty(&str3Slice);
        }
//        std::cerr << "================= COMPLETE =================" << std::endl;
//        std::cerr << std::endl << "================= TESTING str4's SLICE =================" << std::endl;
        {
            assert(sslice.SetContext("testsrcSlice.cpp", "main", 1));
            auto str4Slice = sslice.Find("str4").second;
            std::set<unsigned int> defanswer = {6};
            std::set<unsigned int> useanswer = {8};

//            OutputCompare(str4Slice.second.def, defanswer);
//            OutputCompare(str4Slice.second.use, useanswer);

            testDef(&str4Slice, defanswer);
            testUse(&str4Slice, useanswer);
            assertCfuncsEmpty(&str4Slice);
            ASSERT_EQ(str4Slice.aliases.empty(), true);
            assertDvarsEmpty(&str4Slice);
        }
//        std::cerr << "================= COMPLETE =================" << std::endl;
//        std::cerr << std::endl << "================= TESTING str5's SLICE =================" << std::endl;
        {
            assert(sslice.SetContext("testsrcSlice.cpp", "main", 1));
            auto str5Slice = sslice.Find("str5").second;
            std::set<unsigned int> defanswer = {8};
            std::unordered_set<std::string> aliasanswer = {"str4"};

//            OutputCompare(str5Slice.second.def, defanswer);
//            OutputCompare<std::unordered_set<std::string>>(str5Slice.second.aliases, aliasanswer);

            testDef(&str5Slice, defanswer);
            assertUseEmpty(&str5Slice);
            assertCfuncsEmpty(&str5Slice);
            assertCfuncsEmpty(&str5Slice);
            ASSERT_EQ(str5Slice.aliases, aliasanswer);
            assertDvarsEmpty(&str5Slice);
        }
//        std::cerr << "================= COMPLETE =================" << std::endl;
//        std::cerr << std::endl << "================= TESTING str6's SLICE =================" << std::endl;
        {
            assert(sslice.SetContext("testsrcSlice.cpp", "main", 1));
            auto str6Slice = sslice.Find("str6").second;
            std::set<unsigned int> defanswer = {6};

//            OutputCompare(str6Slice.second.def, defanswer);

            testDef(&str6Slice, defanswer);
            assertUseEmpty(&str6Slice);
            assertCfuncsEmpty(&str6Slice);
            ASSERT_EQ(str6Slice.aliases.empty(), true);
            assertDvarsEmpty(&str6Slice);
        }
//        std::cerr << "================= COMPLETE =================" << std::endl;
//        std::cerr << std::endl << "================= TESTING mp1's SLICE =================" << std::endl;
        {
            assert(sslice.SetContext("testsrcSlice.cpp", "main", 1));
            auto mp1Slice = sslice.Find("mp1").second;
            std::set<unsigned int> defanswer = {7};

//            OutputCompare(mp1Slice.second.def, defanswer);

            testDef(&mp1Slice, defanswer);
            assertUseEmpty(&mp1Slice);
            assertCfuncsEmpty(&mp1Slice);
            ASSERT_EQ(mp1Slice.aliases.empty(), true);
            assertDvarsEmpty(&mp1Slice);
        }
//        std::cerr << "================= COMPLETE =================" << std::endl;

    } catch (SAXError e) {
        std::cerr << "ERROR: " << e.message << std::endl;
        FAIL();
    }
}
