# Benchmark log

Append-only. One line per scenario per run. **Never edit a past line** — the
value of this file is the shape of the curve over months, and an edited row is a
lost data point.

Every number comes from the `bench` target, never from the playground's `step:`
readout. To produce a row:

```sh
cmake --preset release && cmake --build --preset release --target bench
./build/release/bench/bench
```

Method (fixed in `bench/bench.cpp`): 1920x1080, median of 3 runs, 10 warm-up +
20 timed steps, fresh `World` per run.

Flag with -DNDEBUG to check if build flags are reaching the sim:
clang++ -std=c++17 -O2 -DNDEBUG -Isim/include bench/bench.cpp sim/src/*.cpp -o /tmp/b_ndebug
clang++ -std=c++17 -O2          -Isim/include bench/bench.cpp sim/src/*.cpp -o /tmp/b_assert

Change kTimed to double or triple its value to check if setup is leaking into timing:
in bench.cpp: constexpr int kTimed = 20;

## Rules

- Bench in `release`, never `debug`. The debug preset cannot build this target.
- Close other apps. If the three medians disagree by more than a few percent,
  the machine is too noisy — don't record it.
- **Never compare across the `machine` column.** The Phase 0 baseline below was
  taken on a different CPU and is not comparable to anything measured since.
- Every optimisation commit adds a row. If the number didn't move, the
  optimisation didn't happen.

## Log

- command for format: ./build/bench/bench/bench | sed "s/YYYY-MM-DD/$(date +%F)/;
  s/<commit>/$(git rev-parse --short HEAD)/; s|<machine>|M2 Pro 14in|"

| date | commit | machine | preset | scenario | ms/step |
|---|---|---|---|---|---|
| (phase 0) | — | 2.1 GHz Xeon, 1 core | release | empty | 2.0 |
| (phase 0) | — | 2.1 GHz Xeon, 1 core | release | falling | 4.4 |
| (phase 0) | — | 2.1 GHz Xeon, 1 core | release | settled | 17.2 |

| 2026-09-11 | ee261dc | M2 Pro 14in | release | empty | 1.321 |
| 2026-09-11 | ee261dc | M2 Pro 14in | release | settled | 7.779 |
| 2026-09-11 | ee261dc | M2 Pro 14in | release | falling | 3.892 |