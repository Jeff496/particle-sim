#include "aulara/sim/rules.h"
#include "aulara/sim/cell_context.h"

namespace aulara {

bool Rules::update_powder(CellContext &ctx, int x, int y, const MaterialDef &def) {
    // move down
    if (ctx.can_move_down(x, y, def)) {
        ctx.swap_cells(x, y, x, y + 1);
        return true;
    }

    // move diagonal
    const int first = ctx.rng().chance(128) ? -1 : 1;
    for (int dir : {first, -first}) {
        if (ctx.can_move_shear(x + dir, y, def)) {
            ctx.swap_cells(x, y, x + dir, y + 1);
            return true;
        }
    }
    return false;
}

bool Rules::update_liquid(CellContext &ctx, int x, int y, const MaterialDef &def) {
    // move down
    if (ctx.can_move_down(x, y, def)) {
        ctx.swap_cells(x, y, x, y + 1);
        return true;
    }

    // randomly decide whether to check left or right diagonal/sideways direction first
    const int first = ctx.rng().chance(128) ? -1 : 1;

    // move diagonal
    for (int dir : {first, -first}) {
        if (ctx.can_move_shear(x + dir, y, def)) {
            ctx.swap_cells(x, y, x + dir, y + 1);
            return true;
        }
    }

    // move sideways
    for (int dir : {first, -first}) {
        if (ctx.can_move_sideways(x + dir, y)) {
            ctx.swap_cells(x, y, x + dir, y);
            return true;
        }
    }

    return false;
}

} // namespace aulara