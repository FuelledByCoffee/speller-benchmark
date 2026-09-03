cmake_minimum_required(VERSION 4.1)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(vcpkg) # Must set toolchain file before calling project()
project(speller-benchmark VERSION 2.1 LANGUAGES CXX)

set(CMAKE_CXX_VISIBILITY_PRESET hidden)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

include(GNUInstallDirs)
include(BuildType)
include(Warnings)
include(Sanitizers)
include(Coverage)
set_default_buildtype()
enable_warnings()
enable_sanitizers()
enable_coverage()

add_subdirectory(src)

find_path(
    SPELLER_LOCATION
    NAMES speller
    PATHS
        "${CMAKE_SOURCE_DIR}/.."
        "${CMAKE_SOURCE_DIR}/../speller"
        "${CMAKE_SOURCE_DIR}/../cs50/speller"
        "${CMAKE_SOURCE_DIR}/../cs50/week5/speller"
    NO_DEFAULT_PATH
    DOC "The location of your speller project directory"
)

file(
    ARCHIVE_CREATE
    OUTPUT "${CMAKE_BINARY_DIR}/bench.1.gz"
    PATHS "${CMAKE_SOURCE_DIR}/bench.1"
    FORMAT raw
    COMPRESSION GZip
)

install(TARGETS bench CONFIGURATIONS Release)
install(
    FILES "${CMAKE_BINARY_DIR}/bench.1.gz"
    DESTINATION "${CMAKE_INSTALL_MANDIR}/man1"
)

include(CTest)
add_test(
    NAME single_thread
    COMMAND bench -1 --no-staff
    WORKING_DIRECTORY ${SPELLER_LOCATION}
)
add_test(NAME multi_thread COMMAND bench WORKING_DIRECTORY ${SPELLER_LOCATION})
add_test(
    NAME single_thread_with_staff
    COMMAND bench -1
    WORKING_DIRECTORY ${SPELLER_LOCATION}
)
add_test(
    NAME multi_thread_with_staff
    COMMAND bench
    WORKING_DIRECTORY ${SPELLER_LOCATION}
)
add_test(NAME invalid_flag COMMAND bench -z)

set_property(TEST invalid_flag PROPERTY WILL_FAIL TRUE)
