#include "aulara/sim/rules.h"
#include "aulara/sim/cell_context.h"

namespace aulara {

bool Rules::update_powder(CellContext &ctx, int x, int y, const MaterialDef &def) {
    if (ctx.in_bounds(x, y + 1) && ctx.is_passable(x, y + 1, def)) { // move down
        ctx.swap_cells(x, y, x, y + 1);
        return true;
    }
    const int first = ctx.rng().chance(128) ? -1 : 1;
    for (int dir : {first, -first}) {
        if (ctx.in_bounds(x + dir, y) && !ctx.is_static(x + dir, y) && ctx.in_bounds(x + dir, y + 1) && ctx.is_passable(x + dir, y + 1, def)) {
            ctx.swap_cells(x, y, x + dir, y + 1);
            return true;
        }
    }
    return false;
}

}