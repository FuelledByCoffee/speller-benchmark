configure_file(${CMAKE_CURRENT_SOURCE_DIR}/version.hpp.in ${CMAKE_BINARY_DIR}/version.hpp @ONLY)

add_executable(bench main.cpp benchmark.cpp)
target_include_directories(bench
   PRIVATE ${PROJECT_BINARY_DIR}
   PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(bench PRIVATE pthread)

find_package(fmt CONFIG REQUIRED)
target_link_libraries(bench PRIVATE fmt::fmt)
