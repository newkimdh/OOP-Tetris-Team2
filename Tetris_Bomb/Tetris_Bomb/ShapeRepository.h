#pragma once
#include "Common.h"
#include <array>

/*
 * 파일명: ShapeRepository.h
 * 설명: 테트리스 블록들의 모양 데이터(4x4 격자)와 회전 상태,
 * 색상 정보를 저장하고 제공하는 저장소(Repository) 클래스입니다.
 */

namespace Tetris {

    class ShapeRepository {
    public:
        // ===========================================================
        // [타입 정의] 블록 모양 데이터 구조
        // ===========================================================

        // 4x4 격자 하나를 나타내는 타입 (0:빈공간, 1:블록)
        using ShapeGrid = std::array<std::array<int, 4>, 4>;

        // 블록 하나가 가질 수 있는 4가지 회전 상태의 모음
        using ShapeRotations = std::array<ShapeGrid, 4>;


        // ===========================================================
        // [정적 함수] 데이터 접근 인터페이스
        // ===========================================================

        /* * 설명: 지정된 타입(번호)에 해당하는 블록의 모양 데이터(회전 포함)를 반환합니다.
         * static 함수이므로 객체 생성 없이 호출 가능합니다.
         */
        static const ShapeRotations& getShape(int type);

        /* * 설명: 지정된 타입(번호)에 해당하는 블록의 고유 색상을 반환합니다.
         */
        static Color getColorForType(int type);
    };
}