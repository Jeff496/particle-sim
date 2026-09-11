#pragma once
#include <cstdint>
#include <utility>

namespace aulara {

inline std::uint64_t hash64(std::uint64_t a, std::uint64_t b, std::uint64_t c) {
    std::uint64_t h = a * 0x9E3779B97F4A7C15ULL;
    h ^= b + 0x9E3779B97F4A7C15ULL + (h << 6) + (h >> 2);
    h ^= c + 0x9E3779B97F4A7C15ULL + (h << 6) + (h >> 2);

    // Final mixing
    h ^= h >> 30;
    h *= 0xBF58476D1CE4E5B9ULL;
    h ^= h >> 27;
    h *= 0x94D049BB133111EBULL;
    h ^= h >> 31;

    return h;
}

inline std::uint64_t xorshift64(std::uint64_t num) {
        num ^= num << 13;
        num ^= num >> 7;
        num ^= num << 17;

        return num;
}

class Rng {
public:
    explicit Rng(std::uint64_t seed) : s_(seed ? seed : 1) {};
    std::uint64_t next() { return s_ = xorshift64(s_); }
    bool chance(std::uint8_t p_256) { return (next() >> 56) < p_256; }
    int pick(int a, int b) { if (a > b) std::swap(a, b);
    const std::uint64_t span = static_cast<std::uint64_t>(b - a) + 1;
    return a + static_cast<int>(((next() >> 32) * span) >> 32);}
private:
    std::uint64_t s_;
};

} // namespace aulara