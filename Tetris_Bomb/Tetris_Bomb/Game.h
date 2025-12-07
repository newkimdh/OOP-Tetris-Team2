#pragma once
#include "Common.h"
#include "Board.h"
#include "Block.h"
#include <string>
#include <vector>

/*
 * 파일명: Game.h
 * 설명: Tetris 게임의 핵심 클래스인 Game의 선언부입니다.
 * 게임의 상태 데이터(변수)와 기능(함수)들이 정의되어 있습니다.
 *
 * [알림] 구현부(.cpp)는 역할에 따라 두 파일로 분리되어 있습니다.
 * 1. Game.cpp    : 게임 로직 (이동, 충돌, 계산)
 * 2. Game_UI.cpp : 화면 출력 (로고, 팝업, 점수판)
 */

namespace Tetris {

    class Game {
    private:
        // ===========================================================
        // [멤버 변수] 게임 상태 데이터
        // ===========================================================

        // 1. 핵심 게임 오브젝트
        Board board;                    // 게임 보드 (격자)
        ShapeBlock currentBlock;        // 현재 조종 중인 블록
        int nextBlockType;              // 다음에 나올 블록 타입
        std::vector<StageConfig> stages; // 스테이지별 난이도 정보

        // 2. 플레이 정보 (점수, 레벨)
        int level;                      // 현재 레벨 (0 ~ 9)
        int score;                      // 현재 점수
        int totalLines;                 // 지운 총 줄 수
        const int HIGH_SCORE_LIMIT = 10;// 랭킹 표시 개수

        // 3. 게임 상태 플래그
        bool gameOver;                  // 게임 종료 여부
        bool isPaused = false;          // 일시정지 여부
        bool shouldExitToTitle;         // 타이틀 화면으로 나가기 플래그

        // 4. 홀드(Hold) 시스템
        int heldBlockType = -1;         // 저장된 블록 (-1: 없음)
        bool canHold = true;            // 이번 턴 사용 가능 여부

        // 5. 폭탄(Bomb) 스킬
        int bombCount;                  // 남은 폭탄 개수
        int savedBlockForBomb;          // 폭탄 변신 전 원래 블록 저장
        const int BOMB_TYPE = 7;        // 폭탄 블록의 타입 ID


    public:
        // ===========================================================
        // [Public 함수] 외부에서 호출 가능한 인터페이스
        // ===========================================================
        Game();         // 생성자
        void run();     // 게임 메인 루프 실행


    private:
        // ===========================================================
        // [Private 함수 1] 게임 핵심 로직 (Game.cpp 구현)
        // ===========================================================

        // 초기화 및 설정
        void initStages();              // 난이도 데이터 설정
        void resetGame();               // 게임 재시작 초기화

        // 블록 생성 및 조작
        int  makeNewBlockType();        // 랜덤 블록 생성
        void spawnNextBlock();          // 다음 블록 소환
        void handleInput();             // 키보드 입력 처리

        // 이동 및 액션
        bool moveDown();                // 한 칸 하강
        void moveSide(int dx);          // 좌우 이동
        void rotateBlock();             // 회전
        void useBomb();                 // 폭탄 스킬 사용


        // ===========================================================
        // [Private 함수 2] 화면 출력 및 UI (Game_UI.cpp 구현)
        // ===========================================================

        // 메인 화면 구성
        void showLogo();                // 인트로 로고
        void selectLevel();             // 레벨 선택 화면
        void showGoodBye();             // 종료 화면

        // 인게임 정보 표시
        void showStats();               // 점수/레벨/폭탄 정보창
        void showNextBlockPreview();    // 다음 블록 미리보기
        void showHoldBlock();           // 홀드 블록 표시
        void drawGhost(bool erase);     // 고스트(낙하 예상 위치) 표시

        // 팝업 및 메뉴
        int  drawPausePopup();          // 일시정지 메뉴 (Resume/Quit)
        void showGameOver();            // 게임 오버 팝업
        void promptNameAndSaveScore();  // 이름 입력 및 점수 저장

        // 랭킹 시스템
        void showHighScores(int maxCount, int startX, int startY,
            const std::string* curName = nullptr, int curScore = 0);
    };
}