#include "ConsoleHelper.h"
#include <windows.h>
#include <iostream>

namespace Tetris {
    void ConsoleHelper::gotoXY(int x, int y) {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);//터미널 핸들러 얻는거
        COORD pos = { static_cast<SHORT>(x), static_cast<SHORT>(y) };//좌표 설정하기
        SetConsoleCursorPosition(hConsole, pos);//커서 옮기기
    }

    void ConsoleHelper::setColor(Color color) {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, static_cast<int>(color));//console 컬러 설정
    }

    void ConsoleHelper::cursorVisible(bool visible) {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_CURSOR_INFO cursorInfo;
        //커서 구조체
        //typedef struct _CONSOLE_CURSOR_INFO {
        //    DWORD dwSize;   // 커서의 두께 (1 ~ 100)
        //    BOOL  bVisible; // 커서 보임 여부 (TRUE / FALSE)
        //} CONSOLE_CURSOR_INFO; 이렇게 있음
        GetConsoleCursorInfo(hConsole, &cursorInfo);
        cursorInfo.bVisible = visible;//설정 수정하기
        SetConsoleCursorInfo(hConsole, &cursorInfo);
    }

    void ConsoleHelper::clear() {
        system("cls");
    }
}