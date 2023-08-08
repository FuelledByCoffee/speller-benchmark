cmake_minimum_required(VERSION 3.21.0)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(vcpkg) # Must set toolchain file before calling project()

project(speller-benchmark VERSION 2.0 LANGUAGES CXX)

set(CMAKE_CXX_VISIBILITY_PRESET hidden)

# set(CPACK_PROJECT_NAME ${PROJECT_NAME})
# set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
# include(CPack)
include(BuildType)
include(Warnings)
include(Sanitizers)
set_default_buildtype()
enable_warnings()
enable_sanitizers()

add_subdirectory(src)

get_filename_component(PARENT_DIR ../ ABSOLUTE)
set(SPELLER_LOCATION "${PARENT_DIR}" CACHE STRING "The location of your cs50 project directory")
set(SPELLER50_BINARY "${SPELLER_LOCATION}/speller50" CACHE STRING "The location of the staff implementation of speller")

option(COMPARE_SPELLER50 "Compare your results against speller50" ON)

if(COMPARE_SPELLER50)
	execute_process(COMMAND ${SPELLER50_BINARY} "texts/cat.txt"
		WORKING_DIRECTORY ${SPELLER_LOCATION}
		OUTPUT_QUIET
		TIMEOUT 20
		RESULT_VARIABLE SPELLER50_RESULT)

	if(SPELLER50_RESULT EQUAL 0)
		message(STATUS "speller50 executed successfully")
		target_compile_definitions(bench PRIVATE COMPARE_STAFF)
	else()
		message(WARNING "speller50 exited with the result: ${SPELLER50_RESULT}, disabling COMPARE_SPELLER50")
		set(COMPARE_SPELLER50 OFF CACHE BOOL "Can't run speller50" FORCE)
	endif()
endif()

install(TARGETS bench DESTINATION ${SPELLER_LOCATION})

include(CTest)
add_test(NAME single_thread
	COMMAND bench -1
	WORKING_DIRECTORY ${SPELLER_LOCATION})
add_test(NAME multi_thread
	COMMAND bench
	WORKING_DIRECTORY ${SPELLER_LOCATION})

if(COMPARE_SPELLER50)
	add_test(NAME single_thread_with_staff
		COMMAND bench -1t
		WORKING_DIRECTORY ${SPELLER_LOCATION})
	add_test(NAME multi_thread_with_staff
		COMMAND bench -t
		WORKING_DIRECTORY ${SPELLER_LOCATION})
endif()
