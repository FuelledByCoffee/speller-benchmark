cmake_minimum_required(VERSION 3.21.0)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

project(speller-benchmark VERSION 2.0 LANGUAGES C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

# set(CPACK_PROJECT_NAME ${PROJECT_NAME})
# set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
# include(CPack)
include(BuildType)
include(Warnings)
include(Sanitizers)
set_default_buildtype()
enable_warnings()
enable_sanitizers()

configure_file(version.h.in version.h)

get_filename_component(PARENT_DIR ../ ABSOLUTE)
set(SPELLER_LOCATION "${PARENT_DIR}" CACHE STRING "The location of your cs50 project directory")
set(SPELLER50_LOCATION "${SPELLER_LOCATION}/speller50" CACHE STRING "The location of the staff implementation of speller")

add_executable(bench bench.c)
target_include_directories(bench PRIVATE ${PROJECT_BINARY_DIR})

option(COMPARE_SPELLER50 "Compare your results against speller50" ON)

if(COMPARE_SPELLER50)
	execute_process(COMMAND ${SPELLER50_LOCATION} "texts/cat.txt"
		WORKING_DIRECTORY ${SPELLER_LOCATION}
		OUTPUT_QUIET
		TIMEOUT 20
		RESULT_VARIABLE SPELLER50_RESULT)

	if(SPELLER50_RESULT EQUAL 0)
		message(STATUS "speller50 exected successfully")
		target_compile_definitions(bench PRIVATE COMPARE_STAFF)
	else()
		message(WARNING "speller50 exited with the result: ${SPELLER50_RESULT}, disabling COMPARE_SPELLER50")
		set(COMPARE_SPELLER50 OFF CACHE BOOL "Can't run speller50" FORCE)
	endif()
endif()

install(TARGETS bench DESTINATION ${SPELLER_LOCATION})

enable_testing()

add_test(NAME run_benchmark
	COMMAND bench
	WORKING_DIRECTORY ${SPELLER_LOCATION})
set_tests_properties(run_benchmark
	PROPERTIES
	REQUIRED_FILES ${SPELLER_LOCATION}/speller)

add_test(NAME wrong_directory
	COMMAND bench)
set_tests_properties(wrong_directory
	PROPERTIES
	WILL_FAIL TRUE)
