#include "aulara/sim/material.h"
#include "aulara/sim/types.h"
#include <cassert>

namespace aulara {

MaterialTable::MaterialTable() {
    MaterialDef air;
    air.name = "air";
    air.phase = Phase::Empty;
    air.density = 0;
    air.base_color = {12, 12, 16};

    MaterialDef sand;
    sand.name = "sand";
    sand.phase = Phase::Powder;
    sand.density = 150;
    sand.base_color = {194, 178, 128};
    sand.friction = 60;

    MaterialDef stone;
    stone.name = "stone";
    stone.phase = Phase::Static;
    stone.density = 255;
    stone.base_color = {110, 110, 115};
    stone.friction = 255;

    MaterialDef water;
    water.name = "water";
    water.phase = Phase::Liquid;
    water.density = 60;
    water.base_color = {56, 132, 207};
    water.friction = 0;

    [[maybe_unused]] const MaterialId air_ = add(std::move(air));
    [[maybe_unused]] const MaterialId sand_ = add(std::move(sand));
    [[maybe_unused]] const MaterialId stone_ = add(std::move(stone));
    [[maybe_unused]] const MaterialId water_ = add(std::move(water));

    assert(air_ == id(Material::Air));
    assert(sand_ == id(Material::Sand));
    assert(stone_ == id(Material::Stone));
    assert(water_ == id(Material::Water));
}

MaterialId MaterialTable::add(MaterialDef md) {
    assert(defs_.size() < 256 && "MaterialId is uint8_t; table is full");
    const MaterialId new_id = static_cast<MaterialId>(defs_.size());
    defs_.push_back(std::move(md));
    return new_id;
}


} // namespace aulara