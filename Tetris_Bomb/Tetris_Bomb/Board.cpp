#include "Board.h"
#include "ConsoleHelper.h"
#include "ShapeRepository.h"
#include <iostream>
#include <string>
#include <windows.h> // Sleep

namespace Tetris {
    Board::Board() : level(0) { reset(); }

    void Board::setLevel(int lvl) { level = lvl; }

    void Board::reset() {
        for (int i = 0; i < BOARD_HEIGHT; ++i) {
            for (int j = 0; j < BOARD_WIDTH; ++j) {
                if (j == 0 || j == BOARD_WIDTH - 1 || i == BOARD_HEIGHT - 1)
                    grid[i][j] = 1; // Board::FILLED
                else
                    grid[i][j] = 0; // board::EMPTY
            }
        }
        for (int j = 1; j < BOARD_WIDTH - 1; ++j) grid[BOARD_HEIGHT - 1][j] = 1;
    }

    void Board::draw() const {
        ConsoleHelper::setColor(Color::DARK_GRAY);
        for (int i = 0; i < BOARD_HEIGHT; ++i) {
            for (int j = 0; j < BOARD_WIDTH; ++j) {
   
                if (grid[i][j] == 1) { // Board::FILLED
                    if (j == 0 || j == BOARD_WIDTH - 1 || i == BOARD_HEIGHT - 1) {
                        ConsoleHelper::setColor(static_cast<Color>((level % 6) + 1));
                    }
                    else {
                        ConsoleHelper::setColor(Color::DARK_GRAY);
                    }
                    ConsoleHelper::gotoXY(j * 2 + OFFSET_X, i + OFFSET_Y);
                    std::cout << "■";
                }
                else std::cout << "  ";
            }
        }
        ConsoleHelper::setColor(Color::BLACK);
    }

    bool Board::checkCollision(const BlockBase& b, int nextX, int nextY, int nextAngle) const {
        const auto& shape = ShapeRepository::getShape(b.getType())[nextAngle];

        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                if (shape[j][i] == 0) continue;

                int globalX = nextX + i;
                int globalY = nextY + j;

                if (globalX <= 0 || globalX >= BOARD_WIDTH - 1) return true;
                if (globalY < 0) continue;
                if (globalY >= BOARD_HEIGHT) return true;
                if (grid[globalY][globalX] == 1) return true;
            }
        }
        return false;
    }

    void Board::merge(const BlockBase& b) {
        const auto& shape = ShapeRepository::getShape(b.getType())[b.angle];
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                if (b.y + j < 0) continue;
                if (shape[j][i] == 1) {
                    grid[b.y + j][b.x + i] = 1; // Board::FILLED
                }
            }
        }
    }

    int Board::processFullLines() {
        int linesCleared = 0;
        for (int i = 0; i < BOARD_HEIGHT - 1; ++i) {
            bool isFull = true;
            for (int j = 1; j < BOARD_WIDTH - 1; ++j) {
                if (grid[i][j] == 0) { isFull = false; break; }
            }

            if (isFull) {
                linesCleared++;
                ConsoleHelper::setColor(Color::BLUE);
                ConsoleHelper::gotoXY(1 * 2 + OFFSET_X, i + OFFSET_Y);
                for (int j = 1; j < BOARD_WIDTH - 1; ++j) { std::cout << "□"; Sleep(10); }
                ConsoleHelper::gotoXY(1 * 2 + OFFSET_X, i + OFFSET_Y);
                for (int j = 1; j < BOARD_WIDTH - 1; ++j) { std::cout << "  "; Sleep(10); }

                for (int k = i; k > 0; --k) {
                    for (int j = 1; j < BOARD_WIDTH - 1; ++j) grid[k][j] = grid[k - 1][j];
                }
                for (int j = 1; j < BOARD_WIDTH - 1; ++j) grid[0][j] = 0; // Board::EMPTY
            }
        }
        if (linesCleared > 0) draw();
        return linesCleared;
    }

    int Board::explode(int centerX, int centerY) {
        int destroyedCount = 0;

        // [Step 1] 폭발 이펙트 (빨간색으로 변함)
        // 폭발 중심 3x3 영역 순회
        for (int y = centerY - 1; y <= centerY + 1; ++y) {
            for (int x = centerX - 1; x <= centerX + 1; ++x) {
                // 범위 체크 (벽 제외)
                if (x > 0 && x < BOARD_WIDTH - 1 && y >= 0 && y < BOARD_HEIGHT - 1) {
                    if (grid[y][x] != 0) { // 블록이 있는 곳만
                        ConsoleHelper::gotoXY(x * 2 + OFFSET_X, y + OFFSET_Y);
                        ConsoleHelper::setColor(Color::RED); // 빨간색
                        std::cout << "▣"; // 폭발 느낌의 꽉 찬 문자 (또는 ■)
                    }
                }
            }
        }
        // 이펙트를 잠시 보여줌 (0.15초)
        Sleep(250);

        // [Step 2] 2차 이펙트: 회색 잔해 (□) - 요청하신 빈 상자
        for (int y = centerY - 1; y <= centerY + 1; ++y) {
            for (int x = centerX - 1; x <= centerX + 1; ++x) {
                if (x > 0 && x < BOARD_WIDTH - 1 && y >= 0 && y < BOARD_HEIGHT - 1 && grid[y][x] != 0) {
                    ConsoleHelper::gotoXY(x * 2 + OFFSET_X, y + OFFSET_Y);
                    ConsoleHelper::setColor(Color::DARK_GRAY); // 타버린 느낌의 어두운 회색
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
                        destroyedCount++; // 점수 계산용 카운트
                        grid[y][x] = 0;   // 빈 공간으로 만듦
                    }
                }
            }
        }

        // [Step 4] 화면 갱신 (빈 공간 반영)
        draw();

        return destroyedCount; // 파괴한 개수 리턴
    }
}