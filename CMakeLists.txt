cmake_minimum_required(VERSION 3.21.0)

project(speller-benchmark VERSION 2.0 LANGUAGES C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

include(CTest)

# set(CPACK_PROJECT_NAME ${PROJECT_NAME})
# set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
# include(CPack)

include(cmake/Warnings.cmake)
enable_warnings()
include(cmake/Sanitizers.cmake)
enable_sanitizers()
configure_file(version.h.in version.h)

add_executable(bench bench.c)
target_include_directories(bench PRIVATE ${PROJECT_BINARY_DIR})

install(TARGETS bench DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/..)
