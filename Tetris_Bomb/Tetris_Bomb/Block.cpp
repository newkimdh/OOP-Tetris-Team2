#include "Block.h"
#include "ConsoleHelper.h"
#include "ShapeRepository.h"
#include <iostream>

/*
 * 파일명: Block.cpp
 * 설명: Tetris 게임에서 사용되는 블록(Block)들의 동작을 구현한 파일입니다.
 * 기본 블록(BlockBase)과 모양을 가진 블록(ShapeBlock)의 이동, 회전, 그리기 등을 처리합니다.
 */

namespace Tetris {

    // ===========================================================
    // [BlockBase] 기본 블록 클래스
    // ===========================================================

    /*
     * [BlockBase 생성자]
     * 설명: 기본 블록의 좌표(x, y)와 회전 각도(angle)를 0으로 초기화합니다.
     */
    BlockBase::BlockBase() 
        : x(0), y(0), angle(0) { }

    /*
     * [함수: getNextAngle]
     * 설명: 현재 각도에서 90도 회전했을 때의 다음 각도(0~3)를 반환합니다.
     * 실제 회전은 하지 않고 예상 값만 계산합니다.
     */
    int BlockBase::getNextAngle() const {
        return (angle + 1) % 4;
    }


    // ===========================================================
    // [ShapeBlock] 모양을 가진 실제 게임 블록 클래스
    // ===========================================================

    /*
     * [ShapeBlock 생성자]
     * 설명: 블록의 타입(type)을 초기화합니다.
     */
    ShapeBlock::ShapeBlock() 
        : type(0) { }

    /*
     * [함수: spawn]
     * 설명: 새로운 블록을 생성하고 초기 위치(보드 상단 중앙)로 배치합니다.
     * 인자: newType (새로 생성할 블록의 종류)
     */
    void ShapeBlock::spawn(int newType) {
        type = newType;
        angle = 0;
        x = 5;  // 보드 가로 중앙
        y = -3; // 보드 위쪽(화면 밖)에서 시작
    }

    /*
     * [함수: getType]
     * 설명: 현재 블록의 타입 번호를 반환합니다.
     */
    int ShapeBlock::getType() const {
        return type;
    }

    /*
     * [함수: draw]
     * 설명: 블록을 화면에 그리거나 지웁니다.
     * 인자:
     * - erase: true면 블록을 검은색(배경색)으로 덮어 지웁니다.
     * - isGhost: true면 낙하 예상 위치(고스트) 스타일로 그립니다.
     */
    void ShapeBlock::draw(bool erase, bool isGhost) const {
        // 현재 타입과 각도에 맞는 4x4 모양 배열을 가져옴
        const auto& shape = ShapeRepository::getShape(type)[angle];

        // [1] 색상 설정
        if (erase) {
            ConsoleHelper::setColor(Color::BLACK);     // 지우기 모드
        }
        else if (isGhost) {
            ConsoleHelper::setColor(Color::DARK_GRAY); // 고스트 모드 (회색)
        }
        else {
            // 일반 모드: 블록 고유의 색상 사용
            ConsoleHelper::setColor(ShapeRepository::getColorForType(type));
        }

        // [2] 4x4 배열 순회하며 블록 출력
        for (int row = 0; row < 4; ++row) {
            for (int col = 0; col < 4; ++col) {
                // 화면 위쪽(보드 밖)에 있는 부분은 그리지 않음
                if (y + col < 0) continue;

                // 블록 데이터가 있는 곳(1)만 그림
                if (shape[col][row] == 1) {
                    // 좌표 계산: (x * 2) 보정 및 오프셋 적용
                    ConsoleHelper::gotoXY((row + x) * 2 + OFFSET_X, col + y + OFFSET_Y);

                    if (erase) {
                        std::cout << "  "; // 지우기 (공백 2칸)
                    }
                    else if (isGhost) {
                        std::cout << "□"; // 고스트 (빈 네모)
                    }
                    else {
                        std::cout << "■"; // 일반 블록 (꽉 찬 네모)
                    }
                }
            }
        }

        // [3] 뒷정리 (색상 복구 및 커서 치우기)
        ConsoleHelper::setColor(Color::BLACK);
        ConsoleHelper::gotoXY(77, 23); // 커서를 방해되지 않는 곳으로 이동
    }
}