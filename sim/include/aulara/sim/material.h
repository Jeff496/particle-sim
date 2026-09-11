#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include "aulara/sim/types.h"

namespace aulara {

struct Color { std::uint8_t r, g, b; };

struct MaterialDef {
    std::string name;
    Phase phase = Phase::Static;
    std::uint8_t density = 0;
    Color base_color{0, 0, 0};
    std::uint8_t friction = 0;
    std::uint8_t spread = 0;
};

class MaterialTable {
public:
    MaterialTable();
    const MaterialDef &operator[](MaterialId id) const {return defs_[id]; }
    std::size_t size() const { return defs_.size(); };
    MaterialId add(MaterialDef md);
private:
    std::vector<MaterialDef> defs_;
};

} // namespace aulara