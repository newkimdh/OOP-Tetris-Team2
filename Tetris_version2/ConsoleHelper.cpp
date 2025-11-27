#include "ConsoleHelper.h"
#include <windows.h>
#include <iostream>
#include <conio.h>
#include <limits>

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

    void ConsoleHelper::flushInput() {
        // 1) 키보드 버퍼 비우기 (_kbhit / _getch)
        while (_kbhit()) {
            _getch();
        }

        // 2) cin 스트림 상태/버퍼 정리
        std::cin.clear();

        // 버퍼에 뭐가 있을 때만 ignore 호출 (없으면 블록되니까)
        auto buf = std::cin.rdbuf();
        if (buf && buf->in_avail() > 0) {
            std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
        }
    }
}