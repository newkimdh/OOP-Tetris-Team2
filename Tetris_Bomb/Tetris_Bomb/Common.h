#pragma once
namespace Tetris {
    // 상수들
    constexpr int BOARD_HEIGHT = 21;
    constexpr int BOARD_WIDTH = 14;
    constexpr int OFFSET_X = 47;
    constexpr int OFFSET_Y = 1;

    // 열거형들
    enum class Key {//enum Class => 꼭 클래스의 이름을 밝혀야 한다 예를 들어 Key::UP 이렇게 => 이름 충돌을 막아준다.
        UP = 0x48, DOWN = 0x50, LEFT = 0x4b, RIGHT = 0x4d, SPACE = 32, NONE = 0
    };

    enum class Color {
        BLACK = 0,
        DARK_BLUE = 1, 
        DARK_GREEN = 2, 
        DARK_SKY_BLUE = 3, 
        DARK_RED = 4, 
        DARK_VIOLET = 5, 
        DARK_YELLOW = 6,
        GRAY = 7, 
        DARK_GRAY = 8,
        BLUE = 9, 
        GREEN = 10, 
        SKY_BLUE = 11, 
        RED = 12, 
        VIOLET = 13, 
        YELLOW = 14, 
        WHITE = 15
    };

    struct StageConfig {
        int speed;
        int stickRate;
        int clearLineGoal;
    };
}
