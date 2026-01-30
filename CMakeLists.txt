cmake_minimum_required(VERSION 3.10)
project(cmap VERSION 1.0.0)

set(CMAKE_C_STANDARD 99)

# Build the library
add_library(cmap STATIC src/map.c)
target_include_directories(cmap PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Build the example
add_executable(basic examples/basic.c)
target_link_libraries(basic cmap)

# Install the library
install(TARGETS cmap
    EXPORT cmap-targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

# Install headers
install(DIRECTORY include/ DESTINATION include)

# Install CMake config
install(EXPORT cmap-targets
    FILE cmap-config.cmake
    NAMESPACE cmap::
    DESTINATION lib/cmake/cmap
)

# Package config
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/cmap-config-version.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/cmap-config-version.cmake"
    DESTINATION lib/cmake/cmap
)
