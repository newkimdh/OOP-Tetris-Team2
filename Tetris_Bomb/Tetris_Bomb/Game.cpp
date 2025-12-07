#include "Game.h"
#include "ConsoleHelper.h"
#include "ShapeRepository.h"
#include <iostream>
#include <conio.h>   // _getch, _kbhit (입력 대기용)
#include <ctime>     // time           
#include <windows.h> // Sleep          (대기 애니메이션용)
#include <string>    // string         (문자열 처리용)

/*
 * 파일명: Game.cpp
 * 설명: Tetris 게임의 핵심 로직을 담당하는 파일입니다.
 * 게임 흐름 제어, 키 입력 처리, 블록 이동 및 충돌 계산 등이 포함됩니다.
 * (화면 출력 관련 코드는 Game_UI.cpp에 있습니다.)
 
  ===========================================================================
    [Game.cpp 함수 목차 (Function Index)]

    1. Game::Game (생성자) .................... Line 34
    2. Game::run (메인 루프) .................. Line 47
    3. Game::initStages (난이도 설정) ......... Line 124
    4. Game::resetGame (초기화) ............... Line 144
    5. Game::makeNewBlockType (블록 생성) ..... Line 158
    6. Game::spawnNextBlock (다음 블록) ....... Line 174
    7. Game::handleInput (키 입력) ............ Line 196
    8. Game::moveDown (하강 로직) ............. Line 294
    9. Game::moveSide (좌우 이동) ............. Line 388
   10. Game::rotateBlock (회전) ............... Line 410
   11. Game::useBomb (폭탄 스킬) .............. Line 438
  ===========================================================================
  */

namespace Tetris {
    /*
     * [Game 생성자]
     * 설명: 게임에 필요한 변수들을 초기화하고 난이도 데이터를 설정합니다.
     * 랜덤 시드값 생성도 여기서 수행합니다.
     */
    Game::Game() 
        : level(0), score(0), totalLines(0), gameOver(false), 
        bombCount(3), savedBlockForBomb(-1), shouldExitToTitle(false)  // [추가] 초기화
    {
        srand(static_cast<unsigned>(time(NULL)));
        initStages();
    }

    /*
     * [함수: run]
     * 설명: 게임의 전체 생명주기를 관리하는 메인 루프입니다.
     * 인트로 -> (초기화 -> 레벨선택 -> 인게임 루프) 과정을 반복합니다.
     * 레벨 선택에서 종료(-1) 신호를 받으면 루프를 탈출합니다.
     */
    void Game::run() {
        // 1. 게임 초기 설정
        ConsoleHelper::cursorVisible(false);
        showLogo();

        while (true) {
            // 2. 매 게임마다 초기화 및 레벨 선택
            resetGame();
            selectLevel();

            // [종료 체크] 레벨 선택에서 'Q'(-1)를 누르면 종료
            if (level == -1) {
                showGoodBye(); // 종료 화면 출력
                break;         // 프로그램 종료
            }

            // 3. 게임 플레이 준비
            ConsoleHelper::clear();
            board.setLevel(level);
            board.reset();
            heldBlockType = -1;
            canHold = true;
            board.draw(); // 보드 그리기

            // 첫 블록 생성 및 UI 초기화
            nextBlockType = makeNewBlockType();
            spawnNextBlock();
            showStats();
            showNextBlockPreview();

            // -------------------------------------------------------
            // [인게임 루프] 프레임 단위로 게임 로직을 실행합니다.
            // -------------------------------------------------------
            int frameCount = 0;
            while (!gameOver) {
                // (1) 키 입력 처리
                handleInput();

                // (2) 강제 종료 체크 (일시정지 메뉴에서 Quit 선택 시)
                if (shouldExitToTitle) {
                    break;
                }

                // (3) 일시정지 체크 (handleInput 내부에서 처리되지만 안전장치 유지)
                if (isPaused) {
                    Sleep(15);
                    continue;
                }

                // (4) 블록 자동 하강 (속도 조절)
                if (frameCount % stages[level].speed == 0) {
                    moveDown();
                }

                // (5) 게임 오버 체크
                if (gameOver) {
                    // 강제 종료가 아닐 때만 결과창 및 랭킹 저장
                    if (!shouldExitToTitle) {
                        showGameOver();
                        promptNameAndSaveScore();
                    }
                    break;
                }

                // (6) 프레임 대기 및 카운트
                Sleep(15);
                frameCount++;
            }
        }
    }

    /*
     * [함수: initStages]
     * 설명: 각 레벨별 난이도 데이터(속도, 확률 등)를 초기화합니다.
     */
    void Game::initStages() {
        // { 낙하 속도(낮을수록 빠름) , 긴 막대기 획득 확률 , 레벨업 조건 줄 수 }
        stages = {
            {40, 20, 10}, 
            {38, 18, 12}, 
            {35, 18, 15}, 
            {30, 17, 20},
            {25, 16, 20}, 
            {20, 14, 25}, 
            {15, 14, 25}, 
            {10, 13, 30},
            {6, 12, 35},  
            {4, 11, 99999}
        };
    }

    /*
     * [함수: resetGame]
     * 설명: 새 게임을 시작하기 위해 점수, 상태 변수들을 리셋합니다.
     */
    void Game::resetGame() {
        score = 0;
        totalLines = 0;
        gameOver = false;
        bombCount = 3;              // 게임 리셋 시 폭탄 3개 충전
        savedBlockForBomb = -1;
        shouldExitToTitle = false;  // 새 게임 시작할 때 '나가기 상태'를 해제
        isPaused = false;           // 혹시 모르니 일시정지 상태도 확실히 꺼둠

    }

    /*
     * [함수: makeNewBlockType]
     * 설명: 확률에 따라 새로운 블록의 타입(번호)을 생성합니다.
     * 리턴: 생성된 블록 타입 ID (0~6, 혹은 7:폭탄)
     */
    int Game::makeNewBlockType() {
        int rnd = rand() % 100;

        // 스테이지 설정에 따라 긴 막대기(Type 0) 확률 조정
        if (rnd <= stages[level].stickRate) 
            return 0;

        return (rand() % 6) + 1;
    }

    /*
     * [함수: spawnNextBlock]
     * 설명: 대기 중이던 Next 블록을 현재 블록으로 가져오고,
     * 새로운 Next 블록을 생성합니다. 관련 UI도 갱신합니다.
     */
    void Game::spawnNextBlock() {
        currentBlock.spawn(nextBlockType);
        nextBlockType = makeNewBlockType();

        showNextBlockPreview();
        canHold = true;      // 홀드 기능 사용 가능 상태로 리셋
        showHoldBlock();
        
        drawGhost(false);
        currentBlock.draw();
    }
    
    /*
     * [함수: handleInput]
     * 설명: 사용자 키보드 입력을 감지하고 해당 동작을 수행합니다.
     * (이동, 회전, 스킬, 일시정지 등)
     */
    void Game::handleInput() {
        if (_kbhit()) {
            int key = _getch();

            // 특수키 (방향키) 처리
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
            // 일반 키 처리
            else {
                // C 키: 홀드
                if (key == 'c' || key == 'C') {
                    // 1. 이번 턴에 이미 썼다면 무시
                    if (!canHold) return;

                    if (heldBlockType != -1 && heldBlockType == currentBlock.getType()) {
                        return;
                    }

                    // 2. 현재 화면에 있는 블록 지우기 (중요: 안 지우면 잔상 남음)
                    currentBlock.draw(true);
                    drawGhost(true);

                    // 홀드 로직: 저장된 블록이 없으면 넣고, 있으면 교체
                    if (heldBlockType == -1) {
                        // Case A: 킵된 블록이 없을 때
                        heldBlockType = currentBlock.getType(); // 현재 블록 저장
                        spawnNextBlock();                       // 다음 블록을 현재 블록으로 가져옴
                    }
                    else {
                        // Case B: 킵 된게 있을 때 (서로 교체)
                        int tempType = currentBlock.getType();

                        currentBlock.spawn(heldBlockType);      // 저장된 타입으로 현재 블록 재생성 (위치, 각도 초기화)
                        heldBlockType = tempType;               // 현재 타입을 저장소로
                    }
                    canHold = false; // 이번 턴에는 다시 사용 불가

                    // 3. 변경된 상태 반영하여 UI 갱신
                    showHoldBlock();        // 왼쪽 홀드창 갱신
                    showNextBlockPreview(); // (Case A일 경우 필요)
                    currentBlock.draw();    // 현재 블록 다시 그리기

                    return; // 입력 처리 완료
                }

                // P 키: 일시 정지 (메뉴 호출)
                if (key == 'p' || key == 'P') {
                    isPaused = true;

                    // 팝업 함수 호출, 결과 반환
                    int choice = drawPausePopup();
                    
                    // [선택 1] 나가기 Quit 선택 시
                    if (choice == 1) {
                        isPaused = false;          // 상태 초기화 
                        shouldExitToTitle = true;  // 타이틀로 간다고 표시
                        gameOver = true;           // 게임 루프 종료시키기
                        return;     
                    }

                    // [선택 0] 계속하기(Resume) 선택 시 -> 기존 복구 로직 실행
                    isPaused = false;
                    ConsoleHelper::clear(); // pause 창 지우기
                    board.draw();           // a. 모드
                    showStats();            // b. 점수, 레벨, 폭탄
                    showNextBlockPreview(); // c. 다음 블록 UI
                    showHoldBlock();        // d. 홀드 UI
                    drawGhost(false);
                    currentBlock.draw();
                    isPaused = false;

                    return;
                }

                // B 키: 폭탄 스킬
                if (key == 'b' || key == 'B') {
                    if (!isPaused) useBomb();
                    return;
                }

                // Space 키: 하드 드롭 (바닥까지 즉시 이동)
                if (isPaused) return;       // 일시정지 중이면 다른 일반 키(스페이스바 등) 무시
                if (key == static_cast<int>(Key::SPACE)) {
                    while (moveDown());     // 바닥에 닿을 때까지 계속 내림
                }
            }
        }
    }

    /*
     * [함수: moveDown]
     * 설명: 블록을 한 칸 아래로 이동시킵니다.
     * 바닥이나 다른 블록에 닿았을 경우 고정(Merge) 및 줄 삭제 처리를 합니다.
     * 리턴: true = 이동 성공, false = 바닥에 닿아서 멈춤(고정됨)
     */
    bool Game::moveDown() {
        // 1. 충돌 체크 (미리 한 칸 아래를 확인)
        if (board.checkCollision(currentBlock, currentBlock.x, currentBlock.y + 1, currentBlock.angle)) {
            drawGhost(true);        // 블록이 굳기 직전에, 그려져 있던 고스트를 지워줍니다.
            currentBlock.draw();    // 최종 위치 그리기

            // 천장에 닿았으면 게임 오버
            if (currentBlock.y < 0) {
                gameOver = true;
                return false;
            }

            // [폭탄 블록 처리] 폭탄인 경우
            if (currentBlock.getType() == BOMB_TYPE) {
                // (1) 폭탄 폭발
                // 폭탄의 중심 좌표 계산 (Block의 x, y는 4x4 박스의 좌상단)
                // 폭탄 모양이 (1, 1)에 점이 있으므로. 실제 좌표는 x+1, y+1
                int destroyed = board.explode(currentBlock.x + 1, currentBlock.y + 1);

                // 점수 규칙: 블록 비례 보너스
                if (destroyed > 0) {
                    if (destroyed <= 3)
                        score += destroyed * 30;
                    else if (destroyed <= 6)
                        score += destroyed * 60;
                    else
                        score += destroyed * 90;
                }

                // (2) 저장해뒀던 원래 블록을 다시 소환 (Next 블록을 소모하지 않음)
                if (savedBlockForBomb != -1) {
                    currentBlock.spawn(savedBlockForBomb);
                    savedBlockForBomb = -1; // 저장소 초기화

                    // 소환된 원래 블록 그리기
                    drawGhost(false);
                    currentBlock.draw();

                    return false;           // 이번 틱 종료 (바로 내려가지 않음)
                }
            } 
            else 
                board.merge(currentBlock);  // 일반 블록 고정

            // 줄 삭제 처리
            int lines = board.processFullLines();

            if (lines == 0) 
                board.draw();   // 줄 삭제 없어도 화면 갱신
            
            if (lines > 0) {
                totalLines += lines;
                // 점수 계산 (기본 점수 + 레벨 가중치)
                for (int k = 0; k < lines; k++) 
                    score += 100 + (level * 10) + (rand() % 10);

                // 레벨업 체크
                if (totalLines >= stages[level].clearLineGoal) {
                    totalLines = 0;
                    if (level < 9) level++;
                    board.setLevel(level);
                    board.draw();   // 레벨업 시 벽 색상 변경 반영
                }
                showStats();
            }

            // 다음 블록 소환
            spawnNextBlock();

            // UI 안전 갱신
            drawGhost(false);
            currentBlock.draw();

            return false; // 더 이상 내려가지 않음
        }

        // 2. 충돌 없음: 실제 이동 처리
        drawGhost(true);         // 이전 고스트 지우기
        currentBlock.draw(true); // 이전 블록 지우기

        currentBlock.y++;        // 좌표 이동

        drawGhost(false);        // 새 고스트 그리기
        currentBlock.draw();     // 새 블록 그리기

        return true;
    }
    
    /*
     * [함수: moveSide]
     * 설명: 블록을 좌(-1) 또는 우(+1)로 이동시킵니다.
     * 인자: dx (이동할 방향과 거리)
     */
    void Game::moveSide(int dx) {
        if (!board.checkCollision(currentBlock, currentBlock.x + dx, currentBlock.y, currentBlock.angle)) {

            // 1. 지우기 (고스트 먼저, 그다음 블록)
            drawGhost(true);         // 고스트 지움
            currentBlock.draw(true); // 블록 지움

            // 2. 이동
            currentBlock.x += dx;

            // 3. 그리기 (고스트 먼저, 그다음 블록)
            // 고스트를 먼저 그려야 블록이 고스트 위에 겹쳐질 때 블록이 우선순위를 가짐
            drawGhost(false);        // 새 위치에 고스트 그림
            currentBlock.draw();     // 새 위치에 블록 그림
        }
    }

    /*
     * [함수: rotateBlock]
     * 설명: 블록을 시계 방향으로 회전시킵니다. (Wall Kick 로직 포함)
     */
    void Game::rotateBlock() {
        int nextAngle = currentBlock.getNextAngle();
        // Wall Kick: 회전 시 벽에 걸리면 위치를 옮겨서 회전을 성공시킴
        int kickOffsets[] = { 0, -1, 1, -2, 2 };

        for (int offset : kickOffsets) {
            int testX = currentBlock.x + offset;
            if (!board.checkCollision(currentBlock, testX, currentBlock.y, nextAngle)) {

                drawGhost(true);         // 회전하기 전에 이전 고스트 지우기
                currentBlock.draw(true); // 이전 블록 지우기

                // 상태 변경 (회전 및 위치 보정)
                currentBlock.angle = nextAngle;
                currentBlock.x = testX;

                drawGhost(false);        // [추가] 회전한 상태의 새 고스트 그리기
                currentBlock.draw();     // 새 블록 그리기

                return;
            }
        }
    }

    /*
     * [함수: useBomb]
     * 설명: 현재 블록을 폭탄 아이템으로 교체합니다. (횟수 제한)
     */
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