#pragma once
#include "Common.h"
#include "Board.h"
#include "Block.h"
#include <string>
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
        int heldBlockType = -1; // -1: 비어있음
        bool canHold = true;    // true: 이번 턴에 사용 가능
        const int HIGH_SCORE_LIMIT = 10; // TOP N 점수 출력 (예: 5개)

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
        void showHoldBlock();

        // 게임 오버 시 닉네임 입력하고 점수 저장하는 함수
        void promptNameAndSaveScore();
        // 점수판 출력
        void showHighScores(int maxCount, int startX, int startY, 
            const std::string* curName = nullptr, int curScore = 0);
    public:
        Game();
        void run();
    };
}