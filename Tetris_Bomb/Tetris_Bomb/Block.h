#pragma once
#include "Common.h"

/*
 * 파일명: Block.h
 * 설명: Tetris 게임에서 사용되는 블록(Block)의 기본 구조와 동작을 정의한 헤더입니다.
 * - BlockBase: 모든 블록의 공통 속성(좌표, 각도)을 가진 추상 기본 클래스
 * - ShapeBlock: 실제 모양(Type)을 가지고 화면에 그려지는 구현 클래스
 */

namespace Tetris {

    // ===========================================================
    // [추상 클래스] BlockBase
    // 설명: 모든 블록이 공통으로 가지는 기본 속성과 인터페이스를 정의합니다.
    // ===========================================================
    class BlockBase {
    public:
        // [멤버 변수] 위치 및 회전 상태
        int x, y;   // 블록의 현재 좌표 (보드 기준)
        int angle;  // 회전 상태 (0 ~ 3)

    public:
        BlockBase();
        virtual ~BlockBase() = default;

        // [순수 가상 함수] 자식 클래스에서 반드시 구현해야 함
        virtual int getType() const = 0;

        /* * [함수: draw]
         * 설명: 블록을 화면에 그립니다.
         * 인자:
         * - erase: true면 지우기 모드 (검은색)
         * - isGhost: true면 고스트 모드 (낙하 예상 위치 표시)
         */
        virtual void draw(bool erase = false, bool isGhost = false) const = 0;

        // [일반 함수] 다음 회전 각도 계산
        int getNextAngle() const;
    };


    // ===========================================================
    // [구현 클래스] ShapeBlock
    // 설명: 실제 게임에 등장하는 7가지 모양(+폭탄) 블록을 구현한 클래스입니다.
    // ===========================================================
    class ShapeBlock : public BlockBase {
    private:
        int type;   // 블록의 모양 타입 (0:I, 1:J, 2:L ... 7:Bomb)

    public:
        ShapeBlock();

        // [함수: spawn] 새로운 블록을 생성하고 초기화합니다.
        void spawn(int newType);

        // [오버라이드] 부모 클래스의 가상 함수 구현
        int getType() const override;

        // 블록 그리기 구현 (일반/지우기/고스트 모드 지원)
        void draw(bool erase = false, bool isGhost = false) const override;
    };

}