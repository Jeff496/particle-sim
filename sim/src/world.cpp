#include <cstddef>
#include <cstdint>
#include <sys/types.h>
#include <vector>
#include <utility>

#include "aulara/sim/world.h"
#include "aulara/sim/profile.h"
#include "aulara/sim/types.h"

namespace aulara {

namespace {
    enum class Direction : std::uint8_t{
        Down = 0,
        Diagonal = 1,
    };

    std::uint64_t xorshift64(std::uint64_t num) {
        num ^= num << 13;
        num ^= num >> 7;
        num ^= num << 17;

        return num;
    }

    std::uint64_t hash3(std::uint64_t x, std::uint64_t y, std::uint64_t frame) {
        std::uint64_t h = x * 0x9E3779B97F4A7C15ULL;
        h ^= y + 0x9E3779B97F4A7C15ULL + (h << 6) + (h >> 2);
        h ^= frame + 0x9E3779B97F4A7C15ULL + (h << 6) + (h >> 2);

        // Final mixing
        h ^= h >> 30;
        h *= 0xBF58476D1CE4E5B9ULL;
        h ^= h >> 27;
        h *= 0x94D049BB133111EBULL;
        h ^= h >> 31;

        return h;
    }

    bool can_move (const World &w, int x, int y, Direction d, int dir) {
        switch (d) {
            case Direction::Down: {
                bool is_air = false;
                if (y + 1 < w.height()) {
                    is_air = w.get_cell(x, y + 1).material == Material::Air;
                }
                return is_air;
            }
            case Direction::Diagonal: {
                bool in_bounds = false;
                bool is_air = false;
                bool not_surrounded = false;
                if (dir == -1) {
                    in_bounds = x - 1 >= 0;
                }
                else {
                    in_bounds = x + 1 < w.width();
                }
                if (y + 1 < w.height() && in_bounds) {
                    is_air = w.get_cell(x + dir, y + 1).material == Material::Air;   
                    not_surrounded = w.get_cell(x + dir, y).material != Material::Stone;
                }
                return is_air && not_surrounded;
            }
        }
        return false;
    }
}


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

            if (cells_[index(x, y)].material == Material::Sand) {
    
                if (can_move(*this, x, y, Direction::Down, 0)) {
                    std::swap(cells_[index(x, y)], cells_[index(x, y + 1)]);
                }
                else {
                    rng_state_ = xorshift64(rng_state_);
                    int dir = (rng_state_ >> 8) & 1 ? 1 : -1;

                    if (can_move(*this, x, y, Direction::Diagonal, dir)) {
                        std::swap(cells_[index(x, y)], cells_[index(x + dir, y + 1)]);
                    }
                    else if (can_move(*this, x, y, Direction::Diagonal, -dir)) {
                        std::swap(cells_[index(x, y)], cells_[index(x - dir, y + 1)]);
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
        c.shade = hash3(static_cast<uint64_t>(x), static_cast<uint64_t>(y), frame_);
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
        out[i * 4 + 0] = std::clamp(r + shade_mask, 0, 255);
        out[i * 4 + 1] = std::clamp(g + shade_mask, 0, 255);
        out[i * 4 + 2] = std::clamp(b + shade_mask, 0, 255);
        out[i * 4 + 3] = 255;
    }
}

} // namespace aulara