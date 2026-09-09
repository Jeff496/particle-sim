#include <cstddef>
#include <cstdint>
#include <vector>

#include "aulara/sim/world.h"
#include "aulara/sim/profile.h"
#include "aulara/sim/types.h"

namespace aulara {

enum class Direction : std::uint8_t{
    Down = 0,
    Diagonal = 1,
};

namespace {
    std::uint64_t xorshift64(std::uint64_t& num) {
        num ^= num << 13;
        num ^= num >> 7;
        num ^= num << 17;
    
        return num;
    }
}

bool canMove (const World &w, int x, int y, Direction d, int dir) {
    switch (d) {
        case Direction::Down: {
            bool isAir = false;
            if (y + 1 < w.height()) {
                isAir = w.get_cell(x, y + 1).material == Material::Air;
            }
            return isAir;
        }
        case Direction::Diagonal: {
            bool inBounds = false;
            bool isAir = false;
            bool notSurrounded = false;
            if (dir == -1) {
                inBounds = x - 1 >= 0;
            }
            else {
                inBounds = x + 1 < w.width();
            }
            if (y + 1 < w.height() && inBounds) {
                isAir = w.get_cell(x + dir, y + 1).material == Material::Air;   
                notSurrounded = w.get_cell(x + dir, y).material != Material::Stone;
            }
            return isAir && notSurrounded;
        }
    }
    return false;
}; // lambda Direction func

World::World(int width, int height, std::uint64_t seed) : 
    width_(width), 
    height_(height), 
    rng_state_(seed ? seed : 1), 
    cells_(static_cast<std::size_t>(width) * height) {}

void World::step() {
    AULARA_ZONE();

    ++frame_;
    bool reverse = frame_ % 2 != 0;

    for (int y = height_ - 1; y >= 0; y-- ) {
        int start = reverse ? width_ - 1 : 0;
        int end = reverse ? -1 : width_;
        int loop_step = reverse ? -1 : 1;
        
        for (int x = start; x != end; x += loop_step) {
            std::size_t cells_index = index(x, y);

            if (cells_[cells_index].material == Material::Sand) {
    
                if (canMove(*this, x, y, Direction::Down, 0)) {
                    std::swap(cells_[cells_index], cells_[cells_index + width_]);
                }
                else {
                    rng_state_ = xorshift64(rng_state_);
                    int dir = (rng_state_ >> 8) & 1 ? 1 : -1;

                    if (canMove(*this, x, y, Direction::Diagonal, dir)) {
                        std::swap(cells_[cells_index], cells_[cells_index + width_ + dir]);
                    }
                    else if (canMove(*this, x, y, Direction::Diagonal, -dir)) {
                        std::swap(cells_[cells_index], cells_[cells_index + width_ - dir]);
                    }
                }
                
            } // end of individual cell check
        } // end of inner width loop
    } // end of outer height loop
}

void World::set_cell(int x, int y, Material m) {
    Cell &c = cells_[index(x, y)];
    c.material = m;
    if (m != Material::Air) {
        c.shade = static_cast<uint8_t>(xorshift64(rng_state_));
    }
    else {
        c.shade = 0;
    }
}

void World::render_rgba(std::uint8_t *out) const {
    AULARA_ZONE();
    for (std::size_t i = 0; i < cells_.size(); i++) {
        std::uint8_t r = 0, g = 0, b = 0;
        switch (cells_[i].material) {
            case Material::Air: r = 20; g = 20; b = 24; break;
            case Material::Sand: r = 214; g = 184; b = 110; break;
            case Material::Stone: r = 110; g = 110; b = 118; break;
        }
        int shade_mask = static_cast<int>(cells_[i].shade) & 31;
        out[i * 4 + 0] = r + shade_mask;
        out[i * 4 + 1] = g + shade_mask;
        out[i * 4 + 2] = b + shade_mask;
        out[i * 4 + 3] = 255;
    }
}

} // namespace aulara