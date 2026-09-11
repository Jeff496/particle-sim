include(FetchContent)
set(DOCTEST_WITH_TESTS OFF CACHE BOOL "" FORCE)  # do not build doctest own test suite
set(DOCTEST_NO_INSTALL ON  CACHE BOOL "" FORCE)
FetchContent_Declare(doctest
  URL      https://github.com/doctest/doctest/archive/refs/tags/v2.5.3.tar.gz
  URL_HASH SHA256=174ebc4e769928959614789c5b4e9c3d0a0f81a62bb608756b127bfebfb21331
  SYSTEM EXCLUDE_FROM_ALL)
FetchContent_MakeAvailable(doctest)

add_executable(sim_tests
  world_tests.cpp
  determinism_tests.cpp
)
target_link_libraries(sim_tests PRIVATE aulara_sim doctest::doctest)
add_test(NAME sim_tests COMMAND sim_tests)
