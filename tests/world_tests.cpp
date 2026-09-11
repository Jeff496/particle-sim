#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <aulara/sim/world.h>

using namespace aulara;

TEST_CASE("set_cell and get_cell round-trip") {
	World w(8, 8);
	w.set_cell(3, 3, id(Material::Stone));
	CHECK(w.get_cell(3, 3).material == id(Material::Stone));
	CHECK(w.get_cell(0, 0).material == id(Material::Air));
}


TEST_CASE("a sand cell falls one row per step") {
	World w(4, 4);
	w.set_cell(1, 0, id(Material::Sand)); // y = 0 is the top row
	w.step();
	CHECK(w.get_cell(1, 0).material == id(Material::Air));
	CHECK(w.get_cell(1, 1).material == id(Material::Sand));
}
