#pragma once
#include <cstddef>
#include <cstdint>
#include <vector>

#include "aulara/sim/types.h"

namespace aulara {

class World {
public:
    World(int width, int height, std::uint64_t seed = 1);
    int width() const { return width_; }
    int height() const { return height_; }
    std::uint64_t frame() const { return frame_; }

    void step();

    const Cell &get_cell(int x, int y) const { return cells_[index(x, y)]; }
    void set_cell(int x, int y, Material m);

    void render_rgba(std::uint8_t *out) const;

private:
    std::size_t index(int x, int y) const { return static_cast<std::size_t>(y) * width_ + x; }

    int width_;
    int height_;
    std::uint64_t frame_ = 0;
    std::uint64_t rng_state_;
    std::vector<Cell> cells_;
};

} // namespace aulara