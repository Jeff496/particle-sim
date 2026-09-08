# Dev-only harness: SDL3 window + Dear ImGui + the sim. Never shipped.
include(FetchContent)

# Trim SDL to what a 2D dev tool needs (video, events, renderer). Cuts build time a lot.
set(SDL_SHARED       OFF CACHE BOOL "" FORCE)  # static: no .dylib to find at runtime
set(SDL_STATIC       ON  CACHE BOOL "" FORCE)
set(SDL_TEST_LIBRARY OFF CACHE BOOL "" FORCE)
set(SDL_TESTS        OFF CACHE BOOL "" FORCE)
set(SDL_EXAMPLES     OFF CACHE BOOL "" FORCE)
foreach(sub AUDIO CAMERA DIALOG GPU HAPTIC HIDAPI JOYSTICK POWER SENSOR)
  set(SDL_${sub} OFF CACHE BOOL "" FORCE)
endforeach()

FetchContent_Declare(SDL3
  URL      https://github.com/libsdl-org/SDL/releases/download/release-3.4.16/SDL3-3.4.16.tar.gz
  URL_HASH SHA256=7322236cd12090c3eb40b9728be4d49c76f66ad17d04369584d4ecad5cf77c68
  SYSTEM EXCLUDE_FROM_ALL)
FetchContent_Declare(imgui
  URL      https://github.com/ocornut/imgui/archive/refs/tags/v1.92.9b.tar.gz
  URL_HASH SHA256=21d8a0a565e85dce943e375db00812c2f3f0ab21f3f0f7964e364a63422d7f99)
FetchContent_MakeAvailable(SDL3 imgui)

# Dear ImGui has no CMake project of its own: build the core + the two backends we use.
add_library(dear_imgui STATIC
  ${imgui_SOURCE_DIR}/imgui.cpp
  ${imgui_SOURCE_DIR}/imgui_draw.cpp
  ${imgui_SOURCE_DIR}/imgui_tables.cpp
  ${imgui_SOURCE_DIR}/imgui_widgets.cpp
  ${imgui_SOURCE_DIR}/imgui_demo.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer3.cpp
)
target_include_directories(dear_imgui SYSTEM PUBLIC ${imgui_SOURCE_DIR} ${imgui_SOURCE_DIR}/backends)
target_link_libraries(dear_imgui PUBLIC SDL3::SDL3)

add_executable(playground main.cpp)
target_link_libraries(playground PRIVATE aulara_sim SDL3::SDL3 dear_imgui)
