file(GLOB SRCSLICEHANDLER_SOURCE cpp/*.cpp)
file(GLOB SRCSLICEHANDLER_SOURCE2 cpp/srcSlice*.cpp)
file(GLOB SRCSLICEHANDLER_INCLUDE headers/*.hpp)
file(GLOB SRCSLICETEST_SOURCE tests/*.cpp)
file(GLOB SRCSLICETEST_INCLUDE tests/*.hpp)

link_directories(/usr/local/lib/)

# googletest の設定
set(GTEST_ROOT $ENV{HOME}/oss/googletest/googletest)
include_directories(${GTEST_ROOT}/include/)
link_directories(${GTEST_ROOT}/build/)

# srcMLへの参照を加える
include_directories($ENV{HOME}/github/srcml-fork/src/libsrcml)
link_directories($ENV{HOME}/github/srcml-fork/build/bin)

add_executable(srcSlice ${SRCSLICEHANDLER_SOURCE} ${SRCSLICEHANDLER_INCLUDE})
add_executable(srcslicetest ${SRCSLICEHANDLER_SOURCE2} ${SRCSLICEHANDLER_INCLUDE} tests/OldTest.cpp tests/TestHelper.cpp tests/TestHelper.h)
add_executable(srcslicetest_new ${SRCSLICEHANDLER_SOURCE2} ${SRCSLICEHANDLER_INCLUDE} tests/TempTest.cpp tests/TestPrimitiveTypesSlice.cpp tests/TestDeclSlice.cpp tests/TestExprSlice.cpp tests/TestDotAndMemberAccessSlice.cpp tests/TestHelper.cpp tests/TestHelper.h)

target_link_libraries(srcSlice srcsax_static ${LIBXML2_LIBRARIES})
target_link_libraries(srcslicetest srcsax_static srcml ${LIBXML2_LIBRARIES})
target_link_libraries(srcslicetest_new srcsax_static srcml ${LIBXML2_LIBRARIES} gtest gtest_main pthread)
