#define _CRT_SECURE_NO_WARNINGS 1
#include<stdio.h>
#include<algorithm>
#include <stdlib.h>
#include <time.h>
#include<graphics.h>//ͼ�λ��ƿ�
#include<string>
#include<iostream>
#include<queue>
#include<conio.h>
#include<vector>
#include<math.h>
#include<stack>
using namespace std;

#define SIZE 15 //������С��Ԫ
#define NUMX 64
#define NUMY 56//����xy�����С��Ԫ����
#define MAXX NUMX*SIZE
#define MAXY NUMY*SIZE
#define PI 3.1415926
#define RIGHT 77
#define LEFT 75
#define UP 72
#define DOWN 80
#define ENTER 13
#define W 119
#define A 97
#define S 115
#define D 100
#define KONG 32
#define ESC 27//��ȡ��ֵ
#define KEY_DOWN(vKey) ((GetAsyncKeyState(vKey) & 0x8000) ? 1:0)//�����̰��º���

void game();//��Ϸ�������
void PrintTankNum();