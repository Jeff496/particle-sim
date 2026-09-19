#pragma once
#include "aulara/sim/cell_context.h"

namespace aulara {

class Rules {
public:
    bool update_powder(CellContext &ctx, int x, int y, const MaterialDef &def);
    bool update_liquid(CellContext &ctx, int x, int y, const MaterialDef &def);
    bool update_gas(CellContext &ctx, int x, int y, const MaterialDef &def);

};

} // namespace aulara