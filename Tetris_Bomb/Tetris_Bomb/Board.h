#pragma once
#include "Common.h"
#include "Block.h"

namespace Tetris {
    class Board {
    private:
        // [수정] 값은 0 아니면 1이니까 int 대신 unsigned char 
        //“보드 상태 저장에 int 배열 대신 1바이트 단위(unsigned char)를 사용하여, 
        // 동일한 정보를 더 적은 메모리로 표현하고 캐시 효율을 높였다.”
        unsigned char grid[BOARD_HEIGHT][BOARD_WIDTH];
        int level;

    public:
        static constexpr int EMPTY = 0;
        static constexpr int FILLED = 1;
        Board();
        void setLevel(int lvl);
        void reset();
        void draw() const;

        // 다형성을 위해 BlockBase 참조를 받습니다
        bool checkCollision(const BlockBase& b, int nextX, int nextY, int nextAngle) const;
        void merge(const BlockBase& b);
        int processFullLines();

        // [추가] 특정 좌표 주변을 비우는 explode 함수, 파괴된 블록 수 반환
        int explode(int centerX, int centerY);
    };
}