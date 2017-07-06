# This file is derived from the template at
# http://llvm.org/docs/CMake.html#embedding-llvm-in-your-project

cmake_minimum_required(VERSION 3.7)
project(lang)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

set(CMAKE_CXX_STANDARD 11)
add_compile_options(-Wall -Werror -pedantic)
set(CMAKE_CXX_FLAGS "-Wall -Werror -pedantic")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -O3")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

set(SOURCE_FILES
    src/ast.cpp
    src/codegen.cpp
    src/lex.cpp
    src/main.cpp
    src/parse.cpp
    src/token.cpp
    src/util.cpp)

add_executable(c ${SOURCE_FILES})

llvm_map_components_to_libnames(llvm_libs core)
target_link_libraries(c ${llvm_libs})