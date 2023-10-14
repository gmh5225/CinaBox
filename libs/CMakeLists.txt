cmake_minimum_required(VERSION 3.13)
include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)
project (tinyCCWrapper CXX)

option(FIRST_BUILD "Weather or not files should attempt to be copied!" OFF)

function (invertBoolean varName varValue)
  if(${varValue})
    set(${varName} false PARENT_SCOPE)
  else()
    set(${varName} true PARENT_SCOPE)
  endif()
endfunction()

ExternalProject_Add(TINYCC
    DOWNLOAD_COMMAND ""
    SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/tinycc
    CONFIGURE_COMMAND cd ${CMAKE_CURRENT_LIST_DIR}/tinycc && ./configure --debug --enable-static --with-selinux --extra-cflags="-fPIC" --extra-ldflags="-fPIC"
    BUILD_COMMAND cd ${CMAKE_CURRENT_LIST_DIR}/tinycc && make
    BUILD_BYPRODUCTS ${CMAKE_CURRENT_LIST_DIR}/tinycc/libtcc.a ${CMAKE_CURRENT_LIST_DIR}/tinycc/libtcc1.a ${CMAKE_CURRENT_LIST_DIR}/tinycc/libtcc.h 
    INSTALL_COMMAND ""
    TEST_COMMAND ""
)

ExternalProject_Add(LIBMUSL
    DOWNLOAD_COMMAND ""
    SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/musl
    CONFIGURE_COMMAND cd ${CMAKE_CURRENT_LIST_DIR}/musl && ./configure --enable-debug
    BUILD_COMMAND cd ${CMAKE_CURRENT_LIST_DIR}/musl && make
    BUILD_BYPRODUCTS ${CMAKE_CURRENT_LIST_DIR}/musl/lib/libc.a
    INSTALL_COMMAND ""
    TEST_COMMAND ""
)

function(romfs_copy_files TO output_directory FILES input_files RELATIVE relative_dir)
    foreach(file ${input_files})
        cmake_path(RELATIVE_PATH file BASE_DIRECTORY ${relative_dir} OUTPUT_VARIABLE fileRel)
        # set(fileRel "${output_directory}/${fileRel}")
        cmake_path(ABSOLUTE_PATH fileRel BASE_DIRECTORY ${output_directory} NORMALIZE OUTPUT_VARIABLE fileTo)
        if(NOT EXISTS "${fileTo}")
            configure_file(${file} "${fileTo}")
            message("[copying] ${file} -> ${fileTo}")
        endif()
    endforeach()
endfunction()

# Package nessicary libraries into the binary
invertBoolean("FIRST_BUILD" FIRST_BUILD)
if(${FIRST_BUILD}) # since inverted this is actually NOT(${FIRST_BUILD})
    file(GLOB_RECURSE MUSL_HEADERS "${CMAKE_CURRENT_LIST_DIR}/musl/include/*" LIST_DIRECTORIES false)
    romfs_copy_files(TO ${CMAKE_CURRENT_LIST_DIR}/romfs/musl FILES "${MUSL_HEADERS}" RELATIVE ${CMAKE_CURRENT_LIST_DIR}/musl)
    file(GLOB_RECURSE MUSL_PLATFORM_HEADERS "${CMAKE_CURRENT_LIST_DIR}/musl/arch/x86_64/*" LIST_DIRECTORIES false) # TODO: it would be preferable if we dynamically picked the platform instead of assumign x86_64!
    romfs_copy_files(TO ${CMAKE_CURRENT_LIST_DIR}/romfs/musl/include FILES "${MUSL_PLATFORM_HEADERS}" RELATIVE ${CMAKE_CURRENT_LIST_DIR}/musl/arch/x86_64/)
    file(GLOB_RECURSE MUSL_GENERATED_HEADERS "${CMAKE_CURRENT_LIST_DIR}/musl/obj/include/*" LIST_DIRECTORIES false)
    romfs_copy_files(TO ${CMAKE_CURRENT_LIST_DIR}/romfs/musl/include FILES "${MUSL_GENERATED_HEADERS}" RELATIVE ${CMAKE_CURRENT_LIST_DIR}/musl/obj/include/)
    configure_file(${CMAKE_CURRENT_LIST_DIR}/tinycc/libtcc.h ${CMAKE_CURRENT_LIST_DIR}/romfs/libtcc.h COPYONLY)
    configure_file(${CMAKE_CURRENT_LIST_DIR}/tinycc/libtcc1.a ${CMAKE_CURRENT_LIST_DIR}/romfs/libtcc1.a COPYONLY)
    configure_file(${CMAKE_CURRENT_LIST_DIR}/musl/lib/libc.a ${CMAKE_CURRENT_LIST_DIR}/romfs/musl_libc.a COPYONLY)
else()
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/romfs)
endif()
set(LIBROMFS_PROJECT_NAME ${PROJECT_NAME})
set(LIBROMFS_RESOURCE_LOCATION "${CMAKE_CURRENT_LIST_DIR}/romfs")
add_subdirectory(libromfs)


# Wrapper library to make it all work :D
add_library(${PROJECT_NAME} STATIC empty.cpp)
add_dependencies(${PROJECT_NAME} TINYCC LIBMUSL)
target_link_libraries(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/tinycc/libtcc.a ${LIBROMFS_LIBRARY})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/tinycc ${CMAKE_CURRENT_BINARY_DIR})
