cmake_minimum_required(VERSION 3.20)
project (CInABox CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_POSITION_INDEPENDENT_CODE true)

add_subdirectory(libs)

option(STATIC_LINK "Weather or not the library should be linked statically using the default target (can be overriden by using the specific targets)" ON)
option(BUILD_TESTS "Weather or not we should build tests" ${PROJECT_IS_TOP_LEVEL})

add_library(CInABox-static STATIC cinabox.cpp)
target_link_libraries(CInABox-static PUBLIC tinyCCWrapper)
target_include_directories(CInABox-static PUBLIC ${CMAKE_CURRENT_LIST_DIR})

# add_library(CInABox-shared SHARED cinabox.cpp)
# target_link_libraries(CInABox-shared PUBLIC tinyCCWrapper)
# target_include_directories(CInABox-shared PUBLIC ${CMAKE_CURRENT_LIST_DIR})

if(STATIC_LINK)
    add_library(cinabox ALIAS CInABox-static)
else()
    add_library(cinabox ALIAS CInABox-shared)
endif()

# if(BUILD_TESTS)
    add_executable (tst "test/test.cpp")
    target_link_libraries(tst PUBLIC CInABox-static)
# endif()
