#include "Game.h"
#include "ConsoleHelper.h"
#include "ShapeRepository.h"
#include <iostream>
#include <conio.h>
#include <ctime>
#include <windows.h> // Sleep
#include <fstream>   // [추가] 파일 저장
#include <cstring>   // [추가] strcspn, strlen 등 사용
#include <string>
#include <iomanip>   // setw, left, right
#include <algorithm>

namespace Tetris {
    Game::Game() 
        : level(0), score(0), totalLines(0), gameOver(false), 
        bombCount(3), savedBlockForBomb(-1)  // [추가] 초기화
    {
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
                    showGameOver();           // 기존 게임 오버 팝업
                    promptNameAndSaveScore(); // [추가] 닉네임 입력 팝업
                    break;
                }

                Sleep(15);
                frameCount++;
            }
        }
    }

    void Game::initStages() {
        stages = {
            {40, 20, 10}, {38, 18, 12}, {35, 18, 15}, {30, 17, 20},
            {25, 16, 20}, {20, 14, 25}, {15, 14, 25}, {10, 13, 30},
            {6, 12, 35},  {4, 11, 99999}
        };
    }//speed/긴 막대기 획득 확률/레벨업 조건
    //레벨마다 클리어 조건 바꿈

    void Game::resetGame() {
        score = 0;
        totalLines = 0;
        gameOver = false;
        // [추가] 게임 리셋 시 폭탄 3개 충전
        bombCount = 3;
        savedBlockForBomb = -1;
    }

    int Game::makeNewBlockType() {
        int rnd = rand() % 100;

        // [추가] 5% 확률로 폭탄(Type 7) 생성
        // if (rnd < 5) return 7;
        // [삭제] 3번 액티브로 변경

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
                        std::cout << " PAUSED ";
                    }
                    else {
                        // 게임 화면 복구

                        while (_kbhit()) { _getch(); }
                        board.draw();
                        currentBlock.draw();
                    }
                    return;
                }
                // [추가] 'b', 'B' 입력 시 폭탄 사용
                if (key == 'b' || key == 'B') {
                    if (!isPaused) useBomb();
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

            // [수정] 폭탄 블록(Type 7)인 경우 폭발, 아니면 일반 병합
            if (currentBlock.getType() == BOMB_TYPE) {
                // 1. 폭탄 폭발
                // 폭탄의 중심 좌표 계산 (Block의 x, y는 4x4 박스의 좌상단)
                // 폭탄 모양이 (1, 1)에 점이 있으므로. 실제 좌표는 x+1, y+1
                int destroyed = board.explode(currentBlock.x + 1, currentBlock.y + 1);

                // 점수 규칙: 블록 비례 보너스(임시)
                if (destroyed > 0) {
                    if (destroyed <= 3)
                        score += destroyed * 30;
                    else if (destroyed <= 6)
                        score += destroyed * 60;
                    else
                        score += destroyed * 90;
                }

                // 2. 저장해뒀던 원래 블록을 다시 소환 (Next 블록을 소모하지 않음)
                if (savedBlockForBomb != -1) {
                    currentBlock.spawn(savedBlockForBomb);
                    savedBlockForBomb = -1; // 저장소 초기화

                    // 소환된 원래 블록 그리기
                    drawGhost(false);
                    currentBlock.draw();
                    return false; // 이번 틱 종료 (바로 내려가지 않음)
                }
            } 
            else 
                board.merge(currentBlock);

            int lines = board.processFullLines();

            // [수정] 줄이 안 지워졌을 때 회색으로 안 바뀜, 직접 보드를 그려줘야 함
            if (lines == 0) 
                board.draw();
            
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

        ConsoleHelper::gotoXY(STAT_X - 1, 8); std::cout << "STAGE";
        ConsoleHelper::setColor(static_cast<Color>((level % 6) + 1));
        ConsoleHelper::gotoXY(STAT_X + 5, 8); std::cout << level + 1;
        ConsoleHelper::setColor(Color::GRAY);

        ConsoleHelper::gotoXY(STAT_X - 1, 10); std::cout << "SCORE";
        ConsoleHelper::gotoXY(STAT_X - 1, 11); std::cout << score;

        // [수정] LINES 를 SCORE 와 같은 행에 배치
        ConsoleHelper::gotoXY(STAT_X + 7, 10); std::cout << "LINES";
        ConsoleHelper::gotoXY(STAT_X + 7, 11); std::cout << stages[level].clearLineGoal - totalLines;

        // [추가] 남은 폭탄 개수 표시
        ConsoleHelper::gotoXY(STAT_X - 1, 20); std::cout << "BOMBS";
        ConsoleHelper::gotoXY(STAT_X - 1, 21);
        ConsoleHelper::setColor(Color::RED);
        for (int i = 0; i < 3; ++i) {
            if (i < bombCount) std::cout << "● ";
            else std::cout << "○ ";
        }
    }

    void Game::showNextBlockPreview() {
        ConsoleHelper::setColor(static_cast<Color>((level + 1) % 6 + 1));
        for (int i = 1; i < 7; i++) {
            ConsoleHelper::gotoXY(33, i);
            for (int j = 0; j < 6; j++) {
                if (i == 1 || i == 6 || j == 0 || j == 5) std::cout << " ■";
                else std::cout << "  ";
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
                    std::cout << "■";
                }
            }
        }
    }

    void Game::selectLevel() {
        ConsoleHelper::clear();
        ConsoleHelper::setColor(Color::GRAY);
        ConsoleHelper::gotoXY(10, 7);  std::cout << "┏━━━━━━━━━<GAME KEY>━━━━━━━━━┓";
        ConsoleHelper::gotoXY(10, 8);  std::cout << "┃ UP   : Rotate Block        ┃";
        ConsoleHelper::gotoXY(10, 9);  std::cout << "┃ DOWN : Move One-Step Down  ┃";
        ConsoleHelper::gotoXY(10, 10); std::cout << "┃ SPACE: Move Bottom Down    ┃";
        ConsoleHelper::gotoXY(10, 11); std::cout << "┃ LEFT : Move Left           ┃";
        ConsoleHelper::gotoXY(10, 12); std::cout << "┃ RIGHT: Move Right          ┃";
        ConsoleHelper::gotoXY(10, 13); std::cout << "┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛";

        // [추가] TOP N 점수판 출력
        int scoreX = 10;
        int scoreY = 16;   // GAME KEY 보다 아래
        showHighScores(HIGH_SCORE_LIMIT, scoreX, scoreY);
        ConsoleHelper::setColor(Color::GRAY);

        // 이전 키 입력/엔터 싹 비우기
        ConsoleHelper::flushInput();

        std::string input;
        int selectedLevel = -1;

        // 레벨 입력 동안 커서 보이게
        ConsoleHelper::cursorVisible(true);

        while (selectedLevel < 1 || selectedLevel > 8) {
            ConsoleHelper::setColor(Color::GRAY);
            ConsoleHelper::gotoXY(10, 3);
            std::cout << "Select Start level[1-8]: ";

            // 이전 입력 자국 지우기
            ConsoleHelper::setColor(Color::GRAY);
            ConsoleHelper::gotoXY(10 + 26, 3);  // "Select Start level [1-8]: " 길이 기준
            std::cout << "                                                                         ";
            ConsoleHelper::gotoXY(10 + 26, 3);

            // 실제 입력
            if (!std::getline(std::cin, input) || input.empty()) {
                // 입력 실패시 다시 시도
                ConsoleHelper::flushInput();
                continue;
            }
            
            if (input.size() == 1 && input.at(0) >= '1' && input.at(0) <= '8') {
                selectedLevel = input.at(0) - '0';
            }
            else {
                // 잘못된 입력에 대해서 안내 문구 표시
                ConsoleHelper::gotoXY(10, 4);
                ConsoleHelper::setColor(Color::RED);
                std::cout << "Please enter a number between 1 and 8.";
                ConsoleHelper::setColor(Color::GRAY);
            }
        }
        // 레벨 확정 후 커서 다시 숨기기
        ConsoleHelper::cursorVisible(false);

        level = selectedLevel - 1;
    }

    void Game::showLogo() {
        ConsoleHelper::cursorVisible(false);

        int startX = 15;  // 왼쪽 여백
        int startY = 3;  // 시작 높이 (아트가 커서 조금 위로 올림)

        // ==========================================================
        // 1. "BOMB!" 아트 출력 (빨간색)
        // ==========================================================
        ConsoleHelper::setColor(Color::RED);

        // 백슬래시(\)를 모두 이중(\\)으로 처리했습니다.
        std::string bombArt[6] = {
            "__________              ___.  ._. ",
            "\\______   \\ ____   _____\\_ |__| |",
            " |    |  _//  _ \\ /     \\| __ \\ |",
            " |    |   (  <_> )  Y Y  \\ \\_\\ \\|",
            " |______  /\\____/|__|_|  /___  /_ ",
            "        \\/             \\/    \\/\\/"
        };

        for (int i = 0; i < 6; i++) {
            // 중앙 정렬을 위해 약간 오른쪽(startX + 12)에서 출력
            ConsoleHelper::gotoXY(startX + 12, startY + i);
            std::cout << bombArt[i];
        }

        // ==========================================================
        // 2. "TETRIS" 아트 출력 (한 줄씩 색상 변경 효과)
        // ==========================================================
        int textY = startY + 7; // BOMB 아래쪽

        // 무지개 색상 배열
        Color lineColors[6] = { Color::RED, Color::YELLOW, Color::GREEN, Color::SKY_BLUE, Color::BLUE, Color::VIOLET };
        std::string tetrisArt[6] = {
            "____________________________________________.___  _________",
            "\\__    ___/\\_    _____/\\__    ___/\\______    \\  |/   _____/",
            "  |    |    |    __)_    |    |    |       _/   |\\_____  \\ ",
            "  |    |    |        \\   |    |    |    |   \\   |/        \\",
            "  |____|   /_______  /   |____|    |____|_  /___/_______  /",
            "                   \\/                     \\/            \\/ "
        };

        for (int i = 0; i < 6; i++) {
            // 한 줄마다 색깔을 다르게 해서 레트로한 느낌 주기
            ConsoleHelper::setColor(lineColors[i]);
            ConsoleHelper::gotoXY(startX, textY + i);
            std::cout << tetrisArt[i];
        }

        // ==========================================================
        // 3. 안내 문구 및 대기 애니메이션
        // ==========================================================

        int animBaseY = textY + 12; // 애니메이션 위치

        int tick = 0;               // [추가] 반복 횟수 세는 변수
        bool showText = false;      // [추가] 텍스트 깜빡임

        while (!_kbhit()) {
            // [1] 글씨 깜빡임 처리 (느린 박자)
            // 10번 돌 때마다(약 0.5초) 상태 변경
            if (tick % 10 == 0) {
                showText = !showText;
            }

            // 깜빡임 로직
            ConsoleHelper::gotoXY(startX + 14, textY + 8); // 중앙 정렬 위치 조정
            if (showText) {
                ConsoleHelper::setColor(Color::WHITE);
                std::cout << "[ Please Press Any Key to start ]";
                ConsoleHelper::setColor(Color::WHITE);
                // 잔상 지우기 (너비를 넉넉하게 70칸 정도로 잡음)
                ConsoleHelper::setColor(Color::BLACK);
            }
            else
                std::cout << "                                                ";

            if (tick % 25 == 0) {
                // 잔상 지우기
                ConsoleHelper::setColor(Color::BLACK);
                for (int y = animBaseY - 1; y < animBaseY + 6; ++y) {
                    ConsoleHelper::gotoXY(startX, y);
                    std::cout << "                                                                      ";
                }

                // 장식용 랜덤 블록 그리기
                for (int k = 0; k < 4; ++k) {
                    int rType = rand() % 7;
                    int rX = 20 + k * 15;
                    int rY = animBaseY;

                    const auto& shape = ShapeRepository::getShape(rType)[0];
                    ConsoleHelper::setColor(ShapeRepository::getColorForType(rType));

                    for (int i = 0; i < 4; ++i) {
                        for (int j = 0; j < 4; ++j) {
                            if (shape[i][j]) {
                                ConsoleHelper::gotoXY(rX + j * 2, rY + i);
                                std::cout << "■";
                            }
                        }
                    }
                }
            }

            tick = (tick + 1) % 50;
            Sleep(50);
        }

        _getch();
        ConsoleHelper::clear();
    }

    void Game::showGameOver() {
        ConsoleHelper::setColor(Color::RED);
        ConsoleHelper::gotoXY(15, 8);  std::cout << "┏━━━━━━━━━━━━━━━━━━━━━━━━━━┓";
        ConsoleHelper::gotoXY(15, 9);  std::cout << "┃**************************┃";
        ConsoleHelper::gotoXY(15, 10); std::cout << "┃*       GAME OVER        *┃";
        ConsoleHelper::gotoXY(15, 11); std::cout << "┃**************************┃";
        ConsoleHelper::gotoXY(15, 12); std::cout << "┗━━━━━━━━━━━━━━━━━━━━━━━━━━┛";
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
        ConsoleHelper::gotoXY(34, 13); std::cout << "┏━━ HOLD ━━┓";
        for (int i = 0; i < 4; i++) {
            ConsoleHelper::gotoXY(34, 14 + i); std::cout << "┃          ┃";
        }
        ConsoleHelper::gotoXY(34, 18); std::cout << "┗━━━━━━━━━━┛";

        // 2. 저장된 블록이 없으면 리턴
        if (heldBlockType == -1) return;

        // 3. 저장된 블록 그리기
        // 좌표 계산: UI 박스 내부 (x=4, y=3 정도)
        int startX = 37;
        int startY = 14;

        const auto& shape = ShapeRepository::getShape(heldBlockType)[0]; // 기본 각도(0)로 표시
        ConsoleHelper::setColor(ShapeRepository::getColorForType(heldBlockType));

        for (int r = 0; r < 4; ++r) {
            for (int c = 0; c < 4; ++c) {
                ConsoleHelper::gotoXY((r * 2) + startX, c + startY);
                if (shape[c][r] == 1) {
                    std::cout << "■";
                }
                else {
                    std::cout << "  ";
                }
            }
        }
    }

    void Game::promptNameAndSaveScore() {
        ConsoleHelper::clear();              // 화면 싹 지우고
        ConsoleHelper::setColor(Color::GRAY);

        int startX = 15;
        int startY = 4;

        // 간단한 팝업 박스 (ASCII로)
        ConsoleHelper::gotoXY(startX, startY);     std::cout << "┏━━━━━━━━━━━━━━━━━━━━━━━━━━━┓";
        ConsoleHelper::gotoXY(startX, startY + 1); std::cout << "┃        SCORE RECORD       ┃";
        ConsoleHelper::gotoXY(startX, startY + 2); std::cout << "┃                           ┃";
        ConsoleHelper::gotoXY(startX, startY + 3); std::cout << "┃  Name :                   ┃";
        ConsoleHelper::gotoXY(startX, startY + 4); std::cout << "┃                           ┃";
        ConsoleHelper::gotoXY(startX, startY + 5); std::cout << "┃  Score: " << score;
        int padding = 18 - std::to_string(score).size();
        for (int i = 0; i < padding; i++) std::cout << " ";
        std::cout << "┃";
        ConsoleHelper::gotoXY(startX, startY + 6); std::cout << "┗━━━━━━━━━━━━━━━━━━━━━━━━━━━┛";

        // [추가] 점수 출력
        int scoreX = startX;
        int scoreY = startY + 10;

        showHighScores(HIGH_SCORE_LIMIT, scoreX, scoreY);
        ConsoleHelper::setColor(Color::GRAY);

        // 입력 위치로 커서 옮기기 (“Name : ” 뒤)
        ConsoleHelper::gotoXY(startX + 10, startY + 3);


        // 이전 입력 비우기
        ConsoleHelper::flushInput();

        // 닉네임 입력 시 커서 보이기
        ConsoleHelper::cursorVisible(true);

        std::string name;
        if (!std::getline(std::cin, name) || name.empty()) {
            name = "NONAME";
        }

        //입력 끝났으니 커서 다시 숨김
        ConsoleHelper::cursorVisible(false);

        // 텍스트 파일에 append
        // 실행 파일이 있는 폴더에 score.txt가 계속 쌓이는 형태
        std::ofstream out("score.txt", std::ios::app);
        if (out.is_open()) {
            // 예: HEOEUN<tab>12340 이런 형식으로 저장, 이름과 점수를 구별하는 구별자를 탭으로 함
            out << name << "\t" << score << "\n";
            out.close();
        }

        // [추가] 점수 저장하고 등수 최신화 해서 다시 출력
        showHighScores(HIGH_SCORE_LIMIT, scoreX, scoreY, &name, score);

        // 저장 후 간단히 안내 멘트
        ConsoleHelper::gotoXY(startX, startY + 8);
        ConsoleHelper::setColor(Color::GREEN);
        std::cout << "Saved! Press any key to restart...";
        ConsoleHelper::setColor(Color::BLACK);

        _getch(); // 아무 키나 누르면 다음 게임으로
    }

    void Game::showHighScores(int maxCount, int startX, int startY,
        const std::string* curName, int curScore) {
        std::ifstream in("score.txt");
        if (!in.is_open()) {
            ConsoleHelper::gotoXY(startX, startY);
            ConsoleHelper::setColor(Color::GRAY);
            std::cout << "No scores yet.";
            ConsoleHelper::setColor(Color::BLACK);
            return;
        }

        struct HighScore {
            std::string name;
            int score;
        };

        std::vector<HighScore> scores;
        std::string line;

        while (std::getline(in, line)) {
            if (line.empty()) continue;

            // name \t score 형식 가정
            size_t tabPos = line.rfind('\t');
            if (tabPos == std::string::npos) continue;

            std::string name = line.substr(0, tabPos);
            std::string scoreStr = line.substr(tabPos + 1);

            try {
                int s = std::stoi(scoreStr);
                scores.push_back({ name, s });
            }
            catch (...) {
                // 숫자 파싱 안 되면 스킵
                continue;
            }
        }

        in.close();

        if (scores.empty()) {
            ConsoleHelper::gotoXY(startX, startY);
            ConsoleHelper::setColor(Color::GRAY);
            std::cout << "No scores yet.";
            ConsoleHelper::setColor(Color::BLACK);
            return;
        }

        // 점수 내림차순 정렬
        sort(scores.begin(), scores.end(),
            [](const HighScore& a, const HighScore& b) {
                return a.score > b.score;
            });

        int count = maxCount;
        if (count > static_cast<int>(scores.size())) {
            count = static_cast<int>(scores.size());
        }

        // 헤더 출력
        ConsoleHelper::setColor(Color::WHITE);
        ConsoleHelper::gotoXY(startX, startY);
        std::cout << "RANK  NAME               SCORE";

        bool marked = false; // NEW! 는 한 줄만 표시

        // 내용 출력
        ConsoleHelper::setColor(Color::GRAY);
        for (int i = 0; i < count; ++i) {
            ConsoleHelper::gotoXY(startX, startY + 1 + i);

            // 등수에 따라 색 결정
            if (i == 0) {
                ConsoleHelper::setColor(Color::DARK_YELLOW);  // 1등
            }
            else if (i == 1) {
                ConsoleHelper::setColor(Color::WHITE);        // 2등
            }
            else if (i == 2) {
                ConsoleHelper::setColor(Color::GRAY);         // 3등
            }
            else {
                ConsoleHelper::setColor(Color::DARK_GRAY);    // 나머지
            }

            if (i + 1 < 10)
                std::cout << (i + 1) << ". " << std::setw(3) << " ";
            else
                std::cout << std::setw(2) << (i + 1) << "." << std::setw(3) << " ";

            std::string n = scores[i].name;
            if (n.size() > 15) 
                n = n.substr(0, 15);   // 너무 긴 이름은 잘라버리기
            
            std::cout << std::left << std::setw(17) << n;       // 이름 왼쪽 정렬
            std::cout << std::right << std::setw(7) << scores[i].score; // 점수 오른쪽 정렬

            bool isNew = (curName != nullptr && !marked &&
                scores[i].name == *curName && scores[i].score == curScore);
            if (isNew) {
                std::cout << "  <- NEW!";
                marked = true;
            }
        }

        ConsoleHelper::setColor(Color::BLACK);
    }

    // [추가] 폭탄 스킬 사용 함수
    void Game::useBomb() {
        // 1. 폭탄이 남아있고, 현재 이미 폭탄이 아닌 경우에만 사용 가능
        if (bombCount > 0 && currentBlock.getType() != BOMB_TYPE) {

            // 2. 현재 블록 지우기 (화면 갱신)
            drawGhost(true);
            currentBlock.draw(true);

            // 3. 현재 블록 정보 저장 (타입만 저장)
            savedBlockForBomb = currentBlock.getType();

            // 4. 현재 블록을 폭탄으로 교체
            currentBlock.spawn(BOMB_TYPE);

            // 5. 갯수 차감 및 UI 갱신
            bombCount--;
            showStats();

            // 6. 폭탄 그리기
            drawGhost(false);
            currentBlock.draw();
        }
    }
}