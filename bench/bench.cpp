// Fixed-scenario benchmark. Release builds only -- see bench/CMakeLists.txt.
//
// Every number recorded in docs/bench.md comes from here, never from the
// playground's `step:` readout: that one is measured inside a vsync'd frame
// alongside ImGui and whatever the mouse was doing.
//
// Method, per scenario: build a fresh World, run `kWarmup` untimed steps, then
// time `kTimed` steps and divide. Repeat three times and take the median.

#include <algorithm>
#include <chrono>
#include <cstdio>

#include "aulara/sim/world.h"

using namespace aulara;

namespace {

// Fixed for every run so numbers stay comparable. Do not read a bench number
// against a playground number: the playground grid is a different size.
constexpr int kW = 1920;
constexpr int kH = 1080;

// Warm-up absorbs first-touch page faults on the freshly allocated 8 MB grid,
// cold caches, and an untrained branch predictor. Timing those measures the
// allocator, not the rules.
constexpr int kWarmup = 10;
constexpr int kTimed = 20;
constexpr int kRepeats = 3;

using Scene = void (*)(World &);

// Nothing but air. The floor cost of the sweep itself: every cell visited,
// none doing work. Chunk sleeping should drive this toward zero.
void scene_empty(World &) {}

// Sand at rest on a stone floor. Today's worst case: update_powder runs on
// every grain and none of them move.
void scene_settled(World &w) {
    for (int x = 0; x < kW; ++x) {
        w.set_cell(x, kH - 1, id(Material::Stone));
    }
    for (int y = kH / 2; y < kH - 1; ++y) {
        for (int x = 0; x < kW; ++x) {
            w.set_cell(x, y, id(Material::Sand));
        }
    }
}

// Top half sand, nothing under it. Worst case for movement: maximum swaps.
// Note this scenario decays -- sand falls ~1 cell/step, so over enough steps it
// becomes `settled`. At kWarmup + kTimed = 30 steps it has moved 30 of 1080
// rows, which is negligible. Raising the step counts changes what is measured.
void scene_falling(World &w) {
    for (int y = 0; y < kH / 2; ++y) {
        for (int x = 0; x < kW; ++x) {
            w.set_cell(x, y, id(Material::Sand));
        }
    }
}

// One timed run. A fresh World each time: repeat 2 must not start from where
// repeat 1 finished.
double run_once(Scene build) {
    World w(kW, kH, /*seed=*/1);
    build(w);

    for (int i = 0; i < kWarmup; ++i) {
        w.step();
    }

    // steady_clock, not system_clock: the latter can jump when NTP adjusts.
    // Timing the whole batch rather than each step keeps the clock calls out of
    // the measurement entirely.
    const auto t0 = std::chrono::steady_clock::now();
    for (int i = 0; i < kTimed; ++i) {
        w.step();
    }
    const auto t1 = std::chrono::steady_clock::now();

    return std::chrono::duration<double, std::milli>(t1 - t0).count() / kTimed;
}

// Median, not mean: one repeat losing a core to a background process should be
// discarded, not averaged in.
double median_of_repeats(Scene build) {
    double runs[kRepeats];
    for (int i = 0; i < kRepeats; ++i) {
        runs[i] = run_once(build);
    }
    std::sort(runs, runs + kRepeats);
    return runs[kRepeats / 2];
}

struct Entry {
    const char *name;
    Scene build;
};

} // namespace

int main() {
    const Entry scenes[] = {
        {"empty", scene_empty},
        {"settled", scene_settled},
        {"falling", scene_falling},
    };

    std::printf("aulara bench -- %dx%d, median of %d, %d warm-up + %d timed steps\n\n",
                kW, kH, kRepeats, kWarmup, kTimed);
    std::printf("%-10s %10s\n", "scenario", "ms/step");
    std::printf("%-10s %10s\n", "----------", "----------");

    double results[sizeof(scenes) / sizeof(scenes[0])];
    for (std::size_t i = 0; i < sizeof(scenes) / sizeof(scenes[0]); ++i) {
        results[i] = median_of_repeats(scenes[i].build);
        std::printf("%-10s %10.3f\n", scenes[i].name, results[i]);
    }

    // Paste-ready rows for docs/bench.md. Fill in commit and machine yourself:
    // the binary has no honest way to know either.
    std::printf("\nrows for docs/bench.md (fill in <commit> and <machine>):\n\n");
    for (std::size_t i = 0; i < sizeof(scenes) / sizeof(scenes[0]); ++i) {
        std::printf("| YYYY-MM-DD | <commit> | <machine> | release | %s | %.3f |\n",
                    scenes[i].name, results[i]);
    }

    return 0;
}
