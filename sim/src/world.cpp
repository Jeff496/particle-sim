#include <cstddef>
#include <cstdint>
#include <vector>
#include <algorithm>

#include "aulara/sim/world.h"
#include "aulara/sim/profile.h"
#include "aulara/sim/rules.h"
#include "aulara/sim/cell_context.h"
#include "aulara/sim/rng.h"
#include "aulara/sim/types.h"

namespace aulara {

World::World(int width, int height, std::uint64_t seed) : 
    width_(width), 
    height_(height), 
    seed_(seed ? seed : 1), 
    cells_(static_cast<std::size_t>(width) * height) {}

void World::step() {
    AULARA_ZONE();

    ++frame_;
    bool reverse = frame_ % 2 != 0;
    Rules rules;
    Rng rng(hash64(seed_, frame_, 0));
    CellContext world_context = CellContext(cells_.data(), width_, height_, mats_, rng);

    for (int y = height_ - 1; y >= 0; y-- ) {
        int start = reverse ? width_ - 1 : 0;
        int end = reverse ? -1 : width_;
        int loop_step = reverse ? -1 : 1;
        
        for (int x = start; x != end; x += loop_step) {
            const Cell c = cells_[index(x, y)];
            if (c.material == id(Material::Air)) continue;
            switch (mats_[c.material].phase) {
                case Phase::Empty: {
                    break;
                }
                case Phase::Static: {
                    break;
                }
                case Phase::Powder: {
                    rules.update_powder(world_context, x, y, mats_[c.material]);
                    break;
                }
                case Phase::Liquid: {
                    break;
                }
                case Phase::Gas: {
                    break;
                }
                case Phase::Particle: {
                    break;
                }
            }
        } // end of inner width loop
    } // end of outer height loop
}

void World::set_cell(int x, int y, MaterialId m) {
    Cell &c = cells_[index(x, y)];
    c.material = m;
    if (m != id(Material::Air)) {
        c.shade = hash64(static_cast<uint64_t>(x), static_cast<uint64_t>(y), frame_);
    }
    else {
        c.shade = 0;
    }
}

void World::render_rgba(std::uint8_t *out) const {
    AULARA_ZONE();
    for (std::size_t i = 0; i < cells_.size(); i++) {
        auto [r, g, b] = mats_[cells_[i].material].base_color;
        int shade_mask = static_cast<int>(cells_[i].shade) & 31;
        out[i * 4 + 0] = std::clamp(r + shade_mask, 0, 255);
        out[i * 4 + 1] = std::clamp(g + shade_mask, 0, 255);
        out[i * 4 + 2] = std::clamp(b + shade_mask, 0, 255);
        out[i * 4 + 3] = 255;
    }
}

} // namespace aulara