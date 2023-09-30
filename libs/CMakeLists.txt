cmake_minimum_required(VERSION 3.13)
include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)
project (tinyCCWrapper CXX)

ExternalProject_Add(TINYCC
    DOWNLOAD_COMMAND ""
    SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/tinycc
    CONFIGURE_COMMAND cd ${CMAKE_CURRENT_LIST_DIR}/tinycc && ./configure --debug --enable-static --with-libgcc --with-selinux
    BUILD_COMMAND cd ${CMAKE_CURRENT_LIST_DIR}/tinycc && make
    BUILD_BYPRODUCTS ${CMAKE_CURRENT_LIST_DIR}/tinycc/libtcc.a
    INSTALL_COMMAND ""
    TEST_COMMAND ""
)


include(${CMAKE_CURRENT_LIST_DIR}/includeable.cmake)
make_includeable(${CMAKE_CURRENT_LIST_DIR}/tinycc/libtcc.h ${CMAKE_CURRENT_BINARY_DIR}/tinycc/libtcc.h.hpp)
make_binary_includeable(${CMAKE_CURRENT_LIST_DIR}/tinycc/libtcc1.a ${CMAKE_CURRENT_BINARY_DIR}/tinycc/libtcc1.a.hpp)

# Wrapper library to make it all work :D
add_library(${PROJECT_NAME} STATIC empty.cpp ${CMAKE_CURRENT_BINARY_DIR}/tinycc/libtcc.h.hpp ${CMAKE_CURRENT_BINARY_DIR}/tinycc/libtcc1.a.hpp)
add_dependencies(${PROJECT_NAME} TINYCC)
target_link_libraries(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/tinycc/libtcc.a)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/tinycc ${CMAKE_CURRENT_BINARY_DIR})
target_compile_definitions(${PROJECT_NAME} PUBLIC TCC_LIBRARY_PATH=\"${CMAKE_CURRENT_LIST_DIR}/tinycc\")

# function(target_setup_tcc_paths TARGET)
# target_compile_definitions(TARGET PU
#   <INTERFACE|PUBLIC|PRIVATE> [items1...]
#   [<INTERFACE|PUBLIC|PRIVATE> [items2...] ...])
# endfunction()


