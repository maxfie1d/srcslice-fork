file(GLOB SRCSLICEHANDLER_SOURCE cpp/*.cpp)
file(GLOB SRCSLICEHANDLER_SOURCE2 cpp/srcSlice*.cpp)
file(GLOB SRCSLICEHANDLER_INCLUDE headers/*.hpp)

link_directories(/usr/local/lib/)

# srcMLへの参照を加える
include_directories($ENV{HOME}/github/srcml-fork/src/libsrcml)
link_directories($ENV{HOME}/github/srcml-fork/build/bin)

add_executable(srcSlice ${SRCSLICEHANDLER_SOURCE} ${SRCSLICEHANDLER_INCLUDE})

target_link_libraries(srcSlice srcsax_static ${LIBXML2_LIBRARIES})