#include <windows.h>
#include <time.h>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>

typedef struct _BODY
{
	int x;
	int y;
}BODY;

#define MARKTITLE "2345-2356-7865-5345-2345-2345-6574-9867"         //窗口标志
#define SPEED 150													//蛇的初始速度
#define INITBODYLENGTH 3											//初始化蛇的长度
#define INITPOS_X 7													//初始化蛇的横坐标
#define INITPOS_Y 10												//初始化蛇的纵坐标
#define RIGHT 1
#define LEFT 2
#define UP 3
#define DOWN 4

COORD Food;															//食物的坐标
BODY Snake[512];													//保存蛇的每一段的结构体
unsigned int score = 0;												//记录分数
int len = INITBODYLENGTH;											//记录蛇的长度

//获取控制台的句柄
HWND GetConsoleHwnd(void)
{
	HWND hwndFound;
	char NewWindowTitle[256];
	char OldWindowTitle[256];

	GetConsoleTitle(OldWindowTitle, 256);
	sprintf(NewWindowTitle,"%s", MARKTITLE);
	SetConsoleTitle(NewWindowTitle);

	Sleep(40);

	hwndFound=FindWindow(NULL, NewWindowTitle);
	SetConsoleTitle(OldWindowTitle);

	return(hwndFound);
}

//移动光标
void GotoXY(int x, int y)
{
	HANDLE CurPos;
	COORD NextPos;

	NextPos.X = x;
	NextPos.Y = y;

	CurPos = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(CurPos, NextPos);
}

//输出地图
void InitMap(void)
{
	int i;

	for (i=0; i<80; i++)
		printf("*");

	for (i=0; i<22; i++)
		printf("|\t\t\t\t\t\t\t\t\t       |");

	for (i=0; i<80; i++)
		printf("*");
	printf("得分: 0\t\t\t\t\t\t\t\t   制作者:Zzy");
}

//初始化蛇身
void InitBody(void)
{
	int i, Pos_x;
	Pos_x = INITPOS_X;

	for (i=0; i<INITBODYLENGTH; i++, Pos_x-=2)
	{
		GotoXY(Pos_x, INITPOS_Y);
		printf("■");
		Snake[i].x = Pos_x;
		Snake[i].y = INITPOS_Y;
	}
}

//将读入的按键转化为方向
int GetDirect(int cur, char temp)
{
	switch(temp)
	{
	case 's':
	case 'S':
		{
			if (cur != UP)
				return DOWN;
		}break;

	case 'a':
	case 'A':
		{
			if (cur != RIGHT)
				return LEFT;
		}break;

	case 'd':
	case 'D':
		{
			if (cur != LEFT)
				return RIGHT;
		}break;

	case 'w':
	case 'W':
		{
			if (cur != DOWN)
				return UP;
		}break;
	}

	return cur;
}

//控制蛇的移动,本质就是添头去尾
void Move(int direct, int templen)
{
	templen--;

	if (Snake[templen].x!=0 && Snake[templen].y!=0)
	{
		GotoXY(Snake[templen].x, Snake[templen].y);
		printf(" ");
	}

	for(templen; templen>0; templen--)
	{
		Snake[templen].x = Snake[templen-1].x;
		Snake[templen].y = Snake[templen-1].y;
	}

	switch(direct)
	{
	case LEFT:
		{
			(Snake[0].x)-=2;			//一个正方形占两个位
		}break;
	case RIGHT:
		{
			(Snake[0].x)+=2;
		}break;
	case UP:
		{
			(Snake[0].y--);
		}break;
	case DOWN:
		{
			(Snake[0].y++);
		}break;
	}
	GotoXY(Snake[0].x, Snake[0].y);
	printf("■");
}

//随机生产食物
void MakeFood(void)
{
	int i;
	Food.X = (rand()%39)*2+1;				//要考虑一个食物的宽度为2,防止和墙重叠,并
	Food.Y = rand()%22 + 1;					//且横坐标必须为奇数，否则蛇头触碰不到

	for (i=0; i<len; i++)
	{
		if (Snake[i].x==Food.X && Snake[i].y==Food.Y)
			MakeFood();
	}
	GotoXY(Food.X, Food.Y);
	printf("●");
}

//判断蛇是否吃到了食物
int IfEatFood(void)
{
	if (Snake[0].x==Food.X && Snake[0].y==Food.Y)
		return 1;
	else
		return 0;
}

//判断是否死亡
int IfDead(void)
{
	int i;

	if (Snake[0].y==0 || Snake[0].y==23 || Snake[0].x==-1 || Snake[0].x==79)
		return 1;

	for (i=1; i<len; i++)
	{
		if (Snake[0].x==Snake[i].x && Snake[0].y==Snake[i].y)
			return 1;
	}
	return 0;
}

//输出文件中的内容
void PutOutData(HWND hwnd)
{
	FILE * fp;
	char str[128];
	char text[12800] = {0};

	fp = fopen("rsc\\snake.dat", "r");
	if (fp == NULL)
	{
		MessageBox(NULL, TEXT("文件打开失败!"), TEXT("错误"), MB_OK|MB_ICONERROR);
		exit(0);
	}
	fgets(str, sizeof(str), fp);
	while (!feof(fp))
	{
		strcat(text, str);
		fgets(str, sizeof(str), fp);
	}
	MessageBox(hwnd, text, TEXT("游戏说明"), MB_OK);
	fclose(fp);
}

//读取最高分
int GetHighScore(void)
{
	double score = 0;
	FILE * fp;

	fp = fopen("rsc\\snakerank.dat", "rb");
	if (fp == NULL)
	{
	    fp = fopen("rsc\\snakerank.dat", "wb");
	    if (fp == NULL)
        {
            MessageBox(NULL, TEXT("文件打开失败!"), TEXT("错误"), MB_OK|MB_ICONERROR);
            exit(0);
        }
	}
	fread(&score, sizeof(double), 1, fp);
	fclose(fp);

	return (int)score;
}

//保存最高分
void SaveHighScore(int score)
{
	FILE *fp;
	double temp = (double)score;

	fp = fopen("rsc\\snakerank.dat", "wb");
	if (fp == NULL)
	{
		MessageBox(NULL, TEXT("文件打开失败!"), TEXT("错误"), MB_OK|MB_ICONERROR);
		exit(0);
	}
	fwrite(&temp, sizeof(temp), 1, fp);
	fclose(fp);
}

//改变蛇的速度
void ChangeSpeed(int temp, int *ps)
{
		*ps = SPEED - temp;
}

//倒计时
void CountDown(void)
{
	int i;
	for (i=0; i<3; i++)
	{
		GotoXY(40, 12);
		printf("%d", 3-i);
		Sleep(1000);
	}
	GotoXY(40, 12);
	printf(" ");
}

int main(void)
{
	unsigned long PrevTime = clock();
	int TempScore = 20;
	int direct = RIGHT;
	int HighScore;
	int speed = SPEED;
	char str[128];

	HWND hwnd = GetConsoleHwnd();
	PutOutData(hwnd);
	srand(time(NULL));
	system("color b");
	memset(Snake, 0, sizeof(Snake));
	InitMap();
	InitBody();
	MakeFood();
	CountDown();

	while (1)
	{
		if (kbhit())
		{
			direct = GetDirect(direct, getch());
		}

		Move(direct, len);

		if ( IfEatFood() )
		{
			score += TempScore;
			MakeFood();
			len++;
			TempScore = 20;
			PrevTime = clock();
			GotoXY(6, 24);
			printf("%d", score);
		}

		if ( IfDead() )
		{
			HighScore = GetHighScore();
			if (HighScore < score)
			{
				HighScore = score;
				SaveHighScore(score);
			}

			sprintf(str, "\t游戏结束\t\r\n得分:\t%d\r\n最高分:\t%d", score, HighScore);
			MessageBox(NULL, str, TEXT("GameOver"), MB_OK);
			exit(0);
		}

		//每过1s吃到食物的分减1
		if ((clock()-PrevTime) >= 1000)
		{
			TempScore--;
			PrevTime = clock();
		}

		if (speed>100 && !(len%10))
			ChangeSpeed(len, &speed);
		Sleep(speed);
	}

	return 0;
}
