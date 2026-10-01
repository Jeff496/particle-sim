#pragma once
#include <algorithm>
#include <cstdint>
#include <vector>


namespace aulara {
constexpr int chunk_shift = 6;
constexpr int chunk_size = 1 << chunk_shift;

class ChunkGrid {
public:
    ChunkGrid(int w, int h) : 
        width_(w), 
        height_(h), 
        cols_((w + chunk_size - 1) >> chunk_shift), 
        rows_((h + chunk_size - 1) >> chunk_shift), 
        awake_(cols_ * rows_, 0), 
        awake_next_(cols_ * rows_, 1) {};

    int cols() const { return cols_; }
    bool awake(int cx, int cy) const { return awake_[cy * cols_ + cx]; }

    void begin_step() {
        awake_.swap(awake_next_);
        std::fill(awake_next_.begin(), awake_next_.end(), 0);
    }

    void wake(int x, int y) {
        // fast path: a cell not on its chunk's border has its whole 3x3 neighborhood inside one chunk
        const int lx = x & (chunk_size - 1);
        const int ly = y & (chunk_size - 1);
        if (lx != 0 && lx != chunk_size - 1 && ly != 0 && ly != chunk_size - 1) {
            awake_next_[(y >> chunk_shift) * cols_ + (x >> chunk_shift)] = 1;
            return;
        }

        const int cx0 = std::max(x - 1, 0) >> chunk_shift;
        const int cx1 = std::min(x + 1, width_ - 1) >> chunk_shift;
        const int cy0 = std::max(y - 1, 0) >> chunk_shift;
        const int cy1 = std::min(y + 1, height_ - 1) >> chunk_shift;

        for (int cy = cy0; cy <= cy1; cy++) {
            for (int cx = cx0; cx <= cx1; cx++) {
                awake_next_[cy * cols_ + cx] = 1;
            }
        }
    }


private:
    int width_;
    int height_;
    int cols_;
    int rows_;
    std::vector<std::uint8_t> awake_;
    std::vector<std::uint8_t> awake_next_;


};

} // namespace aulara