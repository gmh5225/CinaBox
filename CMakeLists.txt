cmake_minimum_required(VERSION 3.13)
project (CPluginLoader)


add_subdirectory(libs)

# Wrapper library to make it all work :D
add_executable(test "test.cpp" ../../counter.cpp)
target_link_libraries(test PUBLIC tinyCCWrapper)
set_target_properties(test PROPERTIES CXX_STANDARD 20)
# target_setup_tcc_paths(test)


