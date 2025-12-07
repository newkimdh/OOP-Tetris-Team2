#include "ConsoleHelper.h"
#include <windows.h>
#include <iostream>
#include <conio.h>
#include <limits>

/*
 * 파일명: ConsoleHelper.cpp
 * 설명: ConsoleHelper 클래스의 기능을 Windows API를 사용하여 실제로 구현한 파일입니다.
 */

namespace Tetris {

    /*
     * [함수: gotoXY]
     * 설명: Windows API를 사용하여 콘솔 커서 위치를 이동시킵니다.
     * 인자: x (가로 좌표), y (세로 좌표)
     */
    void ConsoleHelper::gotoXY(int x, int y) {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); // 콘솔 핸들 얻기
        COORD pos = { static_cast<SHORT>(x), static_cast<SHORT>(y) };
        SetConsoleCursorPosition(hConsole, pos); // 커서 이동
    }

    /*
     * [함수: setColor]
     * 설명: 이후 출력되는 텍스트의 색상을 설정합니다.
     * 인자: color (Common.h에 정의된 Color 열거형)
     */
    void ConsoleHelper::setColor(Color color) {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, static_cast<int>(color));
    }

    /*
     * [함수: cursorVisible]
     * 설명: 콘솔 커서의 가시성을 설정합니다.
     * 게임 중에는 커서가 깜빡거리는 것이 거슬리므로 보통 false로 설정합니다.
     */
    void ConsoleHelper::cursorVisible(bool visible) {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_CURSOR_INFO cursorInfo;

        // 현재 커서 정보 가져오기
        GetConsoleCursorInfo(hConsole, &cursorInfo);

        // 가시성 설정 변경 (bVisible)
        cursorInfo.bVisible = visible;

        // 변경된 설정 적용
        SetConsoleCursorInfo(hConsole, &cursorInfo);
    }

    /*
     * [함수: clear]
     * 설명: 시스템 명령(cls)을 호출하여 콘솔 화면을 지웁니다.
     * (잦은 호출은 화면 깜빡임을 유발할 수 있으므로 주의가 필요합니다.)
     */
    void ConsoleHelper::clear() {
        system("cls");
    }

    /*
     * [함수: flushInput]
     * 설명: 입력 버퍼에 남아있는 불필요한 키 입력들을 모두 제거합니다.
     * 메뉴 이동이나 게임 종료 후 남은 엔터키 등이 다음 로직에 영향을 주는 것을 방지합니다.
     */
    void ConsoleHelper::flushInput() {
        // 1) _kbhit / _getch를 이용해 콘솔 입력 버퍼 비우기
        while (_kbhit()) {
            _getch();
        }

        // 2) cin 스트림 상태 및 버퍼 정리
        std::cin.clear();

        // 버퍼에 데이터가 있을 때만 ignore 호출 (블로킹 방지)
        auto buf = std::cin.rdbuf();
        if (buf && buf->in_avail() > 0) {
            std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
        }
    }
}