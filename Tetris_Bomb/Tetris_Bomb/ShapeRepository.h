#pragma once
#include "Common.h"
#include <array>

namespace Tetris {
    class ShapeRepository {
    public:
        using ShapeGrid = std::array<std::array<int, 4>, 4>;//블록 4개가 4개=> 모양한개
        using ShapeRotations = std::array<ShapeGrid, 4>;//블록 돌아가는거 모양 다 들은거

        static const ShapeRotations& getShape(int type);//static => 객체를 만들 필요없이 함수를 호출 할 수 있다.
        static Color getColorForType(int type);
    };
}