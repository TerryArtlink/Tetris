// Tetris Season 1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <time.h>

#define CELL 20
#define ROWS 30
#define COLS 25

//////////////////////////全局变量/////////////////////////////
//h 是类型描述，表示句柄(handle)， Wnd 是变量对象描述，表示窗口，所以hWnd 表示窗口句柄
HWND hwnd;  //保存窗口句柄












int cur_left, cur_top; //记录方块当前的位置
int width_block, height_block; //方块的宽度和高度




static byte *block = NULL; //方块， 方块为随机大小，采用动态分配内存方式， 所以这里是指针变量。

///////////////////////////////////////////////////////////////

//LRESULT指的是从窗口程序或者回调函数返回的32位值
//wParam和lParam 这两个是Win16系统遗留下来的产物，在Win16API中WndProc有两个参数。
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


void color(int a) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), a);
}

int main() {

	//GetModuleHandle是一个计算机函数，功能是获取一个应用程序或动态链接库的模块句柄。只有在当前进程的场景中，这个句柄才会有效。
	HINSTANCE hInstance = GetModuleHandle(NULL);
	//TCHAR 通用的char
	TCHAR szAppName[] = TEXT("tetris");
	//创建一个MSG的msg结构体
	MSG msg;

	WNDCLASS wc;
	//CS_HREDRAW 当水平长度改变或移动窗口时，重画整个窗口
	//CS_VREDRAW 当垂直长度改变或移动窗口时, 重画整个窗口
	wc.style = CS_HREDRAW | CS_VREDRAW;
	//指向窗口过程
	wc.lpfnWndProc = WndProc;
	//指定紧随在 WNDCLASS 数据结构后分配的字节数。系统将其初始化为零。
	wc.cbClsExtra = 0;
	//指定紧随在窗口实例之后分配的字节数，系统将其初始化为零。如果应用程序正在用WNDCLASS结构注册一个在RC
	wc.cbWndExtra = 0;
	//标识了该窗口类的窗口过程所在的模块实例的句柄，不能为NULL。
	wc.hInstance = hInstance;
	//标识了该窗口类的图标。hIcon字段必须是一个图标的句柄；若hIcon字段为NULL，那么系统将提供一个默认的图标。
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	//标识该窗口类的光标，hCursor必须是一个光标资源的句柄。若hCursor字段为NULL，则无论何时鼠标移到应用程序窗口时，应用程序必须显式设置光标形状。
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	// 标识了该窗口类的背景画刷。hbrBackground字段必须是用于绘制背景的物理刷子的句柄，或者是一个颜色的值。如果给出一个颜色的值，它必须是下面列出的标准系统颜色之一（系统将对所选颜色加1）。如果给出了颜色值，它必须是转换成下列的HBRUSH类型之一的颜色:
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	//指向NULL结束的字符串，该字符串描述菜单的资源名，如同在资源文件里显示的名字一样。若使用一个整数标识菜单，可以使用MAKEINTRESOURCE宏。如果lpszMenuName为NULL，那么该窗口类的窗口将没有默认菜单。
	wc.lpszMenuName = NULL;
	//指向NULL结束的字符串，或者是一个原型(atom)。若该参数是一个原型，它必须是一个有先前调用RegisterClass或者 RegisterClassEx函数产生的类原型。类原型必须作为lpszClassName的低字，高字必须为0.若lpszClassName是一个字符串，它描述了窗口类名。这个类名可以是由RegisterClass或RegisterClassEx注册的名字，或者是任何预定义的控件类名。
	wc.lpszClassName = szAppName;

	if (!RegisterClass(&wc)) {
		printf("RegisterClass occur errors!");
		return 0;
	}

	hwnd = CreateWindow(szAppName, TEXT("-------- 俄罗斯方块 --------"),
		WS_OVERLAPPEDWINDOW,
		0, 0, 0, 0,
		NULL,
		NULL,
		hInstance,
		NULL);
	
	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);
	//GetMessage是从调用线程的消息队列里取得一个消息并将其放于指定的结构。此函数可取得与指定窗口联系的消息和由PostThreadMessage寄送的线程消息。此函数接收一定范围的消息值。GetMessage不接收属于其他线程或应用程序的消息。获取消息成功后，线程将从消息队列中删除该消息。函数会一直等待直到有消息到来才有返回值。
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

void DrawPanel(HDC hdc)  //draw game panel
{
	color(13);
	int x, y;
	RECT rect;

	for (y = 0; y < ROWS; y++) {
		for (x = 0; x < COLS; x++) {
			//计算方块的边框范围
			rect.top = y*CELL + 1;
			rect.bottom = (y + 1) *CELL - 1;
			rect.left = x*CELL + 1;
			rect.right = (x + 1) *CELL - 1;
			/*
			FrameRect函数功能：该函数用指定的画刷为指定的矩形画边框。边框的宽和高总是一个逻辑单元。
　　			  函数原型：int frameRect(HDC hdc, CONST RECT *lprc, HBRUSH hbr)；
　　			  参数：
　　			  hdc：将要画边框的设备环境句柄。
　　			  lprc：指向包含矩形左上角和右上角逻辑坐标的结构RECT的指针。
　　			  hbr：用于画边框的画刷句柄。
			*/
			FrameRect(hdc, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));
		}
	}
}

















//输出方块
bool ExportBlock() {  
	int sel;
	color(12);
	if (block) {
		free(block); //释放之前分配的内存
		block = NULL;
	}

	sel = rand() % 7;
	switch (sel) {
	case 0: //水平条
		width_block = 4;
		height_block = 1;
		block = (byte *)malloc(sizeof(byte) *width_block*height_block);
		*(block + 0) = 1; //可以理解为*(block +0*width_block+0)=1, 即第一行的第一个方格，下面同理
		*(block + 1) = 1; //*(block+0*width_block+1)=1
		*(block + 2) = 1; //*(block+0*width_block+2)=1
		*(block + 3) = 1; //*(block+0*width_block+3)=1

		cur_top = 0 - height_block;
		cur_left = (COLS - width_block) / 2;
		break;
	case 1: //三角
		color(13);
		width_block = 3;
		height_block = 2;
		block = (byte *)malloc(sizeof(byte) *width_block*height_block);
		*(block + 0) = 0; //可以理解为*(block+0*width_block+0)=0,即第一行的第一个方格，下面同理
		*(block + 1) = 1; //*(block+0*width_block+1)=1
		*(block + 2) = 0; //*(block+1*width_block+2)=0
		*(block + 3) = 1; //*(block+1*width_block+0)=1 ,第二行开始
		*(block + 4) = 1; //*(block+1*width_block+1)=1
		*(block + 5) = 1; //*(block+1*width_block+2)=1

		cur_top = 0 - height_block;
		cur_left = (COLS - width_block) / 2;
		break;
	}
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	HDC hdc;
	PAINTSTRUCT ps;
	//TCHAR szBuffer[1024];

	switch (message) {
	case WM_CREATE:
		MoveWindow(hwnd, 400, 10, CELL*COLS + 8, CELL*ROWS + 32, FALSE); //补齐宽度和高度
		srand(time(NULL));
		ExportBlock();

		
	}
}