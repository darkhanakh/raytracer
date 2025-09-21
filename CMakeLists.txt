cmake_minimum_required(VERSION 3.31)
project(raytracer C)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

# Prefer system-installed SDL2; fall back to vendored if not found
set(SDL2_FROM_SYSTEM ON CACHE BOOL "Use system-installed SDL2 if available; otherwise fallback to vendored")
if(SDL2_FROM_SYSTEM)
find_package(SDL2 QUIET CONFIG)
if(NOT SDL2_FOUND)
    find_package(SDL2 QUIET)
endif()
endif()

if(NOT SDL2_FOUND)
    add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)
endif()

set(CMAKE_C_STANDARD 23)

add_executable(raytracer main.c)

# Link against SDL2 (system or vendored)
if(TARGET SDL2::SDL2)
    target_link_libraries(raytracer PRIVATE SDL2::SDL2)
elseif(TARGET SDL2::SDL2-static)
    target_link_libraries(raytracer PRIVATE SDL2::SDL2-static)
elseif(DEFINED SDL2_LIBRARIES)
    target_link_libraries(raytracer PRIVATE ${SDL2_LIBRARIES})
    if(DEFINED SDL2_INCLUDE_DIRS)
        target_include_directories(raytracer PRIVATE ${SDL2_INCLUDE_DIRS})
    endif()
endif()
