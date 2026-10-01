#include <cstddef>
#include <cstdint>
#include <vector>
#include <algorithm>

#include "aulara/sim/world.h"
#include "aulara/sim/material.h"
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
    cells_(static_cast<std::size_t>(width) * height),
    chunks_(width, height) {}

void World::step() {
    AULARA_ZONE();

    ++frame_;
    bool reverse = frame_ % 2 != 0;
    const std::uint8_t clock = frame_clock(frame_);
    Rules rules;
    Rng rng(hash64(seed_, frame_, 0));
    CellContext world_context = CellContext(cells_.data(), width_, height_, mats_, rng);

    // awake flags written during the previous step and by set_cell since become this step's read set
    chunks_.begin_step();
    chunks_scanned_ = 0;
    const int loop_step = reverse ? -1 : 1;

    for (int y = height_ - 1; y >= 0; y-- ) {
        const int chunk_row = y >> chunk_shift;

        // same row-major scan as before, but sleeping chunk spans of the row are skipped
        for (int i = 0; i < chunks_.cols(); i++) {
            const int chunk_col = reverse ? chunks_.cols() - 1 - i : i;
            if (!chunks_.awake(chunk_col, chunk_row)) continue;
            // count each awake chunk once, on its bottom row (the first row the scan reaches)
            if (y == height_ - 1 || ((y + 1) & (chunk_size - 1)) == 0) chunks_scanned_++;

            const int x0 = chunk_col << chunk_shift;
            const int x1 = std::min(x0 + chunk_size, width_); // last chunk column may be partial
            const int start = reverse ? x1 - 1 : x0;
            const int end = reverse ? x0 - 1 : x1;

            // first and last x in this span that needs waking. Every move today is at most 1 cell, so the
            // 3x3 neighborhoods of these two cover the same chunks as waking every mover individually,
            // for 2 wake() calls per span instead of 1 per moved cell
            int wake_first = -1;
            int wake_last = -1;

            for (int x = start; x != end; x += loop_step) {
                // note about reading from c: after any rule calls, c can point to a different cell so move reads to top of loop like mat_def
                Cell &c = cells_[index(x, y)];
                if (c.material == id(aulara::Material::Air)) continue;
                const MaterialDef &mat_def = mats_[c.material];
                if (mat_def.phase == Phase::Empty) continue;
                if (mat_def.phase == Phase::Static) continue;
                // a matching clock is either a cell that already moved this step or a stale stamp from before
                // its chunk slept; waking keeps the stale case from being dropped and letting the chunk fall asleep
                bool moved = false;
                if ((c.flags & kClockBit) == clock) {
                    moved = true;
                }
                else {
                    c.flags = (c.flags & ~kClockBit) | clock;
                    switch (mat_def.phase) {
                        case Phase::Empty: {
                            break;
                        }
                        case Phase::Static: {
                            break;
                        }
                        case Phase::Powder: {
                            moved = rules.update_powder(world_context, x, y, mat_def);
                            break;
                        }
                        case Phase::Liquid: {
                            moved = rules.update_liquid(world_context, x, y, mat_def);
                            break;
                        }
                        case Phase::Gas: {
                            break;
                        }
                        case Phase::Particle: {
                            break;
                        }
                    } // end of switch
                }
                if (moved) {
                    if (wake_first < 0) wake_first = x;
                    wake_last = x;
                }
            } // end of inner width loop

            if (wake_first >= 0) {
                chunks_.wake(wake_first, y);
                chunks_.wake(wake_last, y);
            }
        } // end of chunk column loop
    } // end of outer height loop
}

void World::set_cell(int x, int y, MaterialId m) {
    Cell &c = cells_[index(x, y)];
    c.material = m;
    c.flags = frame_clock(frame_);
    if (m != id(Material::Air)) {
        c.shade = hash64(static_cast<uint64_t>(x), static_cast<uint64_t>(y), frame_);
    }
    else {
        c.shade = 0;
    }
    chunks_.wake(x, y);
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