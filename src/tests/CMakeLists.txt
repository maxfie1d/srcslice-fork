file(GLOB SRCSLICEHANDLER_SOURCE ../cpp/*.cpp)
file(GLOB SRCSLICEHANDLER_SOURCE2 ../cpp/srcSlice*.cpp)
file(GLOB SRCSLICEHANDLER_INCLUDE ../headers/*.hpp)
file(GLOB SRCSLICETEST_SOURCE *.cpp)
file(GLOB SRCSLICETEST_INCLUDE *.hpp)
file(GLOB TESTCASES TestJson.cpp)

link_directories(/usr/local/lib/)

# googletest の設定
set(GTEST_ROOT $ENV{HOME}/oss/googletest/googletest)
include_directories(${GTEST_ROOT}/include/)
link_directories(${GTEST_ROOT}/build/)

# srcMLへの参照を加える
include_directories($ENV{HOME}/github/srcml-fork/src/libsrcml)
link_directories($ENV{HOME}/github/srcml-fork/build/bin)

add_executable(srcslicetest ${SRCSLICEHANDLER_SOURCE2} ${SRCSLICEHANDLER_INCLUDE}
        TempTest.cpp
        TestPrimitiveTypesSlice.cpp
        TestDeclSlice.cpp
        TestExprSlice.cpp
        TestDotAndMemberAccessSlice.cpp
        TestApp1.cpp
        TestIf.cpp
        TestWhile.cpp
        TestIssue2.cpp
        TestGlobalIssue3.cpp
        TestHelper.cpp TestHelper.h)
add_executable(ex_srcslicetest ${SRCSLICEHANDLER_SOURCE2} ${SRCSLICEHANDLER_INCLUDE}
        ${TESTCASES} TestHelper.cpp TestHelper.h)

target_link_libraries(srcslicetest srcsax_static srcml ${LIBXML2_LIBRARIES}
        gtest gtest_main pthread)
target_link_libraries(ex_srcslicetest srcsax_static srcml ${LIBXML2_LIBRARIES}
        gtest gtest_main pthread)
