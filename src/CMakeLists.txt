configure_file(${CMAKE_CURRENT_SOURCE_DIR}/version.hpp.in ${CMAKE_BINARY_DIR}/version.hpp @ONLY)

add_executable(bench bench.cpp record.cpp)
target_include_directories(bench
   PRIVATE ${PROJECT_BINARY_DIR}
   PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(bench PRIVATE pthread)

find_package(fmt CONFIG REQUIRED)
target_link_libraries(bench PRIVATE fmt::fmt)

find_package(range-v3 CONFIG REQUIRED)
target_link_libraries(bench PRIVATE range-v3::meta range-v3::concepts range-v3::range-v3)

find_package(Catch2 CONFIG REQUIRED)
target_link_libraries(bench PRIVATE Catch2::Catch2)
include(Catch)

# catch_discover_tests(bench)
