#include "aulara/sim/cell_context.h"
#include "aulara/sim/material.h"

namespace aulara {

bool CellContext::can_move_down(int x, int y, const MaterialDef &def) const{
    return in_bounds(x, y + 1) && is_passable(x, y + 1, def);
}

bool CellContext::can_move_shear(int x, int y, const MaterialDef & def) const{
    return in_bounds(x, y) && !is_static(x, y) && in_bounds(x, y + 1) && is_passable(x, y + 1, def);
}

bool CellContext::can_move_sideways(int x, int y) const{
    return in_bounds(x, y) && is_empty(x, y);
}

} // namespace aulara