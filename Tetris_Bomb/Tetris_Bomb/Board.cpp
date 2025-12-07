#include "Board.h"
#include "ConsoleHelper.h"
#include "ShapeRepository.h"
#include <iostream>
#include <string>
#include <windows.h>

/*
 * 파일명: Board.cpp
 * 설명: Tetris 게임의 [보드(격자)]를 관리하는 클래스 구현부입니다.
 * 게임판 상태 저장, 충돌 체크, 줄 삭제, 특수 효과(폭발) 등을 처리합니다.
 */

namespace Tetris {

    /* 
     * [Board 생성자] 레벨을 초기화하고 보드를 리셋합니다. 
     */
    Board::Board() : level(0) {
        reset();
    }

    /* 
     * [함수: setLevel]
     * 설명: 현재 게임 레벨을 설정합니다. (벽 색상 변경 등에 사용)
     */
    void Board::setLevel(int lvl) {
        level = lvl;
    }

    /*
     * [함수: reset]
     * 설명: 게임 보드를 초기 상태로 만듭니다.
     * 테두리는 벽(1)으로 채우고, 내부는 빈 공간(0)으로 채웁니다.
     */
    void Board::reset() {
        for (int i = 0; i < BOARD_HEIGHT; ++i) {
            for (int j = 0; j < BOARD_WIDTH; ++j) {
                // 좌우 벽과 바닥 벽 설정
                if (j == 0 || j == BOARD_WIDTH - 1 || i == BOARD_HEIGHT - 1)
                    grid[i][j] = 1; // Wall (Filled)
                else
                    grid[i][j] = 0; // Empty
            }
        }
        // 바닥 벽 확실하게 채우기 (중복될 수 있으나 안전 장치)
        for (int j = 1; j < BOARD_WIDTH - 1; ++j) {
            grid[BOARD_HEIGHT - 1][j] = 1;
        }
    }

    /*
     * [함수: draw]
     * 설명: 현재 보드의 상태(벽, 블록)를 화면에 그립니다.
     * 벽 색상은 레벨에 따라 변화합니다.
     */
    void Board::draw() const {
        ConsoleHelper::setColor(Color::DARK_GRAY);

        for (int i = 0; i < BOARD_HEIGHT; ++i) {
            for (int j = 0; j < BOARD_WIDTH; ++j) {

                // 블록이나 벽이 있는 경우
                if (grid[i][j] == 1) {
                    // 벽(테두리)인 경우 -> 레벨별 색상 적용
                    if (j == 0 || j == BOARD_WIDTH - 1 || i == BOARD_HEIGHT - 1) {
                        ConsoleHelper::setColor(static_cast<Color>((level % 6) + 1));
                    }
                    else {
                        // 내부에 쌓인 블록인 경우 -> 회색
                        ConsoleHelper::setColor(Color::DARK_GRAY);
                    }
                    ConsoleHelper::gotoXY(j * 2 + OFFSET_X, i + OFFSET_Y);
                    std::cout << "■";
                }
                else {
                    // 빈 공간
                    std::cout << "  ";
                }
            }
        }
        ConsoleHelper::setColor(Color::BLACK);
    }

    /*
     * [함수: checkCollision]
     * 설명: 블록이 특정 위치(nextX, nextY)와 각도(nextAngle)로 이동할 때
     * 벽이나 다른 블록과 충돌하는지 검사합니다.
     * 리턴: true = 충돌함, false = 이동 가능
     */
    bool Board::checkCollision(const BlockBase& b, int nextX, int nextY, int nextAngle) const {
        const auto& shape = ShapeRepository::getShape(b.getType())[nextAngle];

        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                // 블록 모양이 없는 부분은 패스
                if (shape[j][i] == 0) continue;

                int globalX = nextX + i;
                int globalY = nextY + j;

                // 1. 좌우 벽 충돌 체크
                if (globalX <= 0 || globalX >= BOARD_WIDTH - 1) return true;

                // 2. 위쪽 화면 밖은 충돌 아님 (아직 내려오는 중)
                if (globalY < 0) continue;

                // 3. 바닥 충돌 체크
                if (globalY >= BOARD_HEIGHT) return true;

                // 4. 이미 쌓여있는 블록과 충돌 체크
                if (grid[globalY][globalX] == 1) return true;
            }
        }
        return false;
    }

    /*
     * [함수: merge]
     * 설명: 현재 블록이 바닥에 닿아 멈췄을 때, 보드 데이터(grid)에 고정시킵니다.
     */
    void Board::merge(const BlockBase& b) {
        const auto& shape = ShapeRepository::getShape(b.getType())[b.angle];

        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                if (b.y + j < 0) continue; // 화면 밖 데이터는 무시

                if (shape[j][i] == 1) {
                    grid[b.y + j][b.x + i] = 1; // 보드에 기록
                }
            }
        }
    }

    /*
     * [함수: processFullLines]
     * 설명: 꽉 찬 줄이 있는지 검사하고, 있다면 삭제(Clear)합니다.
     * 삭제된 줄 위의 블록들은 아래로 당겨집니다.
     * 리턴: 삭제된 줄의 개수
     */
    int Board::processFullLines() {
        int linesCleared = 0;

        // 아래에서부터 위로 검사
        for (int i = 0; i < BOARD_HEIGHT - 1; ++i) {
            bool isFull = true;
            // 한 줄 검사 (벽 제외)
            for (int j = 1; j < BOARD_WIDTH - 1; ++j) {
                if (grid[i][j] == 0) { isFull = false; break; }
            }

            // 꽉 찬 줄 발견
            if (isFull) {
                linesCleared++;

                // [연출] 파란색으로 깜빡임 효과
                ConsoleHelper::setColor(Color::BLUE);
                ConsoleHelper::gotoXY(1 * 2 + OFFSET_X, i + OFFSET_Y);
                for (int j = 1; j < BOARD_WIDTH - 1; ++j) { std::cout << "□"; Sleep(10); }

                ConsoleHelper::gotoXY(1 * 2 + OFFSET_X, i + OFFSET_Y);
                for (int j = 1; j < BOARD_WIDTH - 1; ++j) { std::cout << "  "; Sleep(10); }

                // [로직] 윗줄들을 아래로 한 칸씩 당김 (삭제 구현)
                for (int k = i; k > 0; --k) {
                    for (int j = 1; j < BOARD_WIDTH - 1; ++j)
                        grid[k][j] = grid[k - 1][j];
                }
                // 맨 윗줄은 비움
                for (int j = 1; j < BOARD_WIDTH - 1; ++j)
                    grid[0][j] = 0;
            }
        }

        if (linesCleared > 0) draw(); // 변화가 있으면 다시 그림
        return linesCleared;
    }

    /*
     * [함수: explode]
     * 설명: 폭탄 블록이 터졌을 때 주변(3x3) 블록을 파괴합니다.
     * 붉은색 -> 회색 -> 삭제 순서로 이펙트를 보여줍니다.
     * 리턴: 파괴된 블록의 개수
     */
    int Board::explode(int centerX, int centerY) {
        int destroyedCount = 0;

        // [Step 1] 1차 이펙트: 붉은색 폭발 (▣)
        for (int y = centerY - 1; y <= centerY + 1; ++y) {
            for (int x = centerX - 1; x <= centerX + 1; ++x) {
                // 범위 체크 및 블록 존재 여부 확인
                if (x > 0 && x < BOARD_WIDTH - 1 && y >= 0 && y < BOARD_HEIGHT - 1) {
                    if (grid[y][x] != 0) {
                        ConsoleHelper::gotoXY(x * 2 + OFFSET_X, y + OFFSET_Y);
                        ConsoleHelper::setColor(Color::RED);
                        std::cout << "▣";
                    }
                }
            }
        }
        Sleep(250); // 이펙트 대기

        // [Step 2] 2차 이펙트: 회색 잔해 (□)
        for (int y = centerY - 1; y <= centerY + 1; ++y) {
            for (int x = centerX - 1; x <= centerX + 1; ++x) {
                if (x > 0 && x < BOARD_WIDTH - 1 && y >= 0 && y < BOARD_HEIGHT - 1 && grid[y][x] != 0) {
                    ConsoleHelper::gotoXY(x * 2 + OFFSET_X, y + OFFSET_Y);
                    ConsoleHelper::setColor(Color::DARK_GRAY);
                    std::cout << "□";
                }
            }
        }
        Sleep(50);

        // [Step 3] 실제 데이터 삭제 및 카운팅
        for (int y = centerY - 1; y <= centerY + 1; ++y) {
            for (int x = centerX - 1; x <= centerX + 1; ++x) {
                if (x > 0 && x < BOARD_WIDTH - 1 && y >= 0 && y < BOARD_HEIGHT - 1) {
                    if (grid[y][x] != 0) {
                        destroyedCount++;
                        grid[y][x] = 0;   // 빈 공간으로 설정
                    }
                }
            }
        }

        // [Step 4] 화면 갱신 (말끔하게 지워짐)
        draw();

        return destroyedCount;
    }
}