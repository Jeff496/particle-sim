#pragma once
#include <cassert>
#include <utility>
#include <cstddef>
#include "aulara/sim/types.h"
#include "aulara/sim/material.h"
#include "aulara/sim/rng.h"

namespace aulara {

class CellContext {
public:
    CellContext(Cell *cells, int w, int h, const MaterialTable &mats, Rng &rng) : 
    cells_(cells), width_(w), height_(h), mats_(&mats), rng_(&rng) {}

    int width() const { return width_; }
    int height() const { return height_; }
    bool in_bounds(int x, int y) const { return x >= 0 && x < width_ && y < height_ && y >= 0; }
    const Cell &at(int x, int y) const { return cells_[index(x, y)]; }
    bool is_passable(int x, int y, const MaterialDef &mover) const { 
        return mover.density > (*mats_)[cells_[index(x, y)].material].density
        && !is_static(x, y); }
    bool is_static(int x, int y) const { return (*mats_)[cells_[index(x, y)].material].phase == Phase::Static; }
    void swap_cells(int ax, int ay, int bx, int by) { std::swap(cells_[index(ax, ay)], cells_[index(bx, by)]); }
    const MaterialTable &mats() const { return *mats_; }
    Rng &rng() { return *rng_; }

private:
    std::size_t index(int x, int y) const { 
        assert(x >= 0 && x < width_ && "index(): x outside the grid");
        assert(y >= 0 && y < height_ && "index(): y outside the grid");
        return static_cast<std::size_t>(y) * width_ + x; }
    Cell *cells_;
    int width_;
    int height_;
    const MaterialTable *mats_;
    Rng *rng_;
};

} // namespace aulara
