include(FetchContent)
FetchContent_Declare(
   vcpkg
   GIT_REPOSITORY https://github.com/microsoft/vcpkg
   GIT_TAG 501db0f17ef6df184fcdbfbe0f87cde2313b6ab1 # 2023.04.15
)

FetchContent_MakeAvailable(vcpkg)

set(CMAKE_TOOLCHAIN_FILE "${vcpkg_SOURCE_DIR}/scripts/buildsystems/vcpkg.cmake")
# set(VCPKG_CHAINLOAD_TOOLCHAIN_FILE "${CMAKE_CURRENT_LIST_DIR}/toolchain.cmake")
