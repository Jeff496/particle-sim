cmake_minimum_required(VERSION 3.28)
project(aulara_particle_sim LANGUAGES CXX)

# Language + tooling. C++17 matches godot-cpp, so the sim compiles unchanged inside the extension.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)    # build/compile_commands.json for clangd
set(CMAKE_POSITION_INDEPENDENT_CODE ON)  # lets the static sim be linked into a shared library

option(AULARA_PROFILE          "Instrument the sim with Tracy (dev only)" OFF)
option(AULARA_BUILD_PLAYGROUND "Build the SDL3 playground"                ON)
option(AULARA_BUILD_TESTS      "Build the test suite"                     ON)

include(FetchContent)

if(AULARA_PROFILE)
  set(TRACY_ENABLE    ON CACHE BOOL "" FORCE)
  set(TRACY_ON_DEMAND ON CACHE BOOL "" FORCE)   # only record while the profiler GUI is connected
  FetchContent_Declare(tracy
    URL      https://github.com/wolfpld/tracy/archive/refs/tags/v0.14.1.tar.gz
    URL_HASH SHA256=bf4af567e9c7524d07f3caa745fad02fb33bd5694f11910750382d1efbb251c1
    SYSTEM EXCLUDE_FROM_ALL)
  FetchContent_MakeAvailable(tracy)
endif()

add_subdirectory(sim)

if(AULARA_BUILD_PLAYGROUND)
  add_subdirectory(playground)
endif()

if(AULARA_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()
