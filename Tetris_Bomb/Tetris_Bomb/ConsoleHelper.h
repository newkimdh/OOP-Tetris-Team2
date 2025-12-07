#pragma once
#include "Common.h"

/*
 * 파일명: ConsoleHelper.h
 * 설명: 콘솔(터미널) 화면 제어를 위한 정적(Static) 헬퍼 클래스입니다.
 * 커서 이동, 색상 변경, 화면 지우기, 입력 버퍼 비우기 등의 기능을 제공합니다.
 */

namespace Tetris {

    class ConsoleHelper {
    public:
        // ===========================================================
        // [커서 및 화면 제어]
        // ===========================================================

        /* 커서를 지정된 좌표(x, y)로 이동시킵니다. */
        static void gotoXY(int x, int y);

        /* 텍스트 출력 색상을 변경합니다. */
        static void setColor(Color color);

        /* 커서의 깜빡임을 보이거나 숨깁니다. (true: 보임, false: 숨김) */
        static void cursorVisible(bool visible);

        /* 콘솔 화면을 깨끗하게 지웁니다. (cls) */
        static void clear();


        // ===========================================================
        // [입력 제어]
        // ===========================================================

        /* 키보드 입력 버퍼에 남아있는 잔여 입력값들을 모두 비웁니다. */
        static void flushInput();
    };
}