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
    const MaterialTable &mats() const { return *mats_; }
    Rng &rng() { return *rng_; }
    const Cell &at(int x, int y) const { return cells_[index(x, y)]; }
    
    // public movement functions
    // can_move_down takes the (x, y) coordinates of the current cell being evaluated 
    // while movement functions like can_move_shear and can_move_sideways take only the y and use
    // the x coordinate of the position the current cell wants to move into
    // this is to simplify the rng: rng gets advanced one state per movement and whether to move left
    // or right gets decided before hand and input into the can_move functions that require it
    bool can_move_down(int x, int y, const MaterialDef &def) const;
    bool can_move_shear(int x, int y, const MaterialDef &def) const;
    bool can_move_sideways(int x, int y) const;

    // public for now, move to private later?
    bool in_bounds(int x, int y) const { return x >= 0 && x < width_ && y < height_ && y >= 0; }
    void swap_cells(int ax, int ay, int bx, int by) { std::swap(cells_[index(ax, ay)], cells_[index(bx, by)]); }
    
    private:
    // primative movement helper functions
    bool is_passable(int x, int y, const MaterialDef &mover) const { 
        return mover.density > (*mats_)[cells_[index(x, y)].material].density
        && !is_static(x, y); }
    bool is_static(int x, int y) const { return (*mats_)[cells_[index(x, y)].material].phase == Phase::Static; }
    bool is_empty(int x, int y) const { return (*mats_)[cells_[index(x, y)].material].phase == Phase::Empty; }

    // index helper
    std::size_t index(int x, int y) const { 
        assert(x >= 0 && x < width_ && "index(): x outside the grid");
        assert(y >= 0 && y < height_ && "index(): y outside the grid");
        return static_cast<std::size_t>(y) * width_ + x; }

    // vars
    Cell *cells_;
    int width_;
    int height_;
    const MaterialTable *mats_;
    Rng *rng_;
};

} // namespace aulara
