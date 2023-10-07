cmake_minimum_required(VERSION 3.20)
project (CPluginLoader CXX C)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(libs)

add_library(Cloader SHARED plugin.cpp)
target_link_libraries(Cloader PUBLIC tinyCCWrapper)

add_executable (tst "test/test.cpp" ../../counter.cpp)
target_link_libraries(tst PUBLIC tinyCCWrapper)
