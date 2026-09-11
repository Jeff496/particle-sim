#pragma once
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <vector>

#include "aulara/sim/types.h"
#include "aulara/sim/material.h"
#include "aulara/sim/chunk.h"

namespace aulara {

class World {
public:
    World(int width, int height, std::uint64_t seed = 1);
    int width() const { return width_; }
    int height() const { return height_; }
    std::uint64_t frame() const { return frame_; }

    void step();

    const Cell &get_cell(int x, int y) const { return cells_[index(x, y)]; }
    void set_cell(int x, int y, MaterialId);

    void render_rgba(std::uint8_t *out) const;

    const MaterialTable &materials() const { return mats_; }

private:
    // Make sure release config has NDEBUG to remove asserts in build
    std::size_t index(int x, int y) const {
        assert(x >= 0 && x < width_ && "index(): x outside the grid");
        assert(y >= 0 && y < height_ && "index(): y outside the grid");
        return static_cast<std::size_t>(y) * width_ + x;
    }

    int width_;
    int height_;
    std::uint64_t frame_ = 0;
    std::uint64_t seed_;
    MaterialTable mats_;
    std::vector<Cell> cells_;
};

} // namespace aulara