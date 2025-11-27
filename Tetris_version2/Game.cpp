#include "Game.h"
#include "ConsoleHelper.h"
#include "ShapeRepository.h"
#include <iostream>
#include <conio.h>
#include <ctime>
#include <windows.h> // Sleep

namespace Tetris {
    Game::Game() : level(0), score(0), totalLines(0), gameOver(false) {
        srand(static_cast<unsigned>(time(NULL)));
        initStages();
    }

    void Game::run() {
        ConsoleHelper::cursorVisible(false);
        showLogo();

        while (true) {
            resetGame();
            selectLevel();
            ConsoleHelper::clear();

            board.setLevel(level);
            board.reset();
            heldBlockType = -1;
            canHold = true;
            board.draw();

            nextBlockType = makeNewBlockType();
            spawnNextBlock();
            showStats();
            showNextBlockPreview();

            int frameCount = 0;
            while (!gameOver) {
                handleInput();

                //일시정지 로직
                if (isPaused) {
                    Sleep(15);
                    continue;
                }

                if (frameCount % stages[level].speed == 0) {
                    if (!moveDown()) {}
                }

                if (gameOver) {
                    showGameOver();
                    break;
                }

                Sleep(15);
                frameCount++;
            }
        }
    }

    void Game::initStages() {
        stages = {
            {40, 20, 6}, {38, 18, 8}, {35, 18, 10}, {30, 17, 12},
            {25, 16, 14}, {20, 14, 16}, {15, 14, 18}, {10, 13, 20},
            {6, 12, 22},  {4, 11, 99999}
        };
    }//speed/긴 막대기 획득 확률/레벨업 조건
    //레벨마다 클리어 조건 바꿈

    void Game::resetGame() {
        score = 0;
        totalLines = 0;
        gameOver = false;
    }

    int Game::makeNewBlockType() {
        int rnd = rand() % 100;
        if (rnd <= stages[level].stickRate) return 0;
        return (rand() % 6) + 1;
    }

  /*  void Game::spawnNextBlock() {
        currentBlock.spawn(nextBlockType);
        nextBlockType = makeNewBlockType();
        showNextBlockPreview();
    }*/
    void Game::spawnNextBlock() {
        currentBlock.spawn(nextBlockType);
        nextBlockType = makeNewBlockType();
        showNextBlockPreview();

        canHold = true;
        showHoldBlock();
        // [추가] 처음 나올 때도 그려주기
        drawGhost(false);
        currentBlock.draw();
    }
   

    void Game::handleInput() {
        if (_kbhit()) {
            int key = _getch();

            //// [추가] 'p' 또는 'P'를 누르면 일시정지 토글
            //    //일시정지 로직
            //if (key == 'p' || key == 'P') {
            //    isPaused = !isPaused;

            //    // 일시정지 상태면 화면에 표시
            //    if (isPaused) {
            //        ConsoleHelper::gotoXY(OFFSET_X + 4, 10);
            //        ConsoleHelper::setColor(Color::RED);
            //        printf(" PAUSED ");
            //    }
            //    else {
            //        // 다시 게임 화면(보드)을 덮어씌워 "PAUSED" 글자 지우기
            //        board.draw();
            //        currentBlock.draw();
            //    }
            //    return; // 입력을 처리했으니 종료
            //}

            //if (isPaused) return; // [추가] 일시정지 중이면 다른 키 입력 무시


            if (key == 0xE0 || key == 0) {
                key = _getch();
                if (isPaused) return;

                switch (key) {
                case static_cast<int>(Key::UP): rotateBlock(); break;
                case static_cast<int>(Key::LEFT): moveSide(-1); break;
                case static_cast<int>(Key::RIGHT): moveSide(1); break;
                case static_cast<int>(Key::DOWN): moveDown(); break;
                }
            }
            else {
                // P 키 확인 (대소문자 모두)
                if (key == 'c' || key == 'C') {
                    // 1. 이번 턴에 이미 썼다면 무시
                    if (!canHold) return;

                    if (heldBlockType != -1 && heldBlockType == currentBlock.getType()) {
                        return;
                    }

                    // 2. 현재 화면에 있는 블록 지우기 (중요: 안 지우면 잔상 남음)
                    currentBlock.draw(true);
                    drawGhost(true);

                    if (heldBlockType == -1) {
                        // Case A: 킵 된게 없을 때
                        heldBlockType = currentBlock.getType(); // 현재 블록 저장
                        spawnNextBlock(); // 다음 블록을 현재 블록으로 가져옴

                        // spawnNextBlock에서 canHold=true가 되버리므로 다시 잠금
                        canHold = false;
                    }
                    else {
                        // Case B: 킵 된게 있을 때 (서로 교체)
                        int tempType = currentBlock.getType();

                        // 저장된 타입으로 현재 블록 재생성 (위치, 각도 초기화)
                        currentBlock.spawn(heldBlockType);
                        heldBlockType = tempType; // 현재 타입을 저장소로

                        // 교체 후 잠금
                        canHold = false;
                    }

                    // 3. 변경된 상태 화면에 그리기
                    showHoldBlock();    // 왼쪽 홀드창 갱신
                    showNextBlockPreview(); // (Case A일 경우 필요)
                    currentBlock.draw(); // 현재 블록 다시 그리기

                    return; // 입력 처리 완료
                }
                if (key == 'p' || key == 'P') {
                    isPaused = !isPaused; // 상태 반전

                    if (isPaused) {
                        // 일시정지 메시지 출력
                        ConsoleHelper::gotoXY(OFFSET_X + 4, 10);
                        ConsoleHelper::setColor(Color::RED);
                        printf(" PAUSED ");
                    }
                    else {
                        // 게임 화면 복구

                        while (_kbhit()) { _getch(); }
                        board.draw();
                        currentBlock.draw();
                    }
                    return;
                }

                if (isPaused) return; // 일시정지 중이면 다른 일반 키(스페이스바 등) 무시

                // 스페이스바 처리
                if (key == static_cast<int>(Key::SPACE)) {
                    while (moveDown());
                }
            }
        }
    }

    //bool Game::moveDown() {
    //    if (board.checkCollision(currentBlock, currentBlock.x, currentBlock.y + 1, currentBlock.angle)) {
    //        if (currentBlock.y < 0) {
    //            gameOver = true;
    //            return false;
    //        }

    //        board.merge(currentBlock);

    //        int lines = board.processFullLines();
    //        if (lines > 0) {
    //            totalLines += lines;
    //           /* for (int k = 0; k < lines; k++) score += 100 + (level * 10) + (rand() % 10);*/

    //            int baseScore = 0;
    //            switch (lines) {
    //            case 1: baseScore = 10; break;
    //            case 2: baseScore = 30; break;  // 100*2 + 보너스
    //            case 3: baseScore = 50; break;  // 100*3 + 보너스
    //            case 4: baseScore = 80; break;  // '테트리스' (큰 보너스)
    //            default: baseScore = 0; break;
    //            }
    //            // 레벨 가중치 추가
    //            score += baseScore + (lines * level * 20);//line*20에서 점수를 더 주는거
    //            //한줄이면 10, 두줄이면 30, 세줄 : 50, 네줄 : 80

    //            if (totalLines >= stages[level].clearLineGoal) {
    //                totalLines = 0;
    //                if (level < 9) level++;
    //                board.setLevel(level);
    //                board.draw();
    //            }
    //            showStats();
    //        }

    //        spawnNextBlock();
    //        currentBlock.draw();
    //        return false;
    //    }

    //    currentBlock.draw(true);
    //    currentBlock.y++;
    //    currentBlock.draw();
    //    return true;
    //}
    bool Game::moveDown() {
        // 1. 바닥에 닿았는지(충돌) 확인
        if (board.checkCollision(currentBlock, currentBlock.x, currentBlock.y + 1, currentBlock.angle)) {

            // [추가] 블록이 굳기 직전에, 그려져 있던 고스트를 지워줍니다.
            // 안 지우면 바닥에 회색 네모가 남을 수 있습니다.
            drawGhost(true);
            currentBlock.draw();

            if (currentBlock.y < 0) {
                gameOver = true;
                return false;
            }

            board.merge(currentBlock);

            int lines = board.processFullLines();
            if (lines > 0) {
                totalLines += lines;
                for (int k = 0; k < lines; k++) score += 100 + (level * 10) + (rand() % 10);

                if (totalLines >= stages[level].clearLineGoal) {
                    totalLines = 0;
                    if (level < 9) level++;
                    board.setLevel(level);
                    board.draw();
                }
                showStats();
            }

            spawnNextBlock();

            // [추가] spawnNextBlock 안에서 그리겠지만, 혹시 모르니 여기서도 그려줍니다.
            // (spawnNextBlock에 이미 추가했다면 이 줄은 없어도 됩니다)
            drawGhost(false);
            currentBlock.draw();

            return false;
        }

        // 2. 바닥에 안 닿았으면 아래로 이동

        // [추가] 이동하기 전에 '옛날 고스트' 먼저 지우기
        drawGhost(true);

        currentBlock.draw(true); // 옛날 블록 지우기

        currentBlock.y++; // 좌표 이동

        // [추가] 이동한 곳에 '새 고스트' 먼저 그리기
        drawGhost(false);

        currentBlock.draw(); // 새 블록 그리기 (고스트 위에 덮어씀)

        return true;
    }

  /*  void Game::moveSide(int dx) {
        if (!board.checkCollision(currentBlock, currentBlock.x + dx, currentBlock.y, currentBlock.angle)) {
            currentBlock.draw(true);
            currentBlock.x += dx;
            currentBlock.draw();
        }
    }*/
    void Game::moveSide(int dx) {
        if (!board.checkCollision(currentBlock, currentBlock.x + dx, currentBlock.y, currentBlock.angle)) {

            // 1. 지우기 (고스트 먼저, 그다음 블록)
            drawGhost(true);       // 고스트 지움
            currentBlock.draw(true); // 블록 지움

            // 2. 이동
            currentBlock.x += dx;

            // 3. 그리기 (고스트 먼저, 그다음 블록)
            // 고스트를 먼저 그려야 블록이 고스트 위에 겹쳐질 때 블록이 우선순위를 가짐
            drawGhost(false);      // 새 위치에 고스트 그림
            currentBlock.draw();     // 새 위치에 블록 그림
        }
    }

   /* void Game::rotateBlock() {
        int nextAngle = currentBlock.getNextAngle();
        int kickOffsets[] = { 0, -1, 1, -2, 2 };

        for (int offset : kickOffsets) {
            int testX = currentBlock.x + offset;
            if (!board.checkCollision(currentBlock, testX, currentBlock.y, nextAngle)) {
                currentBlock.draw(true);
                currentBlock.angle = nextAngle;
                currentBlock.x = testX;
                currentBlock.draw();
                return;
            }
        }
    }*/
    void Game::rotateBlock() {
        int nextAngle = currentBlock.getNextAngle();
        int kickOffsets[] = { 0, -1, 1, -2, 2 };

        for (int offset : kickOffsets) {
            int testX = currentBlock.x + offset;
            if (!board.checkCollision(currentBlock, testX, currentBlock.y, nextAngle)) {

                // [추가] 회전하기 전에 '옛날 고스트' 지우기
                drawGhost(true);

                currentBlock.draw(true); // 옛날 블록 지우기

                // 상태 변경 (회전 및 위치 보정)
                currentBlock.angle = nextAngle;
                currentBlock.x = testX;

                // [추가] 회전한 상태의 '새 고스트' 먼저 그리기
                drawGhost(false);

                currentBlock.draw(); // 새 블록 그리기
                return;
            }
        }
    }

    void Game::showStats() {
        static const int STAT_X = 35;
        ConsoleHelper::setColor(Color::GRAY);

        ConsoleHelper::gotoXY(STAT_X, 7); printf("STAGE");
        ConsoleHelper::gotoXY(STAT_X + 6, 7); printf("%d", level + 1);

        ConsoleHelper::gotoXY(STAT_X, 9); printf("SCORE");
        ConsoleHelper::gotoXY(STAT_X, 10); printf("%10d", score);

        ConsoleHelper::gotoXY(STAT_X, 12); printf("LINES");
        ConsoleHelper::gotoXY(STAT_X, 13); printf("%10d", stages[level].clearLineGoal - totalLines);
    }

    void Game::showNextBlockPreview() {
        ConsoleHelper::setColor(static_cast<Color>((level + 1) % 6 + 1));
        for (int i = 1; i < 7; i++) {
            ConsoleHelper::gotoXY(33, i);
            for (int j = 0; j < 6; j++) {
                if (i == 1 || i == 6 || j == 0 || j == 5) printf(" ■");
                else printf("  ");
            }
        }

        ShapeBlock temp;
        temp.spawn(nextBlockType);
        temp.x = 16;
        temp.y = 1;

        const auto& shape = ShapeRepository::getShape(nextBlockType)[0];
        ConsoleHelper::setColor(ShapeRepository::getColorForType(nextBlockType));
        for (int r = 0; r < 4; ++r) {
            for (int c = 0; c < 4; ++c) {
                if (shape[c][r] == 1) {
                    ConsoleHelper::gotoXY((r + temp.x) * 2 + OFFSET_X, c + temp.y + OFFSET_Y);
                    printf("■");
                }
            }
        }
    }

    void Game::selectLevel() {
        ConsoleHelper::clear();
        ConsoleHelper::setColor(Color::GRAY);
        ConsoleHelper::gotoXY(10, 7); printf("┏━━━━━━━━━━<GAME KEY>━━━━━━━━┓");
        ConsoleHelper::gotoXY(10, 8); printf("┃ UP   : Rotate Block        ┃");
        ConsoleHelper::gotoXY(10, 9); printf("┃ DOWN : Move One-Step Down  ┃");
        ConsoleHelper::gotoXY(10, 10); printf("┃ SPACE: Move Bottom Down    ┃");
        ConsoleHelper::gotoXY(10, 11); printf("┃ LEFT : Move Left           ┃");
        ConsoleHelper::gotoXY(10, 12); printf("┃ RIGHT: Move Right          ┃");
        ConsoleHelper::gotoXY(10, 13); printf("┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛");

        char buffer[100];
        int selectedLevel = -1;
        while (selectedLevel < 1 || selectedLevel > 8) {
            ConsoleHelper::gotoXY(10, 3);
            printf("Select Start level[1-8]:       \b\b\b\b\b\b\b");

            if (fgets(buffer, 100, stdin) == NULL) continue;
            if (strlen(buffer) == 2 && buffer[0] >= '1' && buffer[0] <= '8') {
                selectedLevel = buffer[0] - '0';
            }
        }
        level = selectedLevel - 1;
    }

    void Game::showLogo() {
        ConsoleHelper::clear();
        ConsoleHelper::gotoXY(21, 3); printf("┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓");
        ConsoleHelper::gotoXY(21, 4); printf("┃ ◆◆◆   ◆◆◆   ◆◆◆   ◆◆◆    ◆   ◆◆◆ ┃");
        ConsoleHelper::gotoXY(21, 5); printf("┃  ◆     ◆       ◆     ◆  ◆   ◆   ◆   ┃");
        ConsoleHelper::gotoXY(21, 6); printf("┃  ◆     ◆◆◆     ◆     ◆◆◆    ◆    ◆  ┃");
        ConsoleHelper::gotoXY(21, 7); printf("┃  ◆     ◆       ◆     ◆ ◆    ◆      ◆ ┃");
        ConsoleHelper::gotoXY(21, 8); printf("┃  ◆     ◆◆◆     ◆     ◆  ◆   ◆   ◆◆◆ ┃");
        ConsoleHelper::gotoXY(21, 9); printf("┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛");

        ConsoleHelper::gotoXY(28, 20);
        printf("Please Press Any Key~!");

        ShapeBlock temp;
        for (int i = 0; ; i++) {
            if (i % 40 == 0) {
                for (int j = 0; j < 5; j++) {
                    ConsoleHelper::gotoXY(19, 14 + j);
                    printf("                                          ");
                }
                int positions[] = { 7, 13, 19, 25 };
                for (int pos : positions) {
                    temp.spawn(rand() % 7);
                    temp.angle = rand() % 4;
                    temp.x = pos;
                    temp.y = 14 - 1;
                    temp.draw();
                }
            }
            if (_kbhit()) break;
            Sleep(30);
        }
        _getche();
    }

    void Game::showGameOver() {
        ConsoleHelper::setColor(Color::RED);
        ConsoleHelper::gotoXY(15, 8);  printf("┏━━━━━━━━━━━━━━━━━━━━━━━━━━┓");
        ConsoleHelper::gotoXY(15, 9);  printf("┃**************************┃");
        ConsoleHelper::gotoXY(15, 10); printf("┃* GAME OVER        *┃");
        ConsoleHelper::gotoXY(15, 11); printf("┃**************************┃");
        ConsoleHelper::gotoXY(15, 12); printf("┗━━━━━━━━━━━━━━━━━━━━━━━━━━┛");
        Sleep(3000);
        system("cls");
    }
   
    void Game::drawGhost(bool erase) {
        // 1. 현재 블록을 복사 (위치, 회전각도, 모양 다 복사됨)
        ShapeBlock ghost = currentBlock;

        // 2. 바닥에 닿을 때까지 내림 (Hard Drop 계산)
        while (!board.checkCollision(ghost, ghost.x, ghost.y + 1, ghost.angle)) {
            ghost.y++;
        }

        // 3. 고스트 그리기 (isGhost = true 로 호출)
        ghost.draw(erase, true);
        //drawGhost(true) => 고스트 지우기
        //drawGhost(false) => 고스트 그리기
    }

    // [Game 클래스 내부 private 함수로 추가]

    void Game::showHoldBlock() {
        // 1. UI 틀 그리기 (왼쪽 상단 좌표: x=2, y=1)
        ConsoleHelper::setColor(Color::GRAY);
        ConsoleHelper::gotoXY(33, 15); printf("┏━━ HOLD ━━┓");
        for (int i = 0; i < 6; i++) {
            ConsoleHelper::gotoXY(33, 16 + i); printf("┃          ┃");
        }
        ConsoleHelper::gotoXY(33, 22); printf("┗━━━━━━━━━━┛");

        // 2. 저장된 블록이 없으면 리턴
        if (heldBlockType == -1) return;

        // 3. 저장된 블록 그리기
        // 좌표 계산: UI 박스 내부 (x=4, y=3 정도)
        int startX = 36;
        int startY = 17;

        const auto& shape = ShapeRepository::getShape(heldBlockType)[0]; // 기본 각도(0)로 표시
        ConsoleHelper::setColor(ShapeRepository::getColorForType(heldBlockType));

        for (int r = 0; r < 4; ++r) {
            for (int c = 0; c < 4; ++c) {
                ConsoleHelper::gotoXY((r * 2) + startX, c + startY);
                if (shape[c][r] == 1) {
                    printf("■");
                }
                else {
                    printf("  ");
                }
            }
        }
    }
}