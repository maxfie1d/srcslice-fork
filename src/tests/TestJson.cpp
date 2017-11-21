#include <gtest/gtest.h>
#include <json.hpp>

using json = nlohmann::json;

TEST(LibTest, TestJson) {
    json j;
    j["vars"] = "csv_vars";
    j["funcs"] = "csv_funcs";
    std::cout << j.dump(4) << std::endl;
}
