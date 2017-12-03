file(GLOB SRCSLICEHANDLER_SOURCE ../cpp/*.cpp)
file(GLOB SRCSLICEHANDLER_SOURCE2
        ../cpp/tables/*.cpp
        ../cpp/core/*.cpp
        ../cpp/helpers/*.cpp
        ../cpp/*.cpp
        )
file(GLOB SRCSLICEHANDLER_INCLUDE
        ../headers/**/*.hpp
        ../headers/*.hpp
        ../headers/**/*.h
        ../headers/*.h)
file(GLOB SRCSLICETEST_SOURCE *.cpp)
file(GLOB SRCSLICETEST_INCLUDE *.hpp)
file(GLOB TESTCASES basic/*.cpp
        issues/*.cpp
        unittest/*.cpp
        )
file(GLOB EXTESTCASES
                issues/TestIssue31.cpp
#        basic/TestIf.cpp
#        basic/TestWhile.cpp
#                issues/TestGlobalIssue3.cpp
        )

link_directories(/usr/local/lib/)

# googletest の設定
set(GTEST_ROOT $ENV{HOME}/oss/googletest/googletest)
include_directories(${GTEST_ROOT}/include/)
link_directories(${GTEST_ROOT}/build/)

# srcMLへの参照を加える
include_directories($ENV{HOME}/github/srcml-fork/src/libsrcml)
link_directories($ENV{HOME}/github/srcml-fork/build/bin)

add_executable(srcslicetest
        ${SRCSLICEHANDLER_SOURCE2}
        ${SRCSLICEHANDLER_INCLUDE}
        ${TESTCASES}
        helpers/TestHelper.cpp helpers/TestHelper.hpp
        )
add_executable(ex_srcslicetest ${SRCSLICEHANDLER_SOURCE2} ${SRCSLICEHANDLER_INCLUDE}
        ${EXTESTCASES}
        helpers/TestHelper.cpp helpers/TestHelper.hpp
        )

target_link_libraries(srcslicetest srcsax_static srcml ${LIBXML2_LIBRARIES}
        gtest gtest_main pthread)
target_link_libraries(ex_srcslicetest srcsax_static srcml ${LIBXML2_LIBRARIES}
        gtest gtest_main pthread)
