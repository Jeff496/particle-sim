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

// // 128x128 = 2x2 chunks. A stone floor with a full-width sand layer on it: nothing can move.
// static void build_settled(World &w) {
// 	for (int x = 0; x < w.width(); ++x) {
// 		w.set_cell(x, 100, id(Material::Stone));
// 	}
// 	for (int y = 80; y < 100; ++y) {
// 		for (int x = 0; x < w.width(); ++x) {
// 			w.set_cell(x, y, id(Material::Sand));
// 		}
// 	}
// }

// TEST_CASE("settled sand puts every chunk to sleep after 2 steps") {
// 	World w(128, 128);
// 	build_settled(w);
// 	w.step();
// 	CHECK(w.chunks_scanned_last_step() == 4);
// 	w.step();
// 	CHECK(w.chunks_scanned_last_step() == 0);
// }

// TEST_CASE("removing the floor under sleeping sand makes it fall") {
// 	// run once with the hole landing on an odd frame and once on an even one: sleeping cells keep the
// 	// clock stamp from the last step they were evaluated, and one of the two parities matches it
// 	for (int extra_idle_steps : {0, 1}) {
// 		CAPTURE(extra_idle_steps);
// 		World w(128, 128);
// 		build_settled(w);
// 		for (int i = 0; i < 2 + extra_idle_steps; ++i) w.step();
// 		REQUIRE(w.chunks_scanned_last_step() == 0);

// 		w.set_cell(64, 100, id(Material::Air)); // hole in the floor, on a chunk border
// 		for (int i = 0; i < 6; ++i) w.step();

// 		int below_floor = 0;
// 		for (int y = 101; y < w.height(); ++y) {
// 			if (w.get_cell(64, y).material == id(Material::Sand)) ++below_floor;
// 		}
// 		CHECK(below_floor > 0);
// 	}
// }
