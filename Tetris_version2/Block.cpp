#include "Block.h"
#include "ConsoleHelper.h"
#include "ShapeRepository.h"
#include <iostream>

namespace Tetris {
    // === BlockBase ===
    BlockBase::BlockBase() : x(0), y(0), angle(0) {}

    int BlockBase::getNextAngle() const {
        return (angle + 1) % 4;
    }

    // === ShapeBlock ===
    ShapeBlock::ShapeBlock() : type(0) {}

    void ShapeBlock::spawn(int newType) {//새로운 블록 초기화 함수
        type = newType;
        angle = 0;
        x = 5;
        y = -3;
    }

    int ShapeBlock::getType() const {
        return type;
    }

    //void ShapeBlock::draw(bool erase) const {//콘솔에 블록 보이게 하는거
    //    const auto& shape = ShapeRepository::getShape(type)[angle];

    //    if (erase) ConsoleHelper::setColor(Color::BLACK);
    //    else ConsoleHelper::setColor(ShapeRepository::getColorForType(type));

    //    for (int row = 0; row < 4; ++row) {
    //        for (int col = 0; col < 4; ++col) {
    //            if (y + col < 0) continue;

    //            if (shape[col][row] == 1) {
    //                ConsoleHelper::gotoXY((row + x) * 2 + OFFSET_X, col + y + OFFSET_Y);
    //                std::cout << (erase ? "  " : "■");
    //            }
    //        }
    //    }
    //    ConsoleHelper::setColor(Color::BLACK);
    //    ConsoleHelper::gotoXY(77, 23); // 커서 치우기
    //}

    void ShapeBlock::draw(bool erase, bool isGhost) const {
        const auto& shape = ShapeRepository::getShape(type)[angle];

        // [색상 설정]
        if (erase) {
            ConsoleHelper::setColor(Color::BLACK);
        }
        else if (isGhost) {
            // 고스트면 회색 + 짙은 회색
            ConsoleHelper::setColor(Color::DARK_GRAY);
        }
        else {
            // 일반 블록이면 원래 색
            ConsoleHelper::setColor(ShapeRepository::getColorForType(type));
        }

        for (int row = 0; row < 4; ++row) {
            for (int col = 0; col < 4; ++col) {
                if (y + col < 0) continue;

                if (shape[col][row] == 1) {
                    ConsoleHelper::gotoXY((row + x) * 2 + OFFSET_X, col + y + OFFSET_Y);

                    if (erase) {
                        std::cout << "  ";
                    }
                    else if (isGhost) {
                        std::cout << "□"; // 고스트는 빈 네모로 그리기
                    }
                    else {
                        std::cout << "■"; // 일반 블록은 꽉 찬 네모
                    }
                }
            }
        }
        ConsoleHelper::setColor(Color::BLACK);
        ConsoleHelper::gotoXY(77, 23); // 커서 치우기
    }
}