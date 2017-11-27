#include <gtest/gtest.h>
#include "FunctionTable.h"

FunctionData make_function_data_from_name(std::string func_name){
    FunctionData fd;
    fd.functionName = func_name;
    return fd;
}


TEST(UnitTest, FunctionTableTest) {
    FunctionTable functionTable;

    functionTable.add("func1", make_function_data_from_name("func1"));
    functionTable.add("func2", make_function_data_from_name("func2"));
    functionTable.add("func3", make_function_data_from_name("func3"));

    auto func1 = functionTable.findByName("func1");

    if (!func1) {
        FAIL();
    }
}
