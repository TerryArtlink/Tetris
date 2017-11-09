/*
only test,  never used.
*/
/*
#include "stdafx.h"
#include <iostream>
#include "windows.h"

using namespace std;

void Pos(int x, int y);

// 颜色函数
void color(int a) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), a);//设置控制台颜色
}

int main() {
	for (int i = 0; i < 16; i++) {
		cout << "数字 ：" << i << "的颜色是" << endl;
		color(i);
		//cout << "颜色"<< std::endl;
	}
	Pos(3, 4);
	system("pause");
}

void Pos(int x, int y)//位置函数
{
	COORD pos;
	pos.X = 2*x;
	pos.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos); //设置光标位置
}
*/

