#include "aulara/sim/material.h"
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

    const MaterialId air_ = add(std::move(air));
    const MaterialId sand_ = add(std::move(sand));
    const MaterialId stone_ = add(std::move(stone));

    assert(air_ == id(Material::Air));
    assert(sand_ == id(Material::Sand));
    assert(stone_ == id(Material::Stone));
    (void)air_;
    (void)sand_;
    (void)stone_; 

}

MaterialId MaterialTable::add(MaterialDef md) {
    assert(defs_.size() < 256 && "MaterialId is uint8_t; table is full");
    const MaterialId new_id = static_cast<MaterialId>(defs_.size());
    defs_.push_back(std::move(md));
    return new_id;
}


} // namespace aulara