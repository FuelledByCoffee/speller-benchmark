cmake_minimum_required(VERSION 3.30)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(vcpkg) # Must set toolchain file before calling project()
project(speller-benchmark VERSION 2.0 LANGUAGES CXX)

set(CMAKE_CXX_VISIBILITY_PRESET hidden)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

include(BuildType)
include(Warnings)
include(Sanitizers)
set_default_buildtype()
enable_warnings()
enable_sanitizers()

add_subdirectory(src)

get_filename_component(PARENT_DIR ../ ABSOLUTE)
set(SPELLER_LOCATION
    "${PARENT_DIR}"
    CACHE STRING
    "The location of your cs50 project directory"
)

install(TARGETS bench CONFIGURATIONS Release)

include(CTest)
add_test(
    NAME single_thread
    COMMAND bench -1y
    WORKING_DIRECTORY ${SPELLER_LOCATION}
)
add_test(
    NAME multi_thread
    COMMAND bench -y
    WORKING_DIRECTORY ${SPELLER_LOCATION}
)
add_test(
    NAME single_thread_with_staff
    COMMAND bench -1s
    WORKING_DIRECTORY ${SPELLER_LOCATION}
)
add_test(
    NAME multi_thread_with_staff
    COMMAND bench -s
    WORKING_DIRECTORY ${SPELLER_LOCATION}
)
