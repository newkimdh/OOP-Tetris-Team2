#pragma once
#include "Common.h"

namespace Tetris {
    // 1. 추상 클래스 자식에서 override를 해줘야 한다.
    class BlockBase {
    public:
        int x, y;
        int angle;

        BlockBase();
        virtual ~BlockBase() = default;

        virtual int getType() const = 0;
        /*virtual void draw(bool erase = false) const = 0;*/
        virtual void draw(bool erase = false, bool isGhost = false) const = 0; // 파라미터 추가

        int getNextAngle() const;
    };

    // 2. 실체 클래스
    class ShapeBlock : public BlockBase {
    private:
        int type;

    public:
        ShapeBlock();
        void spawn(int newType);

        int getType() const override;
       /* void draw(bool erase = false) const override;*/
        void draw(bool erase = false, bool isGhost = false) const override; // 파라미터 추가
    };
}