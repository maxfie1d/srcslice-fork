#include <gtest/gtest.h>
#include <srcSlice.hpp>
#include "TestHelper.hpp"

TEST(SliceTest, TestDotAndMemberAccess) {
    std::string srcmlStr = pathToSrcml("testsrcSlice.cpp", "/src/tests/samples/basic/DotAndMemberAccess.cpp");

    try {
        //Run srcSlice
        srcSlice sslice(srcmlStr, 0);
        /*test sum's slice*/
//        std::cerr << std::endl << "================= TESTING y's SLICE =================" << std::endl;
        {
//            std::cerr << "TESTING y's SLICE" << std::endl;
            assert(sslice.SetContext("testsrcSlice.cpp", "main", 1));
            auto y_slice = sslice.Find("y").second;

            std::set<unsigned int> defanswer = {7};
            std::set<unsigned int> useanswer = {8, 9, 10};

            testDef(&y_slice, defanswer);
            testUse(&y_slice, useanswer);
        }
//        std::cerr << "================= COMPLETE =================" << std::endl;
//        std::cerr << std::endl << "================= TESTING x's SLICE =================" << std::endl;
        {
//            std::cerr << "TESTING x's SLICE" << std::endl;
            assert(sslice.SetContext("testsrcSlice.cpp", "main", 10));
            auto x_slice = sslice.Find("x").second;

            std::set<unsigned int> defanswer = {2};
            std::set<unsigned int> useanswer = {4, 5, 6};
            testDef(&x_slice, defanswer);
            testUse(&x_slice, useanswer);
        }
//        std::cerr << "================= COMPLETE =================" << std::endl;
    } catch (SAXError e) {
        std::cerr << "ERROR: " << e.message << std::endl;
        FAIL();
    }
}
