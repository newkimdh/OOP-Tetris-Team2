#include "Game.h"
#include "ConsoleHelper.h"
#include "ShapeRepository.h"
#include <iostream>
#include <iomanip>   // setw               (점수, 레벨 출력용)
#include <string>    // string             (문자열 처리용)
#include <conio.h>   // _getch, _kbhit     (입력 대기용)
#include <vector>    // vector             (랭킹 관리용)
#include <fstream>   // ifstream, ofstream (파일 입출력용)
#include <algorithm> // sort               (랭킹 정렬용)
#include <windows.h> // Sleep              (대기 애니메이션용)

/*
 * 파일명: Game_UI.cpp
 * 설명: Tetris 게임의 화면 출력(UI)을 담당하는 파일입니다.
 * 로고, 메뉴, 팝업창, 점수판 등 시각적인 요소들을 그리는 함수들이 모여 있습니다.
 * (게임 로직은 Game.cpp에 있습니다.)

  ===========================================================================
    [Game_UI.cpp 함수 목차 (Function Index)]

    1. Game::showLogo (인트로) ................ Line 37
    2. Game::selectLevel (레벨 선택) .......... Line 146
    3. Game::showStats (상태창) ............... Line 230
    4. Game::showNextBlockPreview (다음 블록).. Line 297
    5. Game::showHoldBlock (홀드 블록) ........ Line 339
    6. Game::drawGhost (고스트) ............... Line 378
    7. Game::showGameOver (게임 오버) ......... Line 396
    8. Game::promptNameAndSaveScore (점수 저장) Line 499
    9. Game::showHighScores (랭킹 출력) ....... Line 612
   10. Game::drawPausePopup (일시 정지) ....... Line 727
   11. Game::showGoodBye (종료 화면) .......... Line 805
  ===========================================================================
  */

namespace Tetris {
    /*
     * [함수: showLogo]
     * 설명: 게임 시작 시 인트로 화면을 출력합니다.
     * "BOMB TETRIS" 로고와 함께 블록이 움직이는 애니메이션을 보여주며,
     * 아무 키나 누를 때까지 대기합니다.
     */
    void Game::showLogo() {
        ConsoleHelper::cursorVisible(false);

        int startX = 28;  // 왼쪽 여백 (화면 중앙 정렬 기준)
        int startY = 3;   // 상단 여백

        // 1. "BOMB!" 아트 출력 (빨간색)
        ConsoleHelper::setColor(Color::RED);

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
            ConsoleHelper::gotoXY(startX + 13, startY + i);
            std::cout << bombArt[i];
        }

        // 2. "TETRIS" 아트 출력 (한 줄씩 색상 변경 효과)
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
            ConsoleHelper::setColor(lineColors[i]);
            ConsoleHelper::gotoXY(startX, textY + i);
            std::cout << tetrisArt[i];
        }

        // 3. 안내 문구 및 대기 애니메이션
        int animBaseY = textY + 12; // 애니메이션 위치
        int tick = 0;               // 반복 횟수 세는 변수
        bool showText = false;      // 텍스트 깜빡임

        while (!_kbhit()) {
            // 글씨 깜빡임 처리 (약 0.5초 간격)
            if (tick % 10 == 0) {
                showText = !showText;
            }

            // 깜빡임 로직
            ConsoleHelper::gotoXY(startX + 15, textY + 9); // 중앙 정렬 위치 조정
            if (showText) {
                ConsoleHelper::setColor(Color::WHITE);
                std::cout << "[ Please Press Any Key to Start ]";
                ConsoleHelper::setColor(Color::WHITE);
                ConsoleHelper::setColor(Color::BLACK);
            }
            else
                std::cout << "                                                ";

            // 배경 장식용 랜덤 블록 애니메이션
            if (tick % 25 == 0) {
                // 잔상 지우기
                ConsoleHelper::setColor(Color::BLACK);
                for (int y = animBaseY - 1; y < animBaseY + 6; ++y) {
                    ConsoleHelper::gotoXY(startX, y);
                    std::cout << "                                                                      ";
                }

                // 랜덤 블록 그리기
                for (int k = 0; k < 4; ++k) {
                    int rType = rand() % 7;
                    int rX = startX + 5 + k * 16;
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

        _getch();   // 키 입력 버퍼 비우기
        ConsoleHelper::clear();
    }

    /*
     * [함수: selectLevel]
     * 설명: 게임 시작 전 레벨을 선택하는 화면을 출력합니다.
     * 조작 키 설명(GAME KEY)과 현재 랭킹(TOP SCORE)을 보여줍니다.
     * 1~8 사이의 숫자를 입력받아 레벨을 설정하거나, Q를 눌러 종료합니다.
     */
    void Game::selectLevel() {
        ConsoleHelper::clear();

        // 좌표 계산
        int boxX = 42;
        int boxY = 6;

        ConsoleHelper::setColor(Color::GRAY);

        // 1. GAME KEY 설명 박스 (높이 1칸 늘림)
        ConsoleHelper::gotoXY(boxX - 5, boxY);     std::cout << "┏━━━━━━━━━━━━━━━━━━━ <GAME KEY> ━━━━━━━━━━━━━━━━━┓";
        ConsoleHelper::gotoXY(boxX - 5, boxY + 1); std::cout << "┃             UP    : Rotate Block               ┃";
        ConsoleHelper::gotoXY(boxX - 5, boxY + 2); std::cout << "┃             DOWN  : Move One-Step Down         ┃";
        ConsoleHelper::gotoXY(boxX - 5, boxY + 3); std::cout << "┃             SPACE : Move Bottom Down           ┃";
        ConsoleHelper::gotoXY(boxX - 5, boxY + 4); std::cout << "┃             LEFT  : Move Left                  ┃";
        ConsoleHelper::gotoXY(boxX - 5, boxY + 5); std::cout << "┃             RIGHT : Move Right                 ┃";
        ConsoleHelper::gotoXY(boxX - 5, boxY + 6); std::cout << "┃             B     : Bomb Skill                 ┃";
        ConsoleHelper::gotoXY(boxX - 5, boxY + 7); std::cout << "┃             P     : Pause / Menu               ┃";
        ConsoleHelper::gotoXY(boxX - 5, boxY + 8); std::cout << "┃             Q     : Quit Game                  ┃";
        ConsoleHelper::gotoXY(boxX - 5, boxY + 9); std::cout << "┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛";

        // 2. 랭킹 점수판 출력
        int scoreX = boxX + 5;
        int scoreY = boxY + 12; // 박스가 커졌으니 점수판도 한 칸 아래로(11 -> 12)
        showHighScores(HIGH_SCORE_LIMIT, scoreX, scoreY);
        ConsoleHelper::setColor(Color::GRAY);

        // 3. 레벨 입력 루프
        ConsoleHelper::flushInput();
        std::string input;
        int selectedLevel = -1;

        // 안내 문구
        int inputX = 42; // "Select Level [1-8] or [Q]uit: " 길이 고려해서 조정
        int inputY = 3;

        ConsoleHelper::cursorVisible(true);

        while (true) { // 무한 루프로 변경하여 유효 입력까지 대기
            ConsoleHelper::setColor(Color::GRAY);
            ConsoleHelper::gotoXY(inputX, inputY);
            std::cout << "Select Level [1-8] or [Q]uit: ";

            // 입력창 초기화
            ConsoleHelper::setColor(Color::GRAY);
            ConsoleHelper::gotoXY(inputX + 30, inputY);
            std::cout << "          ";
            ConsoleHelper::gotoXY(inputX + 30, inputY);

            // 입력 받기
            if (!std::getline(std::cin, input) || input.empty()) {
                ConsoleHelper::flushInput();
                continue;
            }

            // [종료 체크] Q 또는 q 입력 시
            if (input == "q" || input == "Q") {
                level = -1; // 종료 신호로 -1 사용
                break;
            }

            // [레벨 체크] 1~8 숫자 입력 시
            if (input.size() == 1 && input.at(0) >= '1' && input.at(0) <= '8') {
                selectedLevel = input.at(0) - '0';
                level = selectedLevel - 1; // 실제 레벨(0~7) 설정
                break;
            }

            // 잘못된 입력
            ConsoleHelper::gotoXY(41, inputY + 1);
            ConsoleHelper::setColor(Color::RED);
            std::cout << "Please enter 1-8 or Q to quit.";
            ConsoleHelper::setColor(Color::GRAY);
        }

        ConsoleHelper::cursorVisible(false);
    }

    /*
     * [함수: showStats]
     * 설명: 인게임 화면 우측에 현재 스테이지 정보, 점수, 남은 줄 수,
     * 그리고 보유 중인 폭탄 개수를 박스 형태로 출력합니다.
     */
    void Game::showStats() {
        static const int STAT_X = 77;

        int statsY = 8;     // 시작 Y좌표
        int boxWidth = 24;  // 박스 너비 (내용물에 맞춰 넉넉하게)
        int boxHeight = 7;  // 높이

        ConsoleHelper::setColor(Color::GRAY);

        // 1. STATS 테두리 그리기
        ConsoleHelper::gotoXY(STAT_X, statsY);
        std::cout << "┏━━━━━━ STAT ━━━━━━┓";
        for (int i = 1; i < boxHeight; ++i) {
            ConsoleHelper::gotoXY(STAT_X, statsY + i);
            std::cout << "┃                  ┃";
        }
        ConsoleHelper::gotoXY(STAT_X, statsY + boxHeight);
        std::cout << "┗━━━━━━━━━━━━━━━━━━┛";

        // 정보 출력 (STAGE, SCORE, LINES)
        ConsoleHelper::gotoXY(STAT_X + 3, statsY + 2);
        ConsoleHelper::setColor(Color::GRAY);
        std::cout << "STAGE";

        ConsoleHelper::gotoXY(STAT_X + 11, statsY + 2);
        ConsoleHelper::setColor(static_cast<Color>((level % 6) + 1));
        std::cout << std::setw(2) << level + 1; // 2자리수 정렬

        ConsoleHelper::setColor(Color::GRAY);
        ConsoleHelper::gotoXY(STAT_X + 3, statsY + 4);
        std::cout << "SCORE    LINES";

        ConsoleHelper::setColor(Color::WHITE);
        ConsoleHelper::gotoXY(STAT_X + 3, statsY + 5);
        std::cout << std::setw(5) << score;

        ConsoleHelper::gotoXY(STAT_X + 12, statsY + 5);
        std::cout << std::setw(5) << (stages[level].clearLineGoal - totalLines) << "  "; // 잔상 제거용 공백

        // 2. BOMB 박스 (폭탄 스킬) 테두리 및 아이콘
        int bombY = statsY + boxHeight + 2; // STATS 박스 한 칸 아래 (Y=16)
        int bombHeight = 4;                 // 높이

        ConsoleHelper::setColor(Color::GRAY);
        ConsoleHelper::gotoXY(STAT_X, bombY);
        std::cout << "┏━━ BOMBS ━━┓";
        for (int i = 1; i < bombHeight - 2; ++i) {
            ConsoleHelper::gotoXY(STAT_X, bombY + i);
            std::cout << "┃           ┃";
        }
        ConsoleHelper::gotoXY(STAT_X, bombY + bombHeight - 2);
        std::cout << "┗━━━━━━━━━━━┛";

        // 정보 출력
        ConsoleHelper::gotoXY(STAT_X + 4, bombY + 1);
        ConsoleHelper::setColor(Color::RED);
        for (int i = 0; i < 3; ++i) {
            if (i < bombCount) std::cout << "● ";
            else std::cout << "○ ";
        }
    }

    /*
     * [함수: showNextBlockPreview]
     * 설명: 우측 상단에 다음에 나올 블록(Next Block)을 미리 보여줍니다.
     */
    void Game::showNextBlockPreview() {
        int startX = 77;
        int startY = 1;

        // 1. UI 틀 그리기 (NEXT 스타일)
        ConsoleHelper::setColor(Color::GRAY);
        ConsoleHelper::gotoXY(startX, startY);     std::cout << "┏━━ NEXT ━━┓";
        for (int i = 0; i < 4; i++) {
            ConsoleHelper::gotoXY(startX, startY + 1 + i); std::cout << "┃          ┃";
        }
        ConsoleHelper::gotoXY(startX, startY + 5); std::cout << "┗━━━━━━━━━━┛";

        // 2. 블록 그리기
        // 박스 내부 좌표 계산 (틀 두께 고려)
        int blockDrawX = startX + 3; // 박스 안쪽 X
        int blockDrawY = startY + 1; // 박스 안쪽 Y

        ShapeBlock temp;
        temp.spawn(nextBlockType);  // 미리보기용 임시 블록 생성

        // 블록 모양 데이터 가져오기
        const auto& shape = ShapeRepository::getShape(nextBlockType)[0];
        ConsoleHelper::setColor(ShapeRepository::getColorForType(nextBlockType));

        for (int r = 0; r < 4; ++r) {
            for (int c = 0; c < 4; ++c) {
                // 박스 내부 좌표 기준으로 그리기
                ConsoleHelper::gotoXY((r * 2) + blockDrawX, c + blockDrawY);
                if (shape[c][r] == 1) {
                    std::cout << "■";
                }
                else {
                    std::cout << "  ";
                }
            }
        }
    }

    /*
     * [함수: showHoldBlock]
     * 설명: 좌측 상단에 현재 홀드(Hold) 중인 블록을 표시합니다.
     */
    void Game::showHoldBlock() {
        int startX = 32;
        int startY = 1;

        // 1. UI 틀 그리기
        ConsoleHelper::setColor(Color::GRAY);
        ConsoleHelper::gotoXY(startX, startY);     std::cout << "┏━━ HOLD ━━┓";
        for (int i = 0; i < 4; i++) {
            ConsoleHelper::gotoXY(startX, startY + 1 + i); std::cout << "┃          ┃";
        }
        ConsoleHelper::gotoXY(startX, startY + 5); std::cout << "┗━━━━━━━━━━┛";

        // 2. 저장된 블록이 없으면 리턴
        if (heldBlockType == -1) return;

        // 3. 저장된 블록 그리기
        int blockDrawX = startX + 3;
        int blockDrawY = startY + 1;

        const auto& shape = ShapeRepository::getShape(heldBlockType)[0];
        ConsoleHelper::setColor(ShapeRepository::getColorForType(heldBlockType));

        for (int r = 0; r < 4; ++r) {
            for (int c = 0; c < 4; ++c) {
                ConsoleHelper::gotoXY((r * 2) + blockDrawX, c + blockDrawY);
                if (shape[c][r] == 1) {
                    std::cout << "■";
                }
                else {
                    std::cout << "  ";
                }
            }
        }
    }

    /*
     * [함수: drawGhost]
     * 설명: 현재 조작 중인 블록이 바닥에 떨어질 위치(고스트)를 계산하고 그립니다.
     * erase가 true면 고스트를 지우고, false면 고스트를 그립니다.
     */
    void Game::drawGhost(bool erase) {
        // 1. 현재 블록을 복사 (위치, 회전각도, 모양 다 복사됨)
        ShapeBlock ghost = currentBlock;

        // 2. 바닥에 닿을 때까지 내림 (Hard Drop 계산)
        while (!board.checkCollision(ghost, ghost.x, ghost.y + 1, ghost.angle)) {
            ghost.y++;
        }

        // 3. 고스트 그리기 (isGhost = true 로 호출)
        ghost.draw(erase, true);
    }

    /*
     * [함수: showGameOver]
     * 설명: 게임 오버 시 팝업을 띄우고 사용자의 확인(Enter/Space)을 기다립니다.
     */
    void Game::showGameOver() {
        // 1. GAME OVER 아트 정의
        std::string gameOverArt[6] = {
            "  ________                       ________                     ._.",
            " /  _____/_____    _____   ____   \\_____ \\___   __ ___________| |",
            "/   \\  ___\\__  \\  /     \\_/ __ \\   /   |   \\  \\/ // __ \\_  __ \\ |",
            "\\    \\_\\  \\/ __ \\|  Y Y  \\  ___/  /    |    \\   /\\  ___/|  | \\/\\|",
            " \\______  (____  /__|_|  /\\___  > \\_______  /\\_/  \\___  >__|   __",
            "        \\/     \\/      \\/     \\/          \\/          \\/       \\/"
        };

        int boxWidth = 78;
        int boxHeight = 12;

        // 위치 조정
        int startX = 21;
        int startY = 9;

        // 3. 배경 지우기 (검은색으로 채움)
        ConsoleHelper::setColor(Color::BLACK);
        for (int y = 0; y < boxHeight; ++y) {
            ConsoleHelper::gotoXY(startX, startY + y);
            // 공백으로 채워 뒤의 게임 화면 가리기
            for (int x = 0; x < boxWidth; ++x) {
                std::cout << " ";
            }
        }

        // 4. 테두리 그리기
        ConsoleHelper::setColor(Color::WHITE);
        ConsoleHelper::gotoXY(startX, startY);
        std::cout << "┏";
        for (int i = 0; i <= (boxWidth - 4); ++i) std::cout << "━";
        std::cout << "┓";

        for (int i = 1; i < boxHeight - 1; ++i) {
            ConsoleHelper::gotoXY(startX, startY + i);
            std::cout << "┃";
            // 오른쪽 벽 위치: startX + 전체너비 - 2칸(두께)
            ConsoleHelper::gotoXY(startX + boxWidth - 2, startY + i);
            std::cout << "┃";
        }

        ConsoleHelper::gotoXY(startX, startY + boxHeight - 1);
        std::cout << "┗";
        for (int i = 0; i <= (boxWidth - 4); ++i) std::cout << "━";
        std::cout << "┛";

        // 5. 아트 출력 (테두리 안쪽에 배치)
        ConsoleHelper::setColor(Color::WHITE);
        for (int i = 0; i < 6; ++i) {
            ConsoleHelper::gotoXY(startX + 4, startY + 2 + i);
            std::cout << gameOverArt[i];
        }

        // 6. 안내 메시지 변수 설정
        ConsoleHelper::setColor(Color::GRAY);
        std::string msg = "[ Press Enter to Continue ]";

        // 메시지 중앙 좌표 계산
        int msgX = startX + (boxWidth - msg.length()) / 2;
        int msgY = startY + 9;

        // 깜빡임 제어 변수
        int tick = 0;
        bool showText = true;

        // 7. 루프: 키 입력 대기 + 깜빡임 애니메이션
        while (true) {
            // (1) 키 입력 확인 (Non-blocking)
            if (_kbhit()) {
                int key = _getch();
                if (key == 13) { // Enter
                    break;       // 루프 탈출 -> 게임 종료 처리로 넘어감
                }
            }

            // (2) 깜빡임 로직 (10틱 = 0.5초 주기)
            if (tick % 10 == 0) {
                ConsoleHelper::gotoXY(msgX, msgY);

                if (showText) {
                    // 글씨 보이기
                    ConsoleHelper::setColor(Color::WHITE);
                    std::cout << msg;
                }
                else {
                    // 글씨 지우기 (공백으로 덮어쓰기)
                    // 메시지 길이만큼 공백 출력
                    for (int i = 0; i < msg.length(); ++i) std::cout << " ";
                }
                showText = !showText; // 상태 반전 (보임 <-> 안보임)
            }

            Sleep(50); // 0.05초 대기
            tick = (tick + 1) % 30; // 오버플로우 방지
        }
    }

    /*
     * [함수: promptNameAndSaveScore]
     * 설명: 게임 종료 후 닉네임을 입력받는 팝업을 띄우고, 점수를 파일에 저장합니다.
     * 그 후 저장된 랭킹을 다시 보여줍니다.
     */
    void Game::promptNameAndSaveScore() {
        ConsoleHelper::clear();
        ConsoleHelper::setColor(Color::GRAY);

        int startX = 42;
        int startY = 4;

        // 팝업 박스 그리기
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

        // 점수 출력
        int scoreX = startX;
        int scoreY = startY + 10;
        showHighScores(HIGH_SCORE_LIMIT, scoreX, scoreY);
        ConsoleHelper::setColor(Color::GRAY);

        // 닉네임 입력 로직 (최대 16글자 제한)
        ConsoleHelper::gotoXY(startX + 10, startY + 3);
        ConsoleHelper::flushInput();
        ConsoleHelper::cursorVisible(true); // 닉네임 입력 시 커서 보이기

        std::string name;
        int maxLength = 16;
        while (true) {
            int key = _getch();

            // 1. 엔터 키 (입력 완료)
            if (key == 13) {
                if (name.empty()) name = "NONAME"; // 빈칸이면 기본값
                break;
            }
            // 2. 백스페이스 (지우기)
            else if (key == 8) {
                if (!name.empty()) {
                    name.pop_back();      // 글자 삭제
                    std::cout << "\b \b"; // 화면에서도 지우기 (뒤로가서 공백찍고 다시 뒤로)
                }
            }
            // 3. 일반 문자 (영어, 숫자 등)
            // 특수문자는 처리가 복잡하므로 여기선 제외하거나 단순 처리
            else if (name.length() < maxLength && key >= 32 && key <= 126) {
                name += (char)key;
                std::cout << (char)key;
            }
        }

        // 입력 끝났으니 커서 다시 숨김
        ConsoleHelper::cursorVisible(false);

        // 파일 저장
        std::ofstream out("score.txt", std::ios::app);
        if (out.is_open()) {
            // 예: HEOEUN<tab>12340 이런 형식으로 저장, 이름과 점수를 구별하는 구별자를 탭으로 함
            out << name << "\t" << score << "\n";
            out.close();
        }

        // 랭킹 재출력 및 완료 메시지
        showHighScores(HIGH_SCORE_LIMIT, scoreX, scoreY, &name, score);

        std::string savedMsg = "[ Saved! Press Any Key to Restart ]";

        int msgX = startX;
        int msgY = startY + 8;
        int tick = 0;
        bool showText = true;

        // 키를 누를 때까지 무한 루프
        while (true) {
            // 1. 키 입력 확인 (안 멈춤)
            if (_kbhit()) {
                _getch(); // 입력된 키 소비 (버퍼 비우기)
                break;    // 루프 탈출 -> 함수 종료 -> 게임 재시작
            }

            // 2. 깜빡임 로직 (0.5초 주기)
            if (tick % 10 == 0) {
                ConsoleHelper::gotoXY(msgX, msgY);

                if (showText) {
                    // 텍스트 보이기 (초록색)
                    ConsoleHelper::setColor(Color::GREEN);
                    std::cout << savedMsg;
                }
                else {
                    // 텍스트 지우기 (공백)
                    for (int i = 0; i < savedMsg.length(); ++i) std::cout << " ";
                }
                showText = !showText; // 상태 반전
            }

            Sleep(50);
            tick = (tick + 1) % 30;
        }

        // 색상 초기화
        ConsoleHelper::setColor(Color::BLACK);
    }

    /*
     * [함수: showHighScores]
     * 설명: score.txt 파일에서 점수 데이터를 읽어와 내림차순 정렬 후,
     * 상위 N개의 랭킹을 지정된 좌표에 출력합니다.
     */
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

            // name \t score 형식
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

            std::cout << std::left << std::setw(17) << n;               // 이름 왼쪽 정렬
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

    /*
     * [함수: drawPausePopup]
     * 설명: 일시정지(P) 키를 눌렀을 때 호출됩니다.
     * 화면을 가리고 메뉴(RESUME / QUIT)를 출력하며, 사용자 선택을 반환합니다.
     * 리턴: 0 (RESUME), 1 (QUIT)
     */
    int Game::drawPausePopup() {
        ConsoleHelper::clear();

        // 1. Pause 아트
        std::string pauseArt[5] = {
            "___________   __ __  ______ ____  ",
            "\\____ \\__  \\ |  |  \\/  ___// __ \\ ",
            "|  |_> > __ \\|  |  /\\___ \\\\  ___/ ",
            "|   __(____  /____//____  >\\___  > ",
            "|__|       \\/           \\/     \\/ "
        };

        int startX = 42;
        int startY = 10;

        ConsoleHelper::setColor(Color::RED);
        for (int i = 0; i < 5; ++i) {
            ConsoleHelper::gotoXY(startX, startY + i);
            std::cout << pauseArt[i];
        }

        // 2. 메뉴 선택 로직
        int menuIndex = 0; // 0: Resume, 1: Quit
        int textX = startX + 10;
        int textY = startY + 7;

        while (true) {
            // 메뉴 텍스트 그리기 (선택된 것만 강조)
            ConsoleHelper::gotoXY(textX, textY);
            if (menuIndex == 0) {
                ConsoleHelper::setColor(Color::WHITE); // 선택됨 (흰색 & 화살표)
                std::cout << "▶ [ RESUME ]";
            }
            else {
                ConsoleHelper::setColor(Color::GRAY);  // 안 선택됨 (회색)
                std::cout << "  [ RESUME ]";
            }

            ConsoleHelper::gotoXY(textX, textY + 2);
            if (menuIndex == 1) {
                ConsoleHelper::setColor(Color::WHITE);
                std::cout << "▶ [ QUIT ]";
            }
            else {
                ConsoleHelper::setColor(Color::GRAY);
                std::cout << "  [ QUIT ]";
            }

            // 키 입력 대기
            int key = _getch();

            // 방향키 처리
            if (key == 224) {
                key = _getch();       // 실제 키 코드 읽기
                if (key == 72) {      // UP
                    menuIndex = 0;
                }
                else if (key == 80) { // DOWN
                    menuIndex = 1;
                }
            }
            // Enter키(13) 또는 Space 키(32): 선택 완료
            else if (key == 13 || key == 32) {
                return menuIndex; // 0 또는 1 리턴
            }
            // P 키: Resume과 동일 취급
            else if (key == 'p' || key == 'P') {
                return 0; // Resume
            }
        }
    }

    /*
     * [함수: showGoodBye]
     * 설명: 게임 종료(Quit) 시 작별 인사 아트를 화면 중앙에 출력하고,
     * 잠시 대기한 후 프로그램을 완전히 종료합니다.
     */
    void Game::showGoodBye() {
        ConsoleHelper::clear();

        // 1. Good Bye 아트 정의 (이스케이프 문자 처리 완료)
        std::string goodByeArt[6] = {
            "  ________                   .___ __________             ._.",
            " /  _____/  ____   ____   __| _/ \\______   \\___.__. ____| |",
            "/   \\  ___ /  _ \\ /  _ \\ / __ |   |    |  _<   |  |/ __ \\ |",
            "\\    \\_\\  (  <_> |  <_> ) /_/ |   |    |   \\\\___  \\  ___/\\|",
            " \\______  /\\____/ \\____/\\____ |   |______  // ____|\\___  >_",
            "        \\/                   \\/          \\/ \\/         \\/\\/"
        };

        // 2. 중앙 정렬 좌표 계산
        // 아트 너비: 약 67칸
        // 화면 너비: 120칸
        // 시작 X: (120 - 67) / 2 = 26
        int startX = 26;
        int startY = 10; // 화면 중앙 높이

        // 3. 출력
        ConsoleHelper::setColor(Color::WHITE);
        for (int i = 0; i < 6; ++i) {
            ConsoleHelper::gotoXY(startX, startY + i);
            std::cout << goodByeArt[i];
        }

        // 4. 잠시 보여주고 종료
        Sleep(2000); // 2초 대기

        // 색상 초기화 및 커서 아래로 이동 (콘솔창 닫힐 때 깔끔하게)
        ConsoleHelper::setColor(Color::WHITE);
        ConsoleHelper::gotoXY(0, 38);
    }
}