#pragma once
#include <cstdint>

namespace aulara {

enum class Material : std::uint8_t {
    Air = 0,
    Sand = 1,
    Stone = 2
};

struct Cell {
    Material material = Material::Air;
    std::uint8_t flags = 0;
    std::uint8_t durability = 0;
    std::uint8_t shade = 0;
};
static_assert(sizeof(Cell) == 4, "Keep Cell at 4 bytes");

} // namespace aulara