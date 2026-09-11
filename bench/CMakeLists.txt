# Fixed-scenario benchmark. Links only the sim: no SDL, no ImGui, no doctest.
#
# Deliberately NOT registered with add_test(). Tests answer pass/fail and must
# stay fast; bench answers "how many milliseconds" and is slow. Mixing them
# makes ctest take minutes and lets a busy machine "fail" the suite.
add_executable(bench bench.cpp)
target_link_libraries(bench PRIVATE aulara_sim)
target_compile_options(bench PRIVATE
  $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall;-Wextra>
  $<$<CXX_COMPILER_ID:MSVC>:/W4>
)
