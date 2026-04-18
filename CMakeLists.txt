cmake_minimum_required(VERSION 3.15...4.3)
project(c-do-p)

# set the output directory for built objects.
# This makes sure that the dynamic library goes into the build directory automatically.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

# This assumes the SDL source is available in vendored/SDL
add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)
add_subdirectory(vendored/tracy EXCLUDE_FROM_ALL)

add_executable(c-do-p WIN32 src/main.cpp)

target_link_libraries(c-do-p 
    PRIVATE 
    SDL3::SDL3
    Tracy::TracyClient
)


