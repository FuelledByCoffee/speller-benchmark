cmake_minimum_required(VERSION 3.21.0)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

project(speller-benchmark VERSION 2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# set(CPACK_PROJECT_NAME ${PROJECT_NAME})
# set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
# include(CPack)
include(BuildType)
include(Warnings)
include(Sanitizers)
set_default_buildtype()
enable_warnings()
enable_sanitizers()

configure_file(version.hpp.in version.hpp)

add_executable(bench bench.cpp)
target_include_directories(bench PRIVATE ${PROJECT_BINARY_DIR})

install(TARGETS bench DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/..)

include(CTest)
add_test(NAME run-benchmark
	COMMAND bench
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/..)
