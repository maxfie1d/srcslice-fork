#include <gtest/gtest.h>
#include <SliceProfile.hpp>


TEST(SrcSliceUnitTest, TestFunctionData) {
    FunctionData my_func_fd;
    my_func_fd.functionName = "my_func";
    my_func_fd.fileName = "/app/src/module/math.c";

    FunctionData my_func2_fd;
    my_func2_fd.functionName = "my_func2";
    my_func2_fd.fileName = "/app/src/main.c";
    std::cout << "my_funcの関数ID:  " << my_func_fd.computeId() << std::endl;
    std::cout << "my_func2の関数ID: " << my_func2_fd.computeId() << std::endl;
}
