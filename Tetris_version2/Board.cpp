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
                    grid[i][j] = 1;
                else
                    grid[i][j] = 0;
            }
        }
        for (int j = 1; j < BOARD_WIDTH - 1; ++j) grid[BOARD_HEIGHT - 1][j] = 1;
    }

    void Board::draw() const {
        ConsoleHelper::setColor(Color::DARK_GRAY);
        for (int i = 0; i < BOARD_HEIGHT; ++i) {
            for (int j = 0; j < BOARD_WIDTH; ++j) {
   
                if (grid[i][j] == 1) {
                    if (j == 0 || j == BOARD_WIDTH - 1 || i == BOARD_HEIGHT - 1) {
                        ConsoleHelper::setColor(static_cast<Color>((level % 6) + 1));
                    }
                    else {
                        ConsoleHelper::setColor(Color::DARK_GRAY);
                    }
                    ConsoleHelper::gotoXY(j * 2 + OFFSET_X, i + OFFSET_Y);
                    std::cout << "бс";
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
                    grid[b.y + j][b.x + i] = 1;
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
                for (int j = 1; j < BOARD_WIDTH - 1; ++j) { std::cout << "бр"; Sleep(10); }
                ConsoleHelper::gotoXY(1 * 2 + OFFSET_X, i + OFFSET_Y);
                for (int j = 1; j < BOARD_WIDTH - 1; ++j) { std::cout << "  "; Sleep(10); }

                for (int k = i; k > 0; --k) {
                    for (int j = 1; j < BOARD_WIDTH - 1; ++j) grid[k][j] = grid[k - 1][j];
                }
                for (int j = 1; j < BOARD_WIDTH - 1; ++j) grid[0][j] = 0;
            }
        }
        if (linesCleared > 0) draw();
        return linesCleared;
    }


}