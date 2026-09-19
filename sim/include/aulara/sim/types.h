#pragma once
#include <cstdint>
#include <type_traits>

namespace aulara {

using MaterialId = std::uint8_t;

enum class Material : MaterialId {
    Air = 0,
    Sand = 1,
    Stone = 2,
    Water = 3
};

constexpr MaterialId id(Material m) {return static_cast<MaterialId>(m); }

struct Cell {
    MaterialId material = 0;
    std::uint8_t flags = 0;
    std::uint8_t hp = 0;
    std::uint8_t shade = 0;
};
static_assert(sizeof(Cell) == 4, "Keep Cell at 4 bytes");
static_assert(std::is_trivially_copyable_v<Cell>, "Cell must stay memcpyable");
static_assert(std::is_standard_layout_v<Cell>, "Cell must stay layout predictable");

} // namespace aulara