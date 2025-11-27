#pragma once
#include "Common.h"
#include "Block.h"

namespace Tetris {
    class Board {
    private:
        int grid[BOARD_HEIGHT][BOARD_WIDTH];
        int level;

    public:
        static constexpr int EMPTY = -1;
        static constexpr int WALL = 9;
        Board();
        void setLevel(int lvl);
        void reset();
        void draw() const;

        // 다형성을 위해 BlockBase 참조를 받습니다
        bool checkCollision(const BlockBase& b, int nextX, int nextY, int nextAngle) const;
        void merge(const BlockBase& b);
        int processFullLines();
    };
}