#include <libxml/parser.h>
#include <helpers/FileHelper.h>
#include "../helpers/TestHelper.hpp"
#include "helpers/srcMLHelper.h"


TEST(UnitTest, TestSortingUnitElement) {
    std::string srcmlStr = readFileAsStr(resolvePath("/src/tests/samples/jamss-app.tar.gz.xml").c_str());
    std::string result = reconstructSrcMLStringForSrcSlice(srcmlStr);
    std::cout << result << std::endl;
}
