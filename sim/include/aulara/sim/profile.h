#pragma once

#ifdef AULARA_PROFILE
#include <tracy/Tracy.hpp>
#define AULARA_ZONE() ZoneScoped
#define AULARA_ZONE_N(name) ZoneScopedN(name)
#define AULARA_FRAME() FrameMark
#else
#define AULARA_ZONE()
#define AULARA_ZONE_N(name)
#define AULARA_FRAME()
#endif