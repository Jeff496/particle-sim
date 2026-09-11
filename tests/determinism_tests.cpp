// // Determinism and invariant tests.
// //
// // Two different jobs here, and they fail for different reasons:
// //
// //   * The GOLDEN tests pin the exact output. They fail on ANY behaviour change,
// //     intentional or not. During a refactor that is exactly what you want.
// //   * The INVARIANT tests encode rules that must hold whatever the implementation
// //     is. They survive intentional behaviour changes, so they stay useful forever,
// //     and they name the bug instead of just reporting a hash mismatch.

// #include <doctest/doctest.h>

// #include <cstdint>
// #include <cstdio>
// #include <vector>

// #include "determinism_support.h"

// using namespace aulara;
// using namespace aulara::test;

// namespace {

// // 16x32, 16 frames. Small enough that the per-frame hashes are worth storing
// // individually: a mismatch tells you the exact frame on which behaviour diverged,
// // which is a far better starting point than "the run differs".
// //
// // The grid is deliberately taller than it is wide: at 35% fill the pile needs about
// // 20 frames to settle, so all 16 recorded frames are still in motion. A scene that
// // settles early turns the trailing hashes into copies of each other and quietly
// // stops testing anything.
// constexpr int kSmallW = 16, kSmallH = 32, kSmallFrames = 16;
// constexpr std::uint64_t kSmallSeed = 0xA0DA5EEDULL;

// const std::uint64_t kGoldenSmall[kSmallFrames] = {
//     0xC5C46EACD127214DULL,
//     0x0010674FC197E161ULL,
//     0xE73E02A782722C91ULL,
//     0xAD334C76321152BDULL,
//     0x3619FFD1E9705649ULL,
//     0xC4205095560D835DULL,
//     0x4DD9E90B5C450AF5ULL,
//     0x1FB784760DA50BB5ULL,
//     0x6AD8ABD5B5D7F3C1ULL,
//     0x1918FDF91945D889ULL,
//     0x2D596E5AFD5444F5ULL,
//     0x1F4CDF53887063FDULL,
//     0xB2064462750F89D9ULL,
//     0x499B47C433180F09ULL,
//     0x2D9F8E20490C21EDULL,
//     0x365058A310D38079ULL,
// };

// // 64x64, 200 frames. Broad coverage: settling piles, diagonal fallback, stone
// // blocking, both parities of the alternating scan direction. One rolling hash.
// constexpr int kBigW = 64, kBigH = 64, kBigFrames = 200;
// constexpr std::uint64_t kBigSeed = 0xBADCAFEULL;
// constexpr std::uint64_t kGoldenBig = 0x1CA9533E61241B51ULL;

// std::uint64_t run_rolling(int w_, int h_, std::uint64_t seed, int frames) {
//     World w = make_scene(w_, h_, seed);
//     std::uint64_t rolling = fingerprint(w);
//     for (int i = 0; i < frames; ++i) {
//         w.step();
//         const std::uint64_t f = fingerprint(w);
//         for (int b = 0; b < 8; ++b) {
//             rolling = fnv1a(rolling, static_cast<std::uint8_t>(f >> (b * 8)));
//         }
//     }
//     return rolling;
// }

// } // namespace

// // ---------------------------------------------------------------------------
// // Golden tests
// // ---------------------------------------------------------------------------

// TEST_CASE("golden: 16x32 matches frame by frame") {
//     World w = make_scene(kSmallW, kSmallH, kSmallSeed);
//     for (int i = 0; i < kSmallFrames; ++i) {
//         w.step();
//         const std::uint64_t got = fingerprint(w);
//         INFO("diverged on frame ", i + 1);
//         REQUIRE(got == kGoldenSmall[i]);
//     }
// }

// TEST_CASE("golden: 64x64 over 200 frames") {
//     CHECK(run_rolling(kBigW, kBigH, kBigSeed, kBigFrames) == kGoldenBig);
// }

// TEST_CASE("golden: a run is reproducible within one process") {
//     // Catches uninitialised memory and any accidental dependence on address,
//     // clock, or global state. ASan/MSan find some of these; this finds the rest.
//     CHECK(run_rolling(kBigW, kBigH, kBigSeed, 50) ==
//           run_rolling(kBigW, kBigH, kBigSeed, 50));
// }

// // ---------------------------------------------------------------------------
// // Invariants
// // ---------------------------------------------------------------------------

// TEST_CASE("invariant: sand is neither created nor destroyed") {
//     World w = make_scene(kBigW, kBigH, 0x5A17ULL);

//     auto count_sand = [&] {
//         int n = 0;
//         for (int y = 0; y < w.height(); ++y)
//             for (int x = 0; x < w.width(); ++x)
//                 if (w.get_cell(x, y).material == Material::Sand) ++n;
//         return n;
//     };

//     const int expected = count_sand();
//     for (int i = 0; i < 120; ++i) {
//         w.step();
//         INFO("frame ", i + 1);
//         REQUIRE(count_sand() == expected);
//     }
// }

// TEST_CASE("invariant: stone never moves") {
//     World w = make_scene(kBigW, kBigH, 0x570E5ULL);
//     const std::vector<Material> before = snapshot(w);

//     for (int i = 0; i < 120; ++i) w.step();
//     const std::vector<Material> after = snapshot(w);

//     for (std::size_t i = 0; i < before.size(); ++i) {
//         if (before[i] == Material::Stone) {
//             INFO("stone at linear index ", i, " moved");
//             REQUIRE(after[i] == Material::Stone);
//         }
//     }
// }

// TEST_CASE("invariant: sand only ever arrives from the row above") {
//     // This is the one that catches a row-edge wraparound, and it is the reason the
//     // whole file exists.
//     //
//     // The scan runs bottom-up, so a grain moves at most once per frame, and always
//     // into the row below it. Therefore:
//     //
//     //   * if a cell went Air -> Sand, one of the three cells above it must have held
//     //     sand BEFORE the step -- that is where the grain came from;
//     //   * if a cell went Sand -> Air, one of the three cells below it must hold sand
//     //     AFTER the step -- that is where the grain went.
//     //
//     // A swap that wraps off column 0 lands the grain at (width-1, y): same row, half
//     // a grid away. Neither clause can be satisfied, so it trips on the frame it
//     // happens -- even though the write landed inside the vector and was therefore
//     // completely invisible to ASan.
//     //
//     // Note what this deliberately does NOT assert: that a cell changed at all. A
//     // column of sand falling one row leaves every interior cell reading Sand both
//     // before and after. Only the ends of the column change, so any invariant phrased
//     // as "changed cells come in adjacent pairs" fires on ordinary falling sand.
//     World w = make_scene(kBigW, kBigH, 0x10CA1ULL);

//     for (int frame = 0; frame < 120; ++frame) {
//         const std::vector<Material> before = snapshot(w);
//         w.step();
//         const std::vector<Material> after = snapshot(w);

//         auto at = [&](const std::vector<Material> &g, int x, int y) {
//             return g[static_cast<std::size_t>(y) * w.width() + x];
//         };
//         auto sand_in_row = [&](const std::vector<Material> &g, int y, int cx) {
//             if (y < 0 || y >= w.height()) return false;
//             for (int x = cx - 1; x <= cx + 1; ++x) {
//                 if (x < 0 || x >= w.width()) continue;
//                 if (at(g, x, y) == Material::Sand) return true;
//             }
//             return false;
//         };

//         for (int y = 0; y < w.height(); ++y) {
//             for (int x = 0; x < w.width(); ++x) {
//                 const Material b = at(before, x, y), a = at(after, x, y);

//                 if (b == Material::Air && a == Material::Sand) {
//                     INFO("frame ", frame + 1, ": sand appeared at (", x, ",", y,
//                          ") with no source in the row above");
//                     REQUIRE(sand_in_row(before, y - 1, x));
//                 }
//                 if (b == Material::Sand && a == Material::Air) {
//                     INFO("frame ", frame + 1, ": sand vanished from (", x, ",", y,
//                          ") with no destination in the row below");
//                     REQUIRE(sand_in_row(after, y + 1, x));
//                 }
//             }
//         }
//     }
// }

// TEST_CASE("invariant: sand never rises") {
//     // Sum of y over all sand cells is non-decreasing: sand falls or stays put.
//     World w = make_scene(kBigW, kBigH, 0xFA11ULL);

//     auto height_sum = [&] {
//         long long s = 0;
//         for (int y = 0; y < w.height(); ++y)
//             for (int x = 0; x < w.width(); ++x)
//                 if (w.get_cell(x, y).material == Material::Sand) s += y;
//         return s;
//     };

//     long long prev = height_sum();
//     for (int i = 0; i < 120; ++i) {
//         w.step();
//         const long long now = height_sum();
//         INFO("frame ", i + 1, ": sand moved upward");
//         REQUIRE(now >= prev);
//         prev = now;
//     }
// }

// // ---------------------------------------------------------------------------
// // Regenerating the golden values
// // ---------------------------------------------------------------------------
// //
// // Run ONLY after an intentional behaviour change, and only from a build you have
// // already checked against the invariants above:
// //
// //     ./build/asan/tests/sim_tests -ts=golden-gen --no-skip
// //
// // Paste the output over the constants at the top of this file, and say in the
// // commit message what behaviour changed and why.

// TEST_SUITE("golden-gen") {
//     TEST_CASE("regenerate" * doctest::skip()) {
//         World w = make_scene(kSmallW, kSmallH, kSmallSeed);
//         std::printf("\nconst std::uint64_t kGoldenSmall[kSmallFrames] = {\n");
//         for (int i = 0; i < kSmallFrames; ++i) {
//             w.step();
//             std::printf("    0x%016llXULL,\n",
//                         static_cast<unsigned long long>(fingerprint(w)));
//         }
//         std::printf("};\n");
//         std::printf("constexpr std::uint64_t kGoldenBig = 0x%016llXULL;\n\n",
//                     static_cast<unsigned long long>(
//                         run_rolling(kBigW, kBigH, kBigSeed, kBigFrames)));
//     }
// }
