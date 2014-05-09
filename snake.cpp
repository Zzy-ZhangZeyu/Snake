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

#define MARKTITLE "2345-2356-7865-5345-2345-2345-6574-9867"         //���ڱ�־
#define SPEED 150													//�ߵĳ�ʼ�ٶ�
#define INITBODYLENGTH 3											//��ʼ���ߵĳ���
#define INITPOS_X 7													//��ʼ���ߵĺ�����
#define INITPOS_Y 10												//��ʼ���ߵ�������
#define RIGHT 1
#define LEFT 2
#define UP 3
#define DOWN 4

COORD Food;															//ʳ�������
BODY Snake[512];													//�����ߵ�ÿһ�εĽṹ��
unsigned int score = 0;												//��¼����
int len = INITBODYLENGTH;											//��¼�ߵĳ���

//��ȡ����̨�ľ��
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

//�ƶ����
void GotoXY(int x, int y)
{
	HANDLE CurPos;
	COORD NextPos;

	NextPos.X = x;
	NextPos.Y = y;

	CurPos = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(CurPos, NextPos);
}

//�����ͼ
void InitMap(void)
{
	int i;

	for (i=0; i<80; i++)
		printf("*");

	for (i=0; i<22; i++)
		printf("|\t\t\t\t\t\t\t\t\t       |");

	for (i=0; i<80; i++)
		printf("*");
	printf("�÷�: 0\t\t\t\t\t\t\t\t   ������:Zzy");
}

//��ʼ������
void InitBody(void)
{
	int i, Pos_x;
	Pos_x = INITPOS_X;

	for (i=0; i<INITBODYLENGTH; i++, Pos_x-=2)
	{
		GotoXY(Pos_x, INITPOS_Y);
		printf("��");
		Snake[i].x = Pos_x;
		Snake[i].y = INITPOS_Y;
	}
}

//������İ���ת��Ϊ����
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

//�����ߵ��ƶ�,���ʾ�����ͷȥβ
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
			(Snake[0].x)-=2;			//һ��������ռ����λ
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
	printf("��");
}

//�������ʳ��
void MakeFood(void)
{
	int i;
	Food.X = (rand()%39)*2+1;				//Ҫ����һ��ʳ��Ŀ��Ϊ2,��ֹ��ǽ�ص�,��
	Food.Y = rand()%22 + 1;					//�Һ��������Ϊ������������ͷ��������

	for (i=0; i<len; i++)
	{
		if (Snake[i].x==Food.X && Snake[i].y==Food.Y)
			MakeFood();
	}
	GotoXY(Food.X, Food.Y);
	printf("��");
}

//�ж����Ƿ�Ե���ʳ��
int IfEatFood(void)
{
	if (Snake[0].x==Food.X && Snake[0].y==Food.Y)
		return 1;
	else
		return 0;
}

//�ж��Ƿ�����
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

//����ļ��е�����
void PutOutData(HWND hwnd)
{
	FILE * fp;
	char str[128];
	char text[12800] = {0};

	fp = fopen("rsc\\snake.dat", "r");
	if (fp == NULL)
	{
		MessageBox(NULL, TEXT("�ļ���ʧ��!"), TEXT("����"), MB_OK|MB_ICONERROR);
		exit(0);
	}
	fgets(str, sizeof(str), fp);
	while (!feof(fp))
	{
		strcat(text, str);
		fgets(str, sizeof(str), fp);
	}
	MessageBox(hwnd, text, TEXT("��Ϸ˵��"), MB_OK);
	fclose(fp);
}

//��ȡ��߷�
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
            MessageBox(NULL, TEXT("�ļ���ʧ��!"), TEXT("����"), MB_OK|MB_ICONERROR);
            exit(0);
        }
	}
	fread(&score, sizeof(double), 1, fp);
	fclose(fp);

	return (int)score;
}

//������߷�
void SaveHighScore(int score)
{
	FILE *fp;
	double temp = (double)score;

	fp = fopen("rsc\\snakerank.dat", "wb");
	if (fp == NULL)
	{
		MessageBox(NULL, TEXT("�ļ���ʧ��!"), TEXT("����"), MB_OK|MB_ICONERROR);
		exit(0);
	}
	fwrite(&temp, sizeof(temp), 1, fp);
	fclose(fp);
}

//�ı��ߵ��ٶ�
void ChangeSpeed(int temp, int *ps)
{
		*ps = SPEED - temp;
}

//����ʱ
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

			sprintf(str, "\t��Ϸ����\t\r\n�÷�:\t%d\r\n��߷�:\t%d", score, HighScore);
			MessageBox(NULL, str, TEXT("GameOver"), MB_OK);
			exit(0);
		}

		//ÿ��1s�Ե�ʳ��ķּ�1
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
