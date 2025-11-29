#pragma once
#include "Common.h"

namespace Tetris {
    class ConsoleHelper {
    public:
        static void gotoXY(int x, int y);
        static void setColor(Color color);
        static void cursorVisible(bool visible);
        static void clear();
        static void flushInput();
    };
}