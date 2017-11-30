#include <libxml/parser.h>
#include <FileHelper.h>
#include "../helpers/TestHelper.hpp"
#include "srcMLHelper.h"


TEST(UnitTest, TestSortingUnitElement) {
    std::string srcmlStr = readFileAsStr(resolvePath("/src/tests/samples/jamss-app.tar.gz.xml").c_str());
    std::string result = reconstructSrcMLStringForSrcSlice(srcmlStr);
    std::cout << result << std::endl;
}
