// #pragma once
// // Shared helpers for the determinism / invariant tests.
// //
// // The fingerprint here is the contract a refactor must not break. It lives in the
// // test tree on purpose: it hashes the world through the public API only, so it
// // keeps working no matter how the storage or the index arithmetic is reorganised.

// #include <cstdint>
// #include <vector>

// #include <aulara/sim/world.h>

// namespace aulara::test {

// inline std::uint64_t fnv1a(std::uint64_t h, std::uint8_t byte) {
//     h ^= byte;
//     h *= 0x100000001B3ULL;
//     return h;
// }

// // Hashes every observable byte of every cell, in a fixed row-major order.
// // Row-major order is the test's choice, not the sim's: if a refactor switches to
// // column-major or tiled storage, this still produces the same number.
// inline std::uint64_t fingerprint(const World &w) {
//     std::uint64_t h = 0xCBF29CE484222325ULL;
//     for (int y = 0; y < w.height(); ++y) {
//         for (int x = 0; x < w.width(); ++x) {
//             const Cell &c = w.get_cell(x, y);
//             h = fnv1a(h, static_cast<std::uint8_t>(c.material));
//             h = fnv1a(h, c.flags);
//             h = fnv1a(h, c.durability);
//             h = fnv1a(h, c.shade);
//         }
//     }
//     return h;
// }

// // A scene built from its *own* RNG, never the World's. If a refactor changes when
// // or how often the sim draws from rng_state_, the starting scene must not move with
// // it -- otherwise the golden hashes would shift for a reason that has nothing to do
// // with the bug you are hunting.
// inline World make_scene(int width, int height, std::uint64_t seed) {
//     World w(width, height, seed);

//     std::uint64_t s = seed * 6364136223846793005ULL + 1442695040888963407ULL;
//     auto next = [&s]() {
//         s ^= s << 13;
//         s ^= s >> 7;
//         s ^= s << 17;
//         return s;
//     };

//     for (int y = 0; y < height; ++y) {
//         for (int x = 0; x < width; ++x) {
//             const std::uint64_t r = (next() >> 11) % 100;
//             if (r < 35) {
//                 w.set_cell(x, y, Material::Sand);
//             } else if (r < 42) {
//                 w.set_cell(x, y, Material::Stone);
//             }
//         }
//     }
//     return w;
// }

// inline std::vector<Material> snapshot(const World &w) {
//     std::vector<Material> out;
//     out.reserve(static_cast<std::size_t>(w.width()) * w.height());
//     for (int y = 0; y < w.height(); ++y) {
//         for (int x = 0; x < w.width(); ++x) {
//             out.push_back(w.get_cell(x, y).material);
//         }
//     }
//     return out;
// }

// } // namespace aulara::test
