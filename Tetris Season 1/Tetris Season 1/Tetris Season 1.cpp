// Tetris Season 1.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <windows.h>
#include <time.h>
#define CELL 20
#define ROWS 30
#define COLS 25
//升级所需分数值
#define SCORE_LEVEL_INC 80
#define ID_TIMER 1

byte g_panel[ROWS][COLS] = {0};
//////////////////////////全局变量/////////////////////////////
//h 是类型描述，表示句柄(handle)， Wnd 是变量对象描述，表示窗口，所以hWnd 表示窗口句柄
HWND hwnd;  //保存窗口句柄

int score = 0; //分数
int level = 0; //级数
int interval_unit = 25; //随级数递增的时间间隔增量 
int interval_base = 300; //时间间隔基量
int width_block, height_block; //方块的宽带和高度

bool isPause = false; //暂停标识
UINT timer_id = 0; //保存计时器ID


int cur_left, cur_top; //记录方块当前的位置

static byte *block = NULL; //方块， 方块为随机大小，采用动态分配内存方式， 所以这里是指针变量。

//=======================函数声明区域=================================

//LRESULT指的是从窗口程序或者回调函数返回的32位值
//wParam和lParam 这两个是Win16系统遗留下来的产物，在Win16API中WndProc有两个参数。
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void DrawPanel(HDC hdc); //绘制表格
void RefreshPanel(HDC hdc); //刷新面板
void DoDownShift(HDC hdc); //下移
void DoLeftShift(HDC hdc); //左移
void DoRightShift(HDC hdc); //右移
void DoAccelerate(HDC hdc); //加速
void DoRedirection(HDC hdc); //改变方向
void ClearRow(HDC hdc); //消行
bool ExportBlock(); //输出方块，
					//该函数会直接修改全局变量block， width_block, height_block,
					//cur_left 和 cur_top
bool IsTouchBottom(HDC hdc); //判断是否达到底部

void color(int a) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), a);
}

bool IsTouchBottom(HDC hdc);
void DoDownShift(HDC hdc);  //下移

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
	case 2: //左横折
		color(10);
		width_block = 3;
		height_block = 2;
		block = (byte *)malloc(sizeof(byte) *width_block*height_block);
		*(block + 0) = 1; //可以理解为*(block+0*width_block+0)=1, 下面同理
		*(block + 1) = 0; //*(block+0*width_block+1)=0
		*(block + 2) = 0; //*(block+1*width_block+2)=0
		*(block + 3) = 1; //*(block+1*width_block+0)=1
		*(block + 4) = 1; //*(block+1*width_block+1)=1
		*(block + 5) = 1; //*(block+1*width_block+2)=1

		cur_top = 0 - height_block;
		cur_left = (COLS - width_block) / 2;
		break;
	case 3: //右横折
		color(9);
		width_block = 3;
		height_block = 2;
		block = (byte *)malloc(sizeof(byte) * width_block*height_block);
		*(block + 0) = 0; //可以理解为*(block+0*windth_block+1)=0
		*(block + 1) = 0; //*(block+0*width_block+1)=1
		*(block + 2) = 1; //*(block+1*width_block+2)=1
		*(block + 3) = 1; //*(block+1*width_block+0)=1
		*(block + 4) = 1; //*(block+1*width_block+1)=1
		*(block + 5) = 1; //*(block+1*width_block+2)=1

		cur_top = 0 - height_block;
		cur_left = (COLS - width_block) / 2;
		break;
	case 4: //左闪电
		color(9);
		width_block = 3;
		height_block = 2;
		block = (byte *)malloc(sizeof(byte) *width_block*height_block);
		*(block + 0) = 1; //可以理解为*(block+0*width_block+0)=1, 下面同理
		*(block + 1) = 1; //*(block+0*width_block+1)=1
		*(block + 2) = 0; //*(block+0*width_block+2)=0
		*(block + 3) = 0; //*(block+1*width_block+0)=0
		*(block + 4) = 1; //*(block+1*width_block+1)=1
		*(block + 5) = 1; //*(block+1*width_block+2)=0
		cur_top = 0 - height_block;
		cur_left = (COLS - width_block) / 2;
		break;
	case 5: //右闪电
		color(8);
		width_block = 3;
		height_block = 2;
		block = (byte *)malloc(sizeof(byte) *width_block*height_block);
		*(block + 0) = 0; //可以理解为*(block+0*width_block+0)=0, 下面同理
		*(block + 1) = 1; //*(block+0*width_block+1)=1
		*(block + 2) = 1; //*(block+0*width_block+2)=1
		*(block + 3) = 1; //*(block+1*width_block+0)=1
		*(block + 4) = 1; //*(block+1*width_block+1)=1
		*(block + 5) = 0; //*(block+1*width_block+2)=0

		cur_top = 0 - height_block;
		cur_left = (COLS - width_block) / 2;
		break;
	case 6: //石头
		color(13);
		width_block = 2;
		height_block = 2;
		block = (byte *)malloc(sizeof(byte) *width_block*height_block);
		*(block + 0) = 1; //可以理解为*(block+0*width_block+0)=1,  下面同理
		*(block + 1) = 1; //*(block+0*width_block+1)=1
		*(block + 2) = 1; //*(block+1*width_block+0)=1
		*(block + 3) = 1; //*(block+1*width_block+1)=1

		cur_top = 0 - height_block;
		cur_left = (COLS - width_block) / 2;
		break;
	}
	return block != NULL;
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

		timer_id = SetTimer(hwnd, ID_TIMER, interval_base - level*interval_unit, NULL);
		return 0;
	case WM_TIMER:
		//该函数检索一指定窗口的客户区域或整个屏幕的显示设备上下文环境的句柄，以后可以在GDI函数中使用该句柄来在设备上下文环境中绘图。
		hdc = GetDC(hwnd);
		DoDownShift(hdc);
		ReleaseDC(hwnd, hdc);
		return 0;
	case WM_KEYDOWN:
		hdc = GetDC(hwnd);
		switch (wParam) {
		case VK_LEFT: //左移
			if (!isPause) DoLeftShift(hdc);
			break;
		case VK_RIGHT: //右移
			if (!isPause) DoRightShift(hdc);
			break;
		case VK_UP: //转向
			if (!isPause) DoRedirection(hdc);
			break;
		case VK_DOWN: //加速
			if (!isPause) DoAccelerate(hdc);
			break;
		case VK_SPACE: //暂停
			isPause = !isPause;
			if (isPause) {
				if (timer_id) KillTimer(hwnd, ID_TIMER);
				timer_id = 0;
			}
			else {
				timer_id = SetTimer(hwnd, ID_TIMER, interval_base - level * interval_unit, FALSE);
			}
			break;
		}
		ReleaseDC(hwnd, hdc);
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		DrawPanel(hdc); //绘制面板
		RefreshPanel(hdc); //刷新
		EndPaint(hwnd, &ps);
		return 0;
	case WM_DESTROY:
		if (block) free(block);
		if (timer_id) KillTimer(hwnd, ID_TIMER);
		PostQuitMessage(0);//该函数向系统表明有个线程有终止请求。通常用来响应WM_DESTROY消息。
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

//左移
void DoLeftShift(HDC hdc) {
	int x, y;
	if (NULL == block) return;

	if (0 == cur_left) return;
	if (cur_top < 0) return; //方块没有完整显示前，不能左移
	for (y = 0; y < height_block; y++) {
		for (x = 0; x < width_block; x++) { //从左边开始扫描， 获取该行最左边的实心方格块
			if (*(block + y*width_block + x)) {
				//判断当前方格在面板上面左边一个方格是否为实心，是就代表不能再左移
				if (g_panel[cur_top + y][cur_left + x - 1]) return;

				break; //只判断最左边的一个实心方格，之后直接扫描下一行
			}
		}
	}
	cur_left--;
	RefreshPanel(hdc);
}

//右移
void DoRightShift(HDC hdc) {
	int x, y;
	if (NULL == block) return;

	if (COLS - width_block == cur_left) return;
	if (cur_top < 0) return; //方块完整显示前不能右移
	for (y = 0; y < height_block; y++) {
		for (x = width_block - 1; x >= 0; x--) { //从右边开始臊面，获取该行最右边的实心方格块
			if (*(block + y*width_block + x)) {
				//判断当前方格在面板上右边一个方格是否为实心，是就代表不能再右移
				if (g_panel[cur_top + y][cur_left + x + 1]) return;
				break; //只判断最右边的一个实心方格
			}
		}
	}
	cur_left++;
	RefreshPanel(hdc);
}

void DoRedirection(HDC hdc) { //改变方向
	int i, j;
	byte * temp = NULL;
	if (NULL == block) return;
	if (cur_top < 0) return; //方块完整显示前不能转向

	temp = (byte *)malloc(sizeof(byte) *width_block *height_block);
	for (i = 0; i < width_block; i++) {
		for (j = 0; j < height_block; j++) {
			//temp[i][j] = block[height_block-j-1][i];
			*(temp + i*height_block + j) = *(block + (height_block - j - 1) *width_block + i);
		}
	}
	//给方块重新定位
	int incHeight = width_block - height_block;
	int incWidth = height_block - width_block;
	int temp_cur_top = cur_top - incHeight / 2;
	int temp_cur_left = cur_left - incWidth / 2;

	//system("cls");
	//printf("temp_top=%d, temp_left=%d, temp_cur_top, temp_cur_left");

	//判断当前空间是否足够让方块改变方向
	int max_len = max(width_block, height_block);
	//防止下标访问越界
	if (temp_cur_top + max_len - 1 >= ROWS || temp_cur_left < 0 || temp_cur_left + max_len - 1 >= COLS) {
		free(temp); //退出前必须先释放内存
		return;
	}
	for (i = 0; i < max_len; i++) {
		for (j = 0; j < max_len; j++) {
			//转向所需的空间内有已被占用的实心方格存在，即转向失败
			if (g_panel[temp_cur_top + i][temp_cur_left + j]) {
				free(temp); //退出前必须先释放内存
				return;
			}
		}
	}
	//把临时变量的值赋给block， 只能复制，而不能赋指针值
	for (i = 0; i < height_block; i++) {
		for (j = 0; j < width_block; j++) {
			//block[i][j] = temp[i][j];
			*(block + i*width_block + j) = *(temp + i*width_block + j);
		}
	}
	//全局变量重新被赋值
	cur_top = temp_cur_top;
	cur_left = temp_cur_left;
	//交换
	i = width_block;
	width_block = height_block;
	height_block = i;

	free(temp); //释放为临时变量分配的内存
	RefreshPanel(hdc);
}


bool IsTouchBottom(HDC hdc) {
	int x, y;
	int i, j;
	
	if (NULL == block) return false;
	if (ROWS == cur_top + height_block) {
		// 固定方块
		for (i = 0; i < height_block; i++) {
			for (j = 0; j < width_block; j++) {
				if (*(block + i*width_block + j)) g_panel[cur_top + i][cur_left + j] = 1;
			}
		}
		return true;
	}
	for (y = height_block - 1; y >= 0; y--) { //从底部行开始扫描
		//判断第一个实心方块在面板上邻接的下方方格是否为实心，是就代表已经到达底部
		for (x = 0; x < width_block; x++) {
			if (*(block + y*width_block + x)) {
				if (cur_top + y + 1 < 0) return false;
				if (g_panel[cur_top + y + 1][cur_left + x]) {
					//判断是否gameover
					if (cur_top <= 0) {
						if (timer_id) {
							KillTimer(hwnd, ID_TIMER);
							timer_id = 0;
						}
						MessageBox(hwnd, TEXT("游戏结束"), TEXT("游戏到此结束！"), MB_OK | MB_ICONEXCLAMATION);
						SendMessage(hwnd, WM_CLOSE, 0, 0);
					}
					//固定方块
					for (i = 0; i < height_block; i++) {
						for (j = 0; j < width_block; j++) {
							if (*(block + i*width_block + j)) g_panel[cur_top + i][cur_left + j] = 1;
						}
					}

					return true;
				}
			}
		}
	}

	return false;
}

//消行
void ClearRow(HDC hdc) {
	int i, j, k;
	int count = 0; //消行次数
	bool isFilled;
	//消行处理
	for (i = ROWS - 1; i >= 0; i--) {
		isFilled = true;
		for (j = 0; j < COLS; j++) {
			if (!g_panel[i][j]) {
				isFilled = false;
				break;
			}
		}
		if (isFilled) {
			for (j = 0; j < COLS; j++) {
				g_panel[i][j] = 0;
			}
			//所有方块往下移
			for (k = i - 1; k >= 0; k--) {
				for (j = 0; j < COLS; j++) {
					g_panel[k + 1][j] = g_panel[k][j];
				}
			}
			i = i + 1;
			count++;
		}
	}

	//最高级别为9即， 所以分数极限为(9+1)*SCORE_LEVEL_INC - 1
	if (score >= 10 * SCORE_LEVEL_INC - 1) return;

	//加分规则：消除行数， 1行加1分， 2行加3分， 3行加6分， 4行加10分
	switch (count) {
	case 1:
		score += 10;
		break;
	case 2:
		score += 15;
		break;
	case 3:
		score += 20;
		break;
	case 4:
		score += 30;
		break;
	}

	int temp_level = score / SCORE_LEVEL_INC;
	if (temp_level > level) {
		level = temp_level;
		//
	}
}
//刷新面板
void RefreshPanel(HDC hdc) {
	int x, y;
	RECT rect;
	HBRUSH h_bSolid = (HBRUSH)GetStockObject(GRAY_BRUSH),
		h_bEmpty = (HBRUSH)GetStockObject(WHITE_BRUSH);
	if (NULL == block) return;

	//先刷屏
	for (y = 0; y < ROWS; y++) {
		for (x = 0; x < COLS; x++) {
			//为避免刷掉方块的边框， rect范围必须比边框范围小1
			rect.top = y*CELL + 2;
			rect.bottom = (y + 1) *CELL - 2;
			rect.left = x*CELL + 2;
			rect.right = (x + 1) *CELL - 2;
			if (g_panel[y][x])
				FillRect(hdc, &rect, h_bSolid);
			else
				FillRect(hdc, &rect, h_bEmpty);
		}
	}
	//再定位方块
	for (y = 0; y < height_block; y++) {
		for (x = 0; x < width_block; x++) {
			if (*(block + width_block + x)) {//实心
				rect.top = (y + cur_top) *CELL + 2;
				rect.bottom = (y + cur_top + 1) *CELL - 2;
				rect.left = (x + cur_left) *CELL + 2;
				rect.right = (x + cur_left + 1) *CELL - 2;
				FillRect(hdc, &rect, h_bSolid);
			}
		}
	}
} 

//下移
void DoDownShift(HDC hdc) {
	if (NULL == block) return;

	//判断是否到达底部
	if (IsTouchBottom(hdc)) {//到底部
		//消行处理
		ClearRow(hdc);
		ExportBlock(); //输出下一个方块
	}  

	cur_top++;
	RefreshPanel(hdc);
} 
//加速
void DoAccelerate(HDC hdc) {
	if (NULL == block) return;

	if (IsTouchBottom(hdc)) {
		//消行处理
		ClearRow(hdc);
		ExportBlock();
	}
	cur_top++;
	RefreshPanel(hdc);
}
