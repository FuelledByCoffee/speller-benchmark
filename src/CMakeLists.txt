configure_file(${CMAKE_CURRENT_SOURCE_DIR}/version.hpp.in ${CMAKE_BINARY_DIR}/version.hpp @ONLY)

add_executable(bench main.cpp benchmark.cpp)
target_include_directories(bench
   PRIVATE ${PROJECT_BINARY_DIR}
   PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

find_package(fmt CONFIG REQUIRED)
target_link_libraries(bench PRIVATE fmt::fmt)

set_target_properties(bench
   PROPERTIES
   RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

target_compile_features(bench 
PRIVATE 
cxx_std_20
cxx_decltype
cxx_trailing_return_types)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-march=native" COMPILER_SUPPORTS_MARCH_NATIVE)
if(COMPILER_SUPPORTS_MARCH_NATIVE)
   option(OPTIMIZE_FOR_NATIVE "Build with -march=native" ON)
   if(OPTIMIZE_FOR_NATIVE)
      target_compile_options(bench PRIVATE -march=native)
   endif()
endif()
