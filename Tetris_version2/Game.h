#pragma once
#include "Common.h"
#include "Board.h"
#include "Block.h"
#include <vector>

namespace Tetris {
    class Game {
    private:
        Board board;
        ShapeBlock currentBlock;
        int nextBlockType;
        int level;
        int score;
        int totalLines;
        bool gameOver;
        bool isPaused = false;


        std::vector<StageConfig> stages;

        // 내부 헬퍼 함수들
        void initStages();
        void resetGame();
        int makeNewBlockType();
        void spawnNextBlock();
        void handleInput();
        bool moveDown();
        void moveSide(int dx);
        void rotateBlock();

        // UI 함수들
        void showStats();
        void showNextBlockPreview();
        void selectLevel();
        void showLogo();
        void showGameOver();
        void drawGhost(bool erase);
    public:
        Game();
        void run();
    };
}