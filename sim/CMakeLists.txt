# The simulation library. Pure C++: no SDL, no Godot, no Tracy unless AULARA_PROFILE is on.
add_library(aulara_sim STATIC
  src/world.cpp
  src/material.cpp
  src/rules.cpp
)
target_include_directories(aulara_sim PUBLIC include)
target_compile_options(aulara_sim PRIVATE
  $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall;-Wextra>
  $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

if(AULARA_PROFILE)
  # PUBLIC so anything linking the sim (the playground) can also use the profiler macros.
  target_link_libraries(aulara_sim PUBLIC Tracy::TracyClient)
  target_compile_definitions(aulara_sim PUBLIC AULARA_PROFILE)
endif()
