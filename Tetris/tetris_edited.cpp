#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <Windows.h>
#include <time.h>

//*********************************
//상수 선언
//*********************************

#define EXT_KEY			0xffffffe0	//확장키 인식값 
#define KEY_LEFT		0x4b
#define KEY_RIGHT		0x4d
#define KEY_UP			0x48
#define KEY_DOWN		0x50

//*********************************
//구조체 선언
//*********************************
struct STAGE {		//각 스테이지마다의 난이도 설정
	int	speed;	//숫자가 낮을수록 속도가 빠르다
	int stick_rate;		//막대가 나오는 확률 0~99 , 99면 막대기만 나옴
	int clear_line;
};

enum {
	BLACK,      /*  0 : 까망 */
	DARK_BLUE,    /*  1 : 어두운 파랑 */
	DARK_GREEN,    /*  2 : 어두운 초록 */
	DARK_SKY_BLUE,  /*  3 : 어두운 하늘 */
	DARK_RED,    /*  4 : 어두운 빨강 */
	DARK_VOILET,  /*  5 : 어두운 보라 */
	DARK_YELLOW,  /*  6 : 어두운 노랑 */
	GRAY,      /*  7 : 회색 */
	DARK_GRAY,    /*  8 : 어두운 회색 */
	BLUE,      /*  9 : 파랑 */
	GREEN,      /* 10 : 초록 */
	SKY_BLUE,    /* 11 : 하늘 */
	RED,      /* 12 : 빨강 */
	VOILET,      /* 13 : 보라 */
	YELLOW,      /* 14 : 노랑 */
	WHITE,      /* 15 : 하양 */
};

//*********************************
//전역변수선언
//*********************************
int level;
int ab_x, ab_y;	//화면중 블럭이 나타나는 좌표의 절대위치
int block_shape, block_angle, block_x, block_y;
int next_block_shape;
int score;
int lines;
char total_block[21][14];		//화면에 표시되는 블럭들
struct STAGE stage_data[10];
char block[7][4][4][4] = { //7은 블록의 종류, 4블록의 회전 상태, 4x4 회전상태에서 실제 블록 모양 격자
	//막대모양, 수정: 기존의 1행, 1열에서 회전하는 그림이 나오도록 순서대로 2열, 2행, 3열, 3행으로 변경
	//0도 회전                           90도 회전                          180도 회전                          270도 회전
	0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,	0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,	0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,	0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,

	//네모모양
	1,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,	1,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,	1,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,	1,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,

	//'ㅓ' 모양
	0,1,0,0,1,1,0,0,0,1,0,0,0,0,0,0,	1,1,1,0,0,1,0,0,0,0,0,0,0,0,0,0,	1,0,0,0,1,1,0,0,1,0,0,0,0,0,0,0,	0,1,0,0,1,1,1,0,0,0,0,0,0,0,0,0,

	//'ㄱ'모양
	1,1,0,0,0,1,0,0,0,1,0,0,0,0,0,0,	1,1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,	1,0,0,0,1,0,0,0,1,1,0,0,0,0,0,0,	0,0,1,0,1,1,1,0,0,0,0,0,0,0,0,0,

	//'ㄴ' 모양
	1,1,0,0,1,0,0,0,1,0,0,0,0,0,0,0,	1,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,	0,1,0,0,0,1,0,0,1,1,0,0,0,0,0,0,	1,1,1,0,0,0,1,0,0,0,0,0,0,0,0,0,

	//'Z' 모양
	1,1,0,0,0,1,1,0,0,0,0,0,0,0,0,0,	0,1,0,0,1,1,0,0,1,0,0,0,0,0,0,0,	1,1,0,0,0,1,1,0,0,0,0,0,0,0,0,0,	0,1,0,0,1,1,0,0,1,0,0,0,0,0,0,0,

	//'S' 모양
	0,1,1,0,1,1,0,0,0,0,0,0,0,0,0,0,	1,0,0,0,1,1,0,0,0,1,0,0,0,0,0,0,	0,1,1,0,1,1,0,0,0,0,0,0,0,0,0,0,	1,0,0,0,1,1,0,0,0,1,0,0,0,0,0,0

};
//*********************************
//함수 선언
//*********************************
int gotoxy(int x, int y);	//커서옮기기
void SetColor(int color);	//색표현
int init();					//각종변수 초기화
int show_cur_block(int shape, int angle, int x, int y);	//진행중인 블럭을 화면에 표시한다
int erase_cur_block(int shape, int angle, int x, int y);	//블럭 진행의 잔상을 지우기 위한 함수
int show_total_block();	//쌓여져있는 블럭을 화면에 표시한다.
int show_next_block(int shape);
int make_new_block();	//return값으로 block의 모양번호를 알려줌
int strike_check(int shape, int angle, int x, int y);	//블럭이 화면 맨 아래에 부닥쳤는지 검사 부닥치면 1을리턴 아니면 0리턴
int merge_block(int shape, int angle, int x, int y);	//블럭이 바닥에 닿았을때 진행중인 블럭과 쌓아진 블럭을 합침
int block_start(int shape, int* angle, int* x, int* y);	//블럭이 처음 나올때 위치와 모양을 알려줌
int move_block(int* shape, int* angle, int* x, int* y, int* next_shape);	//게임오버는 1을리턴 바닥에 블럭이 닿으면 2를 리턴
int rotate_block(int shape, int* angle, int* x, int* y);
int show_gameover();
int show_gamestat();
int show_logo();
int input_data();
int check_full_line();
void clear_input_buffer();//버퍼비우는 함수 추가(수정사항)


int main()
{
	int i;
	int is_gameover = 0;
	char keytemp;
	init();
	show_logo();
	while (1)
	{

		input_data();//레벨 선택하면 다음으로 넘어감
		show_total_block();//남은 블록 보이게
		block_shape = make_new_block();//블록 선택해서 넘기기
		next_block_shape = make_new_block();//다음블록 선택해서 넘기기
		show_next_block(next_block_shape);//다음블록 보이게 만들기
		block_start(block_shape, &block_angle, &block_x, &block_y);//블록 스타트
		show_gamestat();//게임 상황 보여주기
		for (i = 1; 1; i++)
		{
			if (_kbhit())//키입력이 있는지 확인
			{
				keytemp = _getche();//키 입력을 넘기기
				if (keytemp == EXT_KEY)//만약 방향키 말고 다른키라면
				{
					keytemp = _getche();//키입력 다시 받기
					switch (keytemp)//키입력에 따라 수행
					{
					case KEY_UP:		//회전하기
						// 수정: 회전 로직은 rotate_block() 함수에서
						rotate_block(block_shape, &block_angle, &block_x, &block_y);
						break;
					case KEY_LEFT:		//왼쪽으로 이동
						if (block_x > -1) // 1 >> 0으로 수정
							// 수정: 막대 블록은 4x4 마스크의 0번 열을 쓰는 대신 최대 3번 열을 쓰므로
							// 1에서 두 번 줄인 -1로, 충돌 여부는 strike_check() 함수에서 판단
						{
							erase_cur_block(block_shape, block_angle, block_x, block_y);
							block_x--;
							if (strike_check(block_shape, block_angle, block_x, block_y) == 1)//왼쪽으로 하는데 부딪히면 움직이지 않는다 다시 ++
								block_x++;

							show_cur_block(block_shape, block_angle, block_x, block_y);
						}
						break;
					case KEY_RIGHT:		//오른쪽으로 이동

						if (block_x < 14)
						{
							erase_cur_block(block_shape, block_angle, block_x, block_y);
							block_x++;
							if (strike_check(block_shape, block_angle, block_x, block_y) == 1)
								block_x--;
							show_cur_block(block_shape, block_angle, block_x, block_y);
						}
						break;
					case KEY_DOWN:		//아래로 이동=>원래 내려가는거 써서 한번더 호출
						is_gameover = move_block(&block_shape, &block_angle, &block_x, &block_y, &next_block_shape);
						show_cur_block(block_shape, block_angle, block_x, block_y);
						break;
					}
				}
				if (keytemp == 32)	//스페이스바를 눌렀을때
				{
					while (is_gameover == 0)
					{
						is_gameover = move_block(&block_shape, &block_angle, &block_x, &block_y, &next_block_shape);
					}
					show_cur_block(block_shape, block_angle, block_x, block_y);
				}
			}
			if (i % stage_data[level].speed == 0)
			{//내려오는 시간 설정 => 반복횟수가 증가할 수록 늦게 내려옴 => speed가 적을 수록 반복이 빠르게 되서 빨리 내려온다
				is_gameover = move_block(&block_shape, &block_angle, &block_x, &block_y, &next_block_shape);

				show_cur_block(block_shape, block_angle, block_x, block_y);
			}

			if (stage_data[level].clear_line == lines)	//클리어 스테이지
			{
				level++;
				lines = 0;
			}

			if (lines >= stage_data[level].clear_line)	//클리어 스테이지 (수정: >= 사용)
			{
				lines = lines - stage_data[level].clear_line; // 초과된 라인 수를 다음 레벨에 반영
				level++;
			}

			if (is_gameover == 1)
			{
				is_gameover = 0;
				show_gameover();
				SetColor(GRAY);
				break;
			}

			gotoxy(77, 23);
			Sleep(15);
			gotoxy(77, 23);
		}
		init();//블럭이 하나 끝날때 마다 초기화해주기
	}
	return 0;
}

int gotoxy(int x, int y)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	//현재 프로그램이 데이터를 출력할 콘솔 창의 고유한 식별자를 요청하고 그 식별자를 hConsole에 저장
	COORD pos; //window에서 정의한 구조체 x,y로 이루어져 있다
	pos.Y = y;
	pos.X = x;
	SetConsoleCursorPosition(hConsole, pos); //커서를 특정 위치로 옮기는거
	return 0;
}

void SetColor(int color)
{
	static HANDLE std_output_handle = GetStdHandle(STD_OUTPUT_HANDLE);//표준 출력 장치에 대한 유효한 값을 운영체제로부터 가져와서 std_output_handle에 넣는다.
	SetConsoleTextAttribute(std_output_handle, color); //색상을 동적으로 변경하는거
}

int init()
{
	int i, j;

	srand((unsigned)time(NULL));


	for (i = 0; i < 20; i++)
	{
		for (j = 0; j < 14; j++)
		{
			if ((j == 0) || (j == 13))
			{
				total_block[i][j] = 1;//화면에 표시되는 양옆에 블록 => 벽
			}
			else {
				total_block[i][j] = 0;//다른 블록이 표시될 자리
			}
		}
	}

	for (j = 0; j < 14; j++)			//화면의 제일 밑의 줄은 1로 채운다.
		total_block[20][j] = 1;//제일 밑에 벽

	//전역변수 초기화
	level = 0;
	lines = 0;
	ab_x = 5;//블록이 처음나오는 위치
	ab_y = 1;//블록이 처음나오는 위치

	//스테이지별 초기화
	stage_data[0].speed = 40;//블록 스피드 0으로 갈수폭 빠르다.
	stage_data[0].stick_rate = 20;	//막대모양이 나오는 확률
	stage_data[0].clear_line = 20; //목표 제거 해야할 줄수 수를 넘으면 다음단계로 넘어간다.
	stage_data[1].speed = 38;
	stage_data[1].stick_rate = 18;
	stage_data[1].clear_line = 20;
	stage_data[2].speed = 35;
	stage_data[2].stick_rate = 18;
	stage_data[2].clear_line = 20;
	stage_data[3].speed = 30;
	stage_data[3].stick_rate = 17;
	stage_data[3].clear_line = 20;
	stage_data[4].speed = 25;
	stage_data[4].stick_rate = 16;
	stage_data[4].clear_line = 20;
	stage_data[5].speed = 20;
	stage_data[5].stick_rate = 14;
	stage_data[5].clear_line = 20;
	stage_data[6].speed = 15;
	stage_data[6].stick_rate = 14;
	stage_data[6].clear_line = 20;
	stage_data[7].speed = 10;
	stage_data[7].stick_rate = 13;
	stage_data[7].clear_line = 20;
	stage_data[8].speed = 6;
	stage_data[8].stick_rate = 12;
	stage_data[8].clear_line = 20;
	stage_data[9].speed = 4;
	stage_data[9].stick_rate = 11;
	stage_data[9].clear_line = 99999;
	return 0;
}

int show_cur_block(int shape, int angle, int x, int y)
{
	int i, j;
	//블럭 모양에 따라 색깔을 다르게 설정
	switch (shape)
	{
	case 0:
		SetColor(RED);
		break;
	case 1:
		SetColor(BLUE);
		break;
	case 2:
		SetColor(SKY_BLUE);
		break;
	case 3:
		SetColor(WHITE);
		break;
	case 4:
		SetColor(YELLOW);
		break;
	case 5:
		SetColor(VOILET);
		break;
	case 6:
		SetColor(GREEN);
		break;
	}
	//블록처럼 보이게 ㅁ를 찍기
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			if ((j + y) < 0)
				continue;

			if (block[shape][angle][j][i] == 1)
			{
				gotoxy((i + x) * 2 + ab_x, j + y + ab_y);
				printf("■");

			}
		}
	}
	SetColor(BLACK);
	gotoxy(77, 23);//흔적을 남기지 않게 하기 위해
	return 0;
}

int erase_cur_block(int shape, int angle, int x, int y)
{
	//블록 print한뒤에 지우는 함수
	int i, j;
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			if (block[shape][angle][j][i] == 1)
			{
				gotoxy((i + x) * 2 + ab_x, j + y + ab_y);
				printf("  ");
				//break;

			}
		}
	}
	return 0;
}



int show_total_block()
{
	//남은 블록 보이게 하는거
	int i, j;
	SetColor(DARK_GRAY);
	for (i = 0; i < 21; i++)
	{
		for (j = 0; j < 14; j++)
		{
			if (j == 0 || j == 13 || i == 20)		//레벨에 따라 외벽 색이 변함
			{
				SetColor((level % 6) + 1);

			}
			else {
				SetColor(DARK_GRAY);
			}
			gotoxy((j * 2) + ab_x, i + ab_y);
			if (total_block[i][j] == 1)
			{
				printf("■");
			}
			else {
				printf("  ");
			}

		}
	}
	SetColor(BLACK);
	gotoxy(77, 23);
	return 0;
}

int make_new_block()
{
	//블록 만드는 함수
	int shape;
	int i;
	i = rand() % 100;
	if (i <= stage_data[level].stick_rate)		//막대기 나올확률 계산
		return 0;							//막대기 모양으로 리턴

	shape = (rand() % 6) + 1;		//shape에는 1~6의 값이 들어감
	show_next_block(shape);
	return shape;
}


int strike_check(int shape, int angle, int x, int y)
{
	int i, j;
	int block_dat;

	for (i = 0; i < 4; i++) // i는 4x4 블록 격자의 행
	{
		// *** 수정된 부분 ***
		// 검사할 실제 y 좌표 (y + i)가 0보다 작은지(화면 밖인지) 확인
		if (y + i < 0)
		{
			continue; // 이 행(i)은 화면 위에 있으니 검사할 필요 없음
		}

		for (j = 0; j < 4; j++) // j는 4x4 블록 격자의 열
		{
			// 1. 유효한 범위(1~12)에 있는지 먼저 확인합니다.
			if (((x + j) > 0) || ((x + j) < 13))
				// 2. 유효한 범위면, total_block에서 값을 가져옵니다.
				block_dat = total_block[y + i][x + j];
			else {
				// 3. 유효한 범위(1~12)가 아니면 (즉, 0, 13, -1, 14 등)
				//    모두 벽(1)으로 간주합니다.
				block_dat = 1;
				// *** (수정 끝) ***
			}

			// 밑에 남은 블록이랑 닿았는지 확인
			if ((block_dat == 1) && (block[shape][angle][i][j] == 1))
			{
				return 1;
			}
		}
	}
	return 0;
}

int merge_block(int shape, int angle, int x, int y)
{
	int i, j;
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			total_block[y + i][x + j] |= block[shape][angle][i][j];//비트연산자 or로 사용해서 있는지 확인
		}
	}
	check_full_line();//merge하고 check를 해서 line를 최신화
	show_total_block();

	return 0;
}

int block_start(int shape, int* angle, int* x, int* y)
{
	//블록 시작
	*x = 5;
	*y = -3;
	*angle = 0;
	return 0;
}

int show_gameover()
{
	SetColor(RED);
	gotoxy(15, 8);
	printf("┏━━━━━━━━━━━━━━━━━━━━━━━━━━┓");
	gotoxy(15, 9);
	printf("┃**************************┃");
	gotoxy(15, 10);
	printf("┃*        GAME OVER       *┃");
	gotoxy(15, 11);
	printf("┃**************************┃");
	gotoxy(15, 12);
	printf("┗━━━━━━━━━━━━━━━━━━━━━━━━━━┛");
	/*fflush(stdin);*///입력버퍼를 비우려면 개행문자를 기다린다 => 개행문자가 입력될때까지 넘어가지 않는 문제가 발생해서 버퍼비우기를 주석처리(수정사항)
	Sleep(3000);//Sleep(1000)에서 3000으로 바꿈 너무 빠른 느낌이라...

	//_getche();//엔터를 기다리지 않고 키보드로 문자를 입력하면 바로 콘솔에 문자 출력(수정사항)
	//없어도 될거 같아서 주석처리
	system("cls");

	return 0;
}

int move_block(int* shape, int* angle, int* x, int* y, int* next_shape)
{
	erase_cur_block(*shape, *angle, *x, *y);//값으로 넣어야해서 *를 붙이고 넣는다.

	(*y)++;	//블럭을 한칸 아래로 내림
	if (strike_check(*shape, *angle, *x, *y) == 1)
	{
		if (*y < 0)	//게임오버
		{
			return 1;
		}
		(*y)--;//충돌을 감지했으니까 y--해서 직전으로 되돌리기
		merge_block(*shape, *angle, *x, *y);//블록 합치기
		*shape = *next_shape;
		*next_shape = make_new_block();

		block_start(*shape, angle, x, y);	//angle,x,y는 포인터임
		show_next_block(*next_shape);
		return 2;//다른 블록이랑 부딪힐 경우
	}
	return 0;
}

int rotate_block(int shape, int* angle, int* x, int* y)
{
	int new_angle = (*angle + 1) % 4;
	int new_x = -1; // -1은 "아직 성공적인 위치를 찾지 못함"을 의미

	// 시도할 x축 이동(킥) 값의 목록입니다.
	// 0: 현재 위치
	// -1: 왼쪽 1칸
	// +1: 오른쪽 1칸
	// -2: 왼쪽 2칸
	// +2: 오른쪽 2칸
	int kick_offsets[] = { 0, -1, 1, -2, 2 };
	int i;

	// 5가지 경우의 수를 순서대로 테스트합니다.
	for (i = 0; i < 5; i++)
	{
		int test_x = *x + kick_offsets[i]; // 현재 x에 보정값을 더해 테스트

		// 1. 이 위치(test_x)에서 충돌이 없는지 검사 (0을 반환하면 성공)
		if (strike_check(shape, new_angle, test_x, *y) == 0)
		{
			// 2. 충돌이 없다! 이 위치로 결정
			new_x = test_x;
			break; // 성공했으니 for 루프 즉시 탈출
		}
	}

	// 3. for 루프가 끝난 후, new_x가 -1이 아니라면
	//    (즉, 5가지 시도 중 하나라도 성공했다면)
	if (new_x != -1)
	{
		// 이전 위치의 블록을 지웁니다.
		erase_cur_block(shape, *angle, *x, *y);

		*angle = new_angle; // 각도를 업데이트하고
		*x = new_x;         // *새로 찾은* x 위치를 업데이트합니다.

		// 새 위치에 블록을 다시 그립니다.
		show_cur_block(shape, *angle, *x, *y);
	}
	// (만약 5가지 시도 모두 실패했다면 new_x는 -1로 남아있을 것이고,
	//  if문을 통과하지 못해 아무 일도 일어나지 않습니다 = 회전 실패)
	return 0;
}

int check_full_line()
{
	int i, j, k;
	for (i = 0; i < 20; i++)
	{
		for (j = 1; j < 13; j++)
		{
			if (total_block[i][j] == 0)
				break;
		}
		if (j == 13)	//한줄이 다 채워졌음
		{
			lines++;
			show_total_block();
			SetColor(BLUE);
			gotoxy(1 * 2 + ab_x, i + ab_y);
			for (j = 1; j < 13; j++)
			{
				printf("□");
				Sleep(10);
			}//빈상자로 표시했다가
			gotoxy(1 * 2 + ab_x, i + ab_y);
			for (j = 1; j < 13; j++)
			{
				printf("  ");
				Sleep(10);
			}//지우기
			//한줄지우고 전부 내리기
			for (k = i; k > 0; k--)
			{
				for (j = 1; j < 13; j++)
					total_block[k][j] = total_block[k - 1][j];
			}
			for (j = 1; j < 13; j++)
				total_block[0][j] = 0;//지운칸 초기화
			score += 100 + (level * 10) + (rand() % 10);
			show_gamestat();
		}
	}
	return 0;
}

int show_next_block(int shape)
{
	//다음블록 보여주는 함수
	int i, j;
	SetColor((level + 1) % 6 + 1);
	for (i = 1; i < 7; i++)
	{
		gotoxy(33, i);
		for (j = 0; j < 6; j++)
		{
			if (i == 1 || i == 6 || j == 0 || j == 5)
			{
				printf(" ■"); // 수정사항: "■" -> " ■"
			}
			else {
				printf("  ");
			}

		}
	}
	show_cur_block(shape, 0, 16, 1);
	return 0;
}

int show_gamestat()
{
	//게임상태 보여주는 함수
	static int printed_text = 0;
	SetColor(GRAY);
	if (printed_text == 0)
	{
		gotoxy(35, 7);
		printf("STAGE");

		gotoxy(35, 9);
		printf("SCORE");

		gotoxy(35, 12);
		printf("LINES");


	}
	gotoxy(41, 7);
	printf("%d", level + 1);
	gotoxy(35, 10);
	printf("%10d", score);
	gotoxy(35, 13);
	printf("%10d", stage_data[level].clear_line - lines);
	return 0;
}


//버퍼 비우는 함수 추가 지금은 c 스타일 바꿀때 C++ 스타일로 바꿔야 함
void clear_input_buffer() {
	int c;
	// 개행문자('\n')가 나올 때까지 읽고 버리는 동작 자체가 버퍼를 비웠는지 확인하는 과정임.
	while ((c = getchar()) != '\n' && c != EOF) {
		/* 버퍼가 비워질 때까지 반복 */
	}
}

int input_data()
{
	//게임 설명
	int i = 0;
	int result;//문자를 예외처리하기 위한 result 변수 추가(수정사항)
	SetColor(GRAY);
	gotoxy(10, 7);
	printf("┏━━━━━━━━━━<GAME KEY>━━━━━━━━┓");
	Sleep(10);
	gotoxy(10, 8);
	printf("┃ UP   : Rotate Block        ┃");
	Sleep(10);
	gotoxy(10, 9);
	printf("┃ DOWN : Move One-Step Down  ┃");
	Sleep(10);
	gotoxy(10, 10);
	printf("┃ SPACE: Move Bottom Down    ┃");
	Sleep(10);
	gotoxy(10, 11);
	printf("┃ LEFT : Move Left           ┃");
	Sleep(10);
	gotoxy(10, 12);
	printf("┃ RIGHT: Move Right          ┃");
	Sleep(10);
	gotoxy(10, 13);
	printf("┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛");
	//여기 조금 위아래 크기 맞추기 수정완료(수정사항)

	//방법 1 : 다른건 다 처리하지만 엔터만 들어가는건 처리를 하지 못함
	//값이 잘못들어가는거 확인하는 변수result추가
	/*while (i<1 || i>8 || result != 1)//조건에도 추가(수정사항)
	{
		gotoxy(10,3);
		printf("Select Start level[1-8]:       \b\b\b\b\b\b\b");
		result=scanf_s("%d",&i);//(수정사항)
		if (result != 1) {
			//잘못된 입력이 들어가면 버퍼 비우기
			//나중에 C++ 스타일 버퍼 비우기로 바꾸면 해결
			/*if (cin.fail()) {
			cin.clear();
			cin.ignore(1024, '\n');
			이렇게 바꾸면 됨*//*
			clear_input_buffer();
		}	//(수정사항)
	}*/

	//아래는 방법 2 : 엔터만 입력할 시 문제 해결(수정사항)
	char buffer[100];//버퍼 저장하기 위한 변수선언
	char extracted_char;//입력값 하나만 받아오기 위한 변수

	while (i < 1 || i>8) {

		gotoxy(10, 3);
		printf("Select Start level[1-8]:       \b\b\b\b\b\b\b");

		// 1. fgets로 한 줄 전체를 입력 받음
		if (fgets(buffer, 100, stdin) == NULL) {
			continue;
		}

		// 2. 입력 길이 확인
		// strlen(buffer)는 \n을 포함한 길이입니다.
		// - 길이가 2인지 확인: [입력 문자] + [\n] = 2글자
		// - 예를 들어, 'a'를 입력하면: buffer = "a\n\0" 이고 strlen(buffer)는 2입니다.

		if (strlen(buffer) == 2) {
			// 3. 길이가 2인 경우, 첫 번째 문자가 유효한 입력 문자가 됨
			extracted_char = buffer[0];
			//아스키 코드 값으로 들어가서 '0'을빼주어 원래 숫자값으로 만들어준다.
			if (extracted_char >= '1' && extracted_char <= '8') {
				i = extracted_char - '0';
			}
			else
			{
				continue;
			}
			// 4. 추출 성공
		}
		else if (strlen(buffer) == 1 && buffer[0] == '\n') {
			// 5. 엔터만 입력된 경우: 길이가 1이고 내용이 '\n'만 있는 경우 (일부 시스템 환경)
			// Windows에서는 \r\n으로 들어오거나, 버퍼 상태에 따라 \n만 남는 경우가 있음
			// 이 로직은 보통 strlen == 2 로직에 포함되나, 안전을 위해 추가
			continue;
		}
		else {
			continue;
			// fgets는 한 줄을 모두 읽었으므로 별도 버퍼 비우기는 필요 없습니다.
		}
	}
	level = i - 1;
	system("cls");
	return 0;
}

int show_logo()
{
	//로고 보여주는 함수
	int i, j;
	gotoxy(13, 3);
	printf("┏━━━━━━━━━━━━━━━━━━━━━━━┓");
	Sleep(100);
	gotoxy(13, 4);
	printf("┃◆◆◆  ◆◆◆  ◆◆◆   ◆◆     ◆   ◆◆◆ ┃");
	Sleep(100);
	gotoxy(13, 5);
	printf("┃  ◆    ◆        ◆     ◆ ◆    ◆   ◆     ┃");
	Sleep(100);
	gotoxy(13, 6);
	printf("┃  ◆    ◆◆◆    ◆     ◆◆     ◆     ◆   ┃");
	Sleep(100);
	gotoxy(13, 7);
	printf("┃  ◆    ◆        ◆     ◆ ◆    ◆       ◆ ┃");
	Sleep(100);
	gotoxy(13, 8);
	printf("┃  ◆    ◆◆◆    ◆     ◆  ◆   ◆   ◆◆◆ ┃");
	Sleep(100);
	gotoxy(13, 9);
	printf("┗━━━━━━━━━━━━━━━━━━━━━━━┛");

	gotoxy(28, 20);
	printf("Please Press Any Key~!");

	for (i = 0; i >= 0; i++) {
		if (i % 40 == 0)
		{


			for (j = 0; j < 5; j++)
			{
				// 수정: 18 >> 17
				// show_cur_block(..., x=6, ...)를 호출하면, 실제 화면에 그려지는 
				// X좌표는 (i + x)*2 + ab_x = (0 + 6)*2 + 5 = 17부터 시작하므로 17로 수정
				gotoxy(17, 14 + j);
				// 수정: show_logo 함수에서 로고 애니메이션을 위해 블록을 지울 때, 
//				// gotoxy의 X좌표가 18로 설정되어 있었습니다.
				// 첫 번째 블록이 그려지는 실제 X좌표는(0 + 6) * 2 + 5 = 17이므로, 
				// 17열의 블록이 지워지지 않고 남아 색상이 겹치는 버그가 발생
				gotoxy(17, 14 + j);
				printf("                                                          ");


			}
			show_cur_block(rand() % 7, rand() % 4, 6, 14);
			show_cur_block(rand() % 7, rand() % 4, 12, 14);
			show_cur_block(rand() % 7, rand() % 4, 19, 14);
			show_cur_block(rand() % 7, rand() % 4, 24, 14);
		}
		if (_kbhit())
			break;
		Sleep(30);
	}

	_getche();
	system("cls");

	return 0;
}