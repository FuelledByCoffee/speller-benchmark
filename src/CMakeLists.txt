configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/version.hpp.in
    ${CMAKE_BINARY_DIR}/version.hpp
    @ONLY
)

add_executable(bench main.cpp benchmark.cpp results.cpp)
target_include_directories(
    bench
    PRIVATE ${PROJECT_BINARY_DIR}
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
)
find_package(boost_process CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
target_link_libraries(bench PRIVATE Boost::process fmt::fmt)

set_target_properties(
    bench
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

target_compile_features(
    bench
    PRIVATE cxx_std_20 cxx_decltype cxx_trailing_return_types
)

include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-march=native" COMPILER_SUPPORTS_MARCH_NATIVE)
check_cxx_compiler_flag("-flto" COMPILER_SUPPORTS_LTO)

if(COMPILER_SUPPORTS_MARCH_NATIVE)
    option(MARCH_NATIVE "Build with -march=native" OFF)
    if(MARCH_NATIVE)
        target_compile_options(bench PRIVATE "-march=native")
    endif()
endif()

if(COMPILER_SUPPORTS_LTO)
    option(LINK_TIME_OPTIMIZATION "Build with -flto" OFF)
    if(LINK_TIME_OPTIMIZATION)
        target_compile_options(bench PRIVATE "$<$<CONFIG:RELEASE>:-flto>")
        target_link_options(bench PRIVATE "$<$<CONFIG:RELEASE>:-flto>")
    endif()
endif()
