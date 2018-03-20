#include "stdafx.h"
#include "Mine Sweeper.h"

HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

int ma[50][50];  //生成的地图
int mb[50][50];  //当前显示的地图
int WIDTH;       //界面宽度
int HEIGHT;      //界面高度
int ROWS;     //行数
int COLUMNS; //列数
int REST; //雷的个数
bool M;//模式:1为计时模式.0为不计时模式，只计分.在自定义模式里面设置，默认为1
int status; //游戏状态
int SCO;//分数
int cur_time;
const int num_size = 20;

int light[10][7] = {
	{ 1,1,1,0,1,1,1 },
	{ 0,0,1,0,0,1,0 },
	{ 1,0,1,1,1,0,1 },
	{ 1,0,1,1,0,1,1 },
	{ 0,1,1,1,0,1,0 },
	{ 1,1,0,1,0,1,1 },
	{ 1,1,0,1,1,1,1 },
	{ 1,0,1,0,0,1,0 },
	{ 1,1,1,1,1,1,1 },
	{ 1,1,1,1,0,1,1 },
};
int sp[10][2] = {
	{ 0,0 },
	{ 0,0 },
	{ num_size,0 },
	{ 0,num_size },
	{ 0,num_size },
	{ num_size,num_size },
	{ 0,2 * num_size }
};
int ep[10][2] = {
	{ num_size,0 },
	{ 0,num_size },
	{ num_size,num_size },
	{ num_size,num_size },
	{ 0,2 * num_size },
	{ num_size,2 * num_size },
	{ num_size,2 * num_size }
};
//把数组坐标转换为界面坐标
void xy_to_pixel(int x, int y, int*pixelx, int *pixely) {
	*pixelx = x*PIECE_WH + ZERO_X;
	*pixely = y*PIECE_WH + ZERO_Y;
}
//以数组坐标画线
void draw_line(HDC hdc, int sx, int sy, int dx, int dy) {
	int psx, psy, pdx, pdy;
	xy_to_pixel(sx, sy, &psx, &psy);
	xy_to_pixel(dx, dy, &pdx, &pdy);
	MoveToEx(hdc, psx, psy, NULL);
	LineTo(hdc, pdx, pdy);
}
//画数字
void draw_num(HDC hdc, int x, int y, int num)
{
	int i;
	static HPEN hWhitePen = CreatePen(PS_SOLID, 3, RGB(255, 255, 255));
	SelectObject(hdc, hWhitePen);
	for (i = 0; i < 7; i++)
	{
		MoveToEx(hdc, x + sp[i][0], y + sp[i][1], NULL);
		LineTo(hdc, x + ep[i][0], y + ep[i][1]);
	}
	static HPEN hRedPen = CreatePen(PS_SOLID, 3, RGB(255, 0, 0));
	SelectObject(hdc, hRedPen);

	for (i = 0; i < 7; i++)
	{
		if (light[num][i])
		{
			MoveToEx(hdc, x + sp[i][0], y + sp[i][1], NULL);
			LineTo(hdc, x + ep[i][0], y + ep[i][1]);
		}
	}
}
//深度优先搜索，用于翻开一连串的空白区域，由(sp1,sp2)向八个方向扩展，并进行递归
void dfs(int sp1, int sp2)
{
	mb[sp1][sp2] = 1;
	if (sp1+1 < COLUMNS && mb[sp1 + 1][sp2] == 0)//向下扩张
	{
		if (ma[sp1 + 1][sp2] == 0)
		{
			dfs(sp1 + 1, sp2);
		}
		else if (ma[sp1 + 1][sp2] > 0)//如果是数字，停止扩张
		{
			mb[sp1 + 1][sp2] = 1;
		}
	}
	if (sp1 > 0 && mb[sp1 - 1][sp2] == 0)//向上扩张
	{
		if (ma[sp1 - 1][sp2] == 0)
		{
			dfs(sp1 - 1, sp2);
		}
		else if (ma[sp1 - 1][sp2] > 0)//如果是数字，停止扩张
		{
			mb[sp1 - 1][sp2] = 1;
		}
	}
	if (sp2+1 <ROWS && mb[sp1][sp2 + 1] == 0)//向右扩张
	{
		if (ma[sp1][sp2 + 1] == 0)
		{
			dfs(sp1, sp2 + 1);
		}
		else if (ma[sp1][sp2 + 1] > 0)//如果是数字，停止扩张
		{
			mb[sp1][sp2 + 1] = 1;
		}
	}
	if (sp2 > 0 && mb[sp1][sp2 - 1] == 0)//向左扩张
	{
		if (ma[sp1][sp2 - 1] == 0)
		{
			dfs(sp1, sp2 - 1);
		}
		else if (ma[sp1][sp2 - 1] > 0)//如果是数字，停止扩张
		{
			mb[sp1][sp2 - 1] = 1;
		}
	}
	if (sp1+1 <COLUMNS && sp2 +1<ROWS && mb[sp1 + 1][sp2 + 1] == 0)//向右下扩张
	{
		if (ma[sp1 + 1][sp2 + 1] == 0)
		{
			dfs(sp1 + 1, sp2 + 1);
		}
		else if (ma[sp1 + 1][sp2 + 1] > 0)//如果是数字，停止扩张
		{
			mb[sp1 + 1][sp2 + 1] = 1;
		}
	}
	if (sp1 > 0 && sp2 > 0 && mb[sp1 - 1][sp2 - 1] == 0)//向左上扩张
	{
		if (ma[sp1 - 1][sp2 - 1] == 0)
		{
			dfs(sp1 - 1, sp2 - 1);
		}
		else if (ma[sp1 - 1][sp2 - 1] > 0)//如果是数字，停止扩张
		{
			mb[sp1 - 1][sp2 - 1] = 1;
		}
	}
	if (sp1 > 0 && sp2 +1 < ROWS && mb[sp1 - 1][sp2 + 1] == 0)//向右上扩张
	{
		if (ma[sp1 - 1][sp2 + 1] == 0)
		{
			dfs(sp1 - 1, sp2 + 1);
		}
		else if (ma[sp1 - 1][sp2 + 1] > 0)//如果是数字，停止扩张
		{
			mb[sp1 - 1][sp2 + 1] = 1;
		}
	}
	if (sp1+1 < COLUMNS && sp2 > 0 && mb[sp1 + 1][sp2 - 1] == 0)//向左下扩张
	{
		if (ma[sp1 + 1][sp2 - 1] == 0)
		{
			dfs(sp1 + 1, sp2 - 1);
		}
		else if (ma[sp1 + 1][sp2 - 1] > 0)//如果是数字，停止扩张
		{
			mb[sp1 + 1][sp2 - 1] = 1;
		}
	}
}
//翻开(sp1,sp2)这个区域
void step(int sp1, int sp2)
{
	if (ma[sp1][sp2] != 0)//不是空白区域，直接翻开此区域
	{
		mb[sp1][sp2] = 1;
	}
	else
	{
		dfs(sp1, sp2);//踩到空白，开始向周围扩张
	}
}
//在整个雷盘布置num个雷，而且保证(px,py)点不能被放上雷
void setmine(int num, int px, int py)
{
	int i;
	int tx, ty;
	srand((unsigned int)time(NULL));
	for (i = 0; i < num; i++)
	{
		tx = rand() %COLUMNS;//随机数挑选纵坐标
		ty = rand() % ROWS;//随机数挑选横坐标
		if (ma[tx][ty] == 10 || (tx == px && ty == py))//如果这一点已经被布置过雷，或者是(px,py)点，那么就跳过，重新挑点
		{
			i--;
			continue;
		}
		ma[tx][ty] = 10;
		//将四周不是雷的点的数字加一
		if (ty>0&&ma[tx][ty - 1] < 9)
		{
			ma[tx][ty - 1]++;
		}
		if (ty + 1<ROWS && ma[tx][ty + 1] < 9)
		{
			ma[tx][ty + 1]++;
		}
		if (tx + 1<COLUMNS && ma[tx + 1][ty] < 9)
		{
			ma[tx + 1][ty]++;
		}
		if (tx>0&&ma[tx - 1][ty] < 9)
		{
			ma[tx - 1][ty]++;
		}
		if (tx +1 <COLUMNS && ty+ 1 <ROWS && ma[tx + 1][ty + 1] < 9)
		{
			ma[tx + 1][ty + 1]++;
		}
		if (tx >0&& ty>0&&ma[tx - 1][ty - 1] < 9)
		{
			ma[tx - 1][ty - 1]++;
		}
		if (tx+1<COLUMNS && ty>0 && ma[tx + 1][ty - 1] < 9)
		{
			ma[tx + 1][ty - 1]++;
		}
		if (tx>0 && ty+1<ROWS && ma[tx - 1][ty + 1] < 9)
		{
			ma[tx - 1][ty + 1]++;
		}
	}
}
//检查玩家目前是否赢得了这局游戏
bool check()
{
	int i, j;
	int flag = 0;
	SCO = 0;
	if (status == 2) {
		for (i = 0; i <COLUMNS; i++)
			for (j = 0; j <ROWS; j++)
			{
				if (ma[i][j] < 10 && mb[i][j] == 1)					SCO++;
				else if (mb[i][j] == -1 && ma[i][j] == 10) 		flag++;
			}
		if (SCO + flag == ROWS * COLUMNS)
		{
			SCO = ROWS * COLUMNS;
			return 1;//检查时没有会出现问题，返回1，表示赢得游戏
		}
	}
	else if(status==3) {
		for (i = 0; i <COLUMNS; i++)
			for (j = 0; j <ROWS; j++)
			{
				if (ma[i][j] < 10 && mb[i][j] == 1)					SCO++;
				else if (mb[i][j] == -1 && ma[i][j] == 10) 		SCO++;
				else if (mb[i][j] == -1 && ma[i][j] != 10) SCO--;
			}
	}
	return 0;
}
/* 游戏说明


界面高度计算公式:HEIGHT=ZERO_Y+ROWS*PIECE_WH+80;
界面宽度计算公式:WIDTH=ZERO_X+COLUMNS*PIECE_WH+40;
状态说明：status0代表还未开始状态，1代表已经开始还未点击第一步的状态，2代表已经点击第一步的状态，3代表游戏结束的状态
游戏说明：游戏开始前，需要点击开始按钮；左键表示翻开或者撤销旗帜；右键代表插旗；
键盘操作：上下左右，移动需要操作的位置;按HOME键相当于鼠标左击，按END键相当于鼠标右击;
ma中0代表周围没有炸弹，无数字；1-8代表周围的炸弹数，10代表为炸弹。
mb中0代表没有被操作，-1代表被红旗标记，1代表已经被翻开。

*/
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    Help(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    MySet(HWND, UINT, WPARAM, LPARAM);
VOID CALLBACK CountDown(HWND, UINT, UINT, DWORD);
VOID CALLBACK TimerScore(HWND hwnd, UINT message, UINT idTimer, DWORD dwTime);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_MINESWEEPER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化: 
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MINESWEEPER));

    MSG msg;

    // 主消息循环: 
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MINESWEEPER));
    wcex.hCursor        = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_CURSOR1));
    wcex.hbrBackground  = (HBRUSH)COLOR_BACKGROUND;
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_MINESWEEPER);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 将实例句柄存储在全局变量中

   ROWS = 13;     //行数
   COLUMNS = 15; //列数
   HEIGHT = ZERO_Y + ROWS*PIECE_WH + 80;
   WIDTH = ZERO_X + COLUMNS*PIECE_WH + 40;

   HWND hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX,
      CW_USEDEFAULT, 0, WIDTH, HEIGHT, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd){      return FALSE; }
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);
   return TRUE;
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  
	auto hmm = GetMenu(hWnd);
	auto hfmn = GetSubMenu(hmm, 0);

	static POINT NowPoint;
	static HBITMAP hbmpFLE, hbmpFL, hbmpBO, hbmpBOE;
	static HWND    hwndButton;	
	static RECT upRect;
	static int  IsFinish;
	static HFONT hFontOld, hFont = CreateFont(26, 26, 0, 0, FW_REGULAR, FALSE, FALSE, FALSE, GB2312_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FIXED_PITCH | FF_MODERN, TEXT("宋体"));
	static COLORREF ColorList[11] = { RGB(0, 255, 0), RGB(255,0,	0) , RGB(0,0,255) , RGB(255,255,0) ,
		RGB(218,112,214) , RGB(255,0,255) ,RGB(106,90,205) , RGB(255,215,0),RGB(0, 0, 0),RGB(255, 255, 255),RGB(203,203,203) };
	static HPEN hpenOld,hpenRed = CreatePen(PS_SOLID, 1, ColorList[1]);

    switch (message)
    {
	case WM_CREATE:
	{
		hbmpFLE = LoadBitmap(hInst, MAKEINTRESOURCE(FLE));
		hbmpFL = LoadBitmap(hInst, MAKEINTRESOURCE(FL));
		hbmpBO = LoadBitmap(hInst, MAKEINTRESOURCE(BO));
		hbmpBOE= LoadBitmap(hInst, MAKEINTRESOURCE(BOE));

		hwndButton = CreateWindow(TEXT("BUTTON"), TEXT("开始"), WS_VISIBLE | WS_CHILD,
			WIDTH / 2-25, 30, 50, 25, hWnd, (HMENU)ID_START, hInst, NULL);
		IsFinish = 0;
		REST = 30;
		M = 1;
		cur_time = TIME;
		upRect.left = 0;
		upRect.top = ZERO_Y;
		upRect.right = WIDTH;
		upRect.bottom = HEIGHT;
		CheckMenuRadioItem(hfmn, ID_L, ID_MYSET, ID_L, MF_BYCOMMAND);
	}
	case WM_KEYDOWN: //添加了键盘控制扫雷这种奇怪的功能
		switch (wParam)
		{
		case VK_HOME: 
		{
			int ti = NowPoint.x;
			int tj = NowPoint.y;
			if (status == 1) {
						setmine(REST, ti, tj);
						if (mb[ti][tj] == -1) {
							mb[ti][tj] = 0;
						}
						else if (mb[ti][tj] == 0) {
							mb[ti][tj] = 1;
							step(ti, tj);
						}
						status = 2;
					}
			else if (status > 1 && status < 3) {
						if (mb[ti][tj] == -1) {
							mb[ti][tj] = 0;
						}
						else if (mb[ti][tj] == 0) {
							mb[ti][tj] = 1;
							step(ti, tj);
							if (ma[ti][tj] == 10)
							{
								status = 3;
								if (M) KillTimer(hWnd, CDTimer);
								KillTimer(hWnd, TSTimer);
								MessageBoxA(hWnd, "你已死亡！", "游戏结束", 0);
							}
						}
					}
			InvalidateRect(hWnd, &upRect, TRUE);
		}
		break;
		case VK_END:
		{
			int ti = NowPoint.x;
			int tj = NowPoint.y;
			if (status>1 && status<3 && mb[ti][tj] == 0) {
						mb[ti][tj] = -1;
			}
			InvalidateRect(hWnd, &upRect, TRUE);
		}
		break;
		case VK_UP:
			if (NowPoint.y > 0) {
				NowPoint.y--;
				InvalidateRect(hWnd, &upRect, TRUE);
			}
			break;
		case VK_DOWN:
			if (NowPoint.y < ROWS) {
				NowPoint.y++;
				InvalidateRect(hWnd, &upRect, TRUE);
			}
			break;
		case VK_LEFT:
			if (NowPoint.x > 0) {
				NowPoint.x--;
				InvalidateRect(hWnd, &upRect, TRUE);
			}
			break;
		case VK_RIGHT:
			if (NowPoint.x < COLUMNS) {
				NowPoint.x++;
				InvalidateRect(hWnd, &upRect, TRUE);
			}
			break;
		default:
			break;
		}
		break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
			case ID_MANNUAL:
				DialogBox(hInst, MAKEINTRESOURCE(IDD_HELP), hWnd, Help);
				break;
			case ID_START:
				if (status==0||status==3) {
					memset(ma, 0, sizeof(ma));
					memset(mb, 0, sizeof(mb));
					NowPoint.x = 0;
					NowPoint.y = 0;
					SCO = 0;
					status = 1;
					IsFinish = 0;
					cur_time = TIME;
					SetWindowText(hwndButton, TEXT("停止"));
					if (M) {
						SetTimer(hWnd, CDTimer, 1000, (TIMERPROC)CountDown);
					}
					SetTimer(hWnd, TSTimer, 200, (TIMERPROC)TimerScore);
				}
				else{
					status = 3;
					SetWindowText(hwndButton, TEXT("开始"));
					if (M) {
						KillTimer(hWnd, CDTimer);
					}
					KillTimer(hWnd, TSTimer);
				}
				InvalidateRect(hWnd, &upRect, TRUE);
				break;
			case ID_L:
			case ID_M:
			case ID_H:
				if (status != 2&&status!=1) {
					if (wmId == ID_L) {
						CheckMenuRadioItem(hfmn, ID_L, ID_MYSET, ID_L, MF_BYCOMMAND);
						ROWS = 13;     //行数
						COLUMNS = 15; //列数
						REST = 30;
					}
					if (wmId == ID_M) {
						CheckMenuRadioItem(hfmn, ID_L, ID_MYSET, ID_M, MF_BYCOMMAND);
						ROWS = 16;     //行数
						COLUMNS = 20; //列数
						REST = 60;
					}
					if (wmId == ID_H) {
						CheckMenuRadioItem(hfmn, ID_L, ID_MYSET, ID_H, MF_BYCOMMAND);
						ROWS = 19;     //行数
						COLUMNS = 25; //列数
						REST = 90;
					}
					status = 0;
					M = 1;
					HEIGHT = ZERO_Y + ROWS*PIECE_WH + 70;
					WIDTH = ZERO_X + COLUMNS*PIECE_WH + 40;
					upRect.left = 0;
					upRect.top = ZERO_Y;
					upRect.right = WIDTH;
					upRect.bottom = HEIGHT;
					MoveWindow(hwndButton, WIDTH / 2 - 33, 30, 50, 25, TRUE);
					POINT point;point.x = 0;point.y = 0;
					ClientToScreen(hWnd, &point);
					MoveWindow(hWnd, point.x-8, point.y-51, WIDTH, HEIGHT, TRUE);
				}
				break;
			case ID_MYSET:
				if (status != 2 && status != 1) {
					status = 0;
					INT_PTR returnData = DialogBox(hInst, MAKEINTRESOURCE(IDD_MYSETBOX), hWnd, MySet);
					if (returnData != NULL)
					{
						if (returnData == IDOK) {
							CheckMenuRadioItem(hfmn, ID_L, ID_MYSET, ID_MYSET, MF_BYCOMMAND);
							HEIGHT = ZERO_Y + ROWS*PIECE_WH + 80;
							WIDTH = ZERO_X + COLUMNS*PIECE_WH + 40;
							upRect.left = 0;
							upRect.top = ZERO_Y;
							upRect.right = WIDTH;
							upRect.bottom = HEIGHT;
							MoveWindow(hwndButton, WIDTH / 2 - 33, 20, 50, 25, TRUE);
							POINT point;point.x = 0;point.y = 0;
							ClientToScreen(hWnd, &point);
							MoveWindow(hWnd, point.x, point.y, WIDTH, HEIGHT, TRUE);
						}
					}
				}
				break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
		{	
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			for (int i = 0; i <= ROWS; i++) {
					draw_line(hdc, 0, i, COLUMNS, i);
			}
			for (int i = 0; i <= COLUMNS; i++) {
					draw_line(hdc, i, 0, i, ROWS);
			}

			SelectObject(hdc, GetStockObject(NULL_BRUSH));
			SelectObject(hdc, GetStockObject(BLACK_PEN));
			SetBkColor(hdc, ColorList[10]);
			SetTextColor(hdc, ColorList[8]);
			TextOut(hdc, WIDTH / 3 * 2, 60, TEXT("得分"), 2);

			RoundRect(hdc, WIDTH / 3 * 2 - 25, 12, WIDTH / 3 * 2 + 63, 60, 2, 2);
			if (M) {
				RoundRect(hdc, WIDTH / 3 - 63, 12, WIDTH / 3 + 25, 60, 2, 2);
				TextOut(hdc, WIDTH / 3 - 40, 60, TEXT("倒计时"), 3);
			}
			HDC memhdc = CreateCompatibleDC(hdc);
			hFontOld = (HFONT)SelectObject(hdc, hFont);       //选择字体

			int px, py;
			if (IsFinish == 0 && cur_time == 0&&(status==1||status==2)) {
				IsFinish = 1;
			}
			if (IsFinish ==0&&check() == 1 && (status == 1||status == 2)) {
				IsFinish = 1;
			}
			for (int j= 0;j<ROWS;j++) {
				for (int i = 0;i < COLUMNS;i++) {
					xy_to_pixel(i,j, &px, &py);
					if (status == 0||status==1) {
							SetBkColor(hdc, ColorList[9]);
							TextOut(hdc, px + 2, py + 2, TEXT(" "), 1);
					}
					else if (status==2) {
						if (mb[i][j] == -1)
						{
							SetBkColor(hdc, ColorList[9]);
							TextOut(hdc, px + 2, py + 2, TEXT(" "), 1);
							SelectObject(memhdc, hbmpFL);
							TransparentBlt(hdc, px+2, py+2,28,28, memhdc, 0, 0, 28,28, ColorList[9]);
						}
						else if (mb[i][j] == 0)
						{
							
							SetBkColor(hdc, ColorList[9]);
							TextOut(hdc, px + 2, py + 2, TEXT(" "), 1);
						}
						else if (mb[i][j] == 1)
						{
							SetBkColor(hdc, ColorList[10]);
							char buff[2];
							if (ma[i][j]) buff[0] = ma[i][j] + '0';
							else buff[0] = ' ';
							buff[1] = '\0';
							if (ma[i][j]) {
								SetTextColor(hdc,ColorList[ma[i][j]-1]);
							}
							TextOutA(hdc, px + 2, py + 2, buff, 1);
						}
					}
					else {
						SetBkColor(hdc, ColorList[10]);
						if (mb[i][j] != -1) {
							if (ma[i][j] == 10) {
								SelectObject(memhdc, hbmpBO);
								TransparentBlt(hdc, px + 2, py + 2, 28, 28, memhdc, 0, 0, 28, 28, ColorList[9]);
							}
							else {
								char buff[2];
								if (9 > ma[i][j] && ma[i][j] >0) {
									if (ma[i][j]) {
										SetTextColor(hdc, ColorList[ma[i][j] - 1]);
									}
									buff[0] = ma[i][j] + '0';
								}
								else {
									buff[0] = ' ';
								}
								buff[1] = '\0';
								TextOutA(hdc, px + 2, py + 2, buff, 1);
							}
							char buff[2];
							if (9>ma[i][j]&&ma[i][j]>0) buff[0] = ma[i][j] + '0';
							else if (ma[i][j]==0)  buff[0] = ' ';
							buff[1] = '\0';
							if (9 > ma[i][j] && ma[i][j] >= 0) {
								if (ma[i][j]) {
									SetTextColor(hdc, ColorList[ma[i][j] - 1]);
								}
								TextOutA(hdc, px + 2, py + 2, buff, 1);
							}
							else {
								SelectObject(memhdc, hbmpBO);
								TransparentBlt(hdc, px + 2, py + 2, 28, 28, memhdc, 0, 0, 28, 28, ColorList[9]);
							}
						}
						else {
							if (ma[i][j]>=0&&ma[i][j]<9) {
								SelectObject(memhdc, hbmpFLE);
								TransparentBlt(hdc, px+2, py+2, 28, 28, memhdc, 0, 0, 28, 28,ColorList[9]);
							}
							else {
								SelectObject(memhdc, hbmpBOE);
								TransparentBlt(hdc, px + 2, py + 2, 28, 28, memhdc, 0, 0, 28, 28, ColorList[9]);
							}
						}
					}
				}
			}

			if (status !=0) {
				hpenOld = (HPEN)SelectObject(hdc, hpenRed);
				draw_line(hdc, NowPoint.x, NowPoint.y, NowPoint.x, NowPoint.y + 1);
				draw_line(hdc, NowPoint.x, NowPoint.y, NowPoint.x + 1, NowPoint.y);
				draw_line(hdc, NowPoint.x + 1, NowPoint.y, NowPoint.x + 1, NowPoint.y + 1);
				draw_line(hdc, NowPoint.x, NowPoint.y + 1, NowPoint.x + 1, NowPoint.y + 1);
				SelectObject(hdc, hpenOld);
			}
			if (IsFinish&&(status==1||status==2)) {
				status = 3;
				IsFinish = 0;
				draw_num(hdc, WIDTH / 3, 15, cur_time % 10);
				draw_num(hdc, WIDTH / 3 - 30, 15, cur_time / 10 % 10);
				draw_num(hdc, WIDTH / 3 - 60, 15, cur_time / 100 % 10);
				draw_num(hdc, WIDTH / 3 * 2 + 37, 15, SCO % 10);
				draw_num(hdc, WIDTH / 3 * 2 + 7, 15, (SCO / 10) % 10);
				draw_num(hdc, WIDTH / 3 * 2 - 23, 15, (SCO / 100) % 10);
				if (M) KillTimer(hWnd, CDTimer);
				KillTimer(hWnd, TSTimer);
				if (cur_time == 0) {
					MessageBox(hWnd, TEXT("游戏结束"), TEXT("时间截止"), 0);
				}else{
					MessageBox(hWnd, TEXT("你已胜利！"), TEXT("游戏结束"), 0);
				}
				SetWindowText(hwndButton, TEXT("开始"));
				InvalidateRect(hWnd, &upRect, TRUE);
			}
			SelectObject(hdc, hFontOld);       //选择字体
			DeleteObject(memhdc);
			EndPaint(hWnd, &ps);
		 }
        break;
	case WM_LBUTTONDOWN:
		{
		if (status != 3) {
			int tx = LOWORD(lParam);
			int ty = HIWORD(lParam);
			if (tx > ZERO_X && ty > ZERO_Y) {
				int ti = (tx - ZERO_X) / 30;
				int tj = (ty - ZERO_Y) / 30;
				if (ti >= 0 && ti < COLUMNS && tj >= 0 && tj < ROWS) {
					//CHAR buff[100];
					//sprintf(buff, "点击第%d行,第%d列", tj+1, ti+1);
					//MessageBoxA(hWnd, buff, "左键点击", 0);
					if (status == 1) {
						setmine(REST, ti, tj);
						if (mb[ti][tj] == -1) {
							mb[ti][tj] = 0;
						}
						else if (mb[ti][tj] == 0) {
							mb[ti][tj] = 1;
							step(ti, tj);
						}
						status = 2;
					}
					else if (status > 1 && status < 3) {
						if (mb[ti][tj] == -1) {
							mb[ti][tj] = 0;
						}
						else if (mb[ti][tj] == 0) {
							mb[ti][tj] = 1;
							step(ti, tj);
							if (ma[ti][tj] == 10)
							{
								status = 3;
								if (M) KillTimer(hWnd, CDTimer);
								KillTimer(hWnd, TSTimer);
								MessageBox(hWnd, TEXT("你已死亡！"), TEXT("游戏结束"), 0);
								SetWindowText(hwndButton, TEXT("开始"));
							}
						}
					}
					NowPoint.x = ti;
					NowPoint.y = tj;
					InvalidateRect(hWnd, &upRect, TRUE);
				}
			}
		}
		}
		break;
	case WM_RBUTTONDOWN:
		{
		if (status != 3) {
			int tx = LOWORD(lParam);
			int ty = HIWORD(lParam);
			if (tx > ZERO_X&&ty > ZERO_Y) {
				int ti = (tx - ZERO_X) / 30;
				int tj = (ty - ZERO_Y) / 30;
				//CHAR buff[100];
				//sprintf(buff, "点击第%d行,第%d列", tj + 1, ti + 1);
				//MessageBoxA(hWnd, buff, "右键点击", 0);
				if (ti >= 0 && ti < COLUMNS && tj >= 0 && tj < ROWS) {
					if (status > 1 && status < 3 && mb[ti][tj] == 0) {
						mb[ti][tj] = -1;
					}
					NowPoint.x = ti;
					NowPoint.y = tj;
					InvalidateRect(hWnd, &upRect, TRUE);
				}
			}
		}
		}
		break;
    case WM_DESTROY:
		DeleteObject(hFont);
		DeleteObject(hbmpBO);
		DeleteObject(hbmpBOE);
		DeleteObject(hbmpFL);
		DeleteObject(hbmpFLE);
		DeleteObject(hpenRed);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
// “教程”框的消息处理程序。
INT_PTR CALLBACK Help(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
// “自定义”框的消息处理程序。
INT_PTR CALLBACK MySet(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)
		{
			//获得Edit编辑框的句柄  
			HWND hEdit = GetDlgItem(hDlg, IDC_EDIT1);
			//获得文本  
			WCHAR wText[100];
			CHAR Text[100];
			GetWindowText(hEdit, wText, 100);
			WideCharToMultiByte(CP_ACP, 0, wText, -1, Text, 100, NULL, NULL);
			int a = atoi(Text);
			hEdit = GetDlgItem(hDlg, IDC_EDIT2);
			GetWindowText(hEdit, wText, 100);
			WideCharToMultiByte(CP_ACP, 0, wText, -1, Text, 100, NULL, NULL);
			int b = atoi(Text);
			hEdit = GetDlgItem(hDlg, IDC_EDIT3);
			GetWindowText(hEdit, wText, 100);
			WideCharToMultiByte(CP_ACP, 0, wText, -1, Text, 100, NULL, NULL);
			int c = atoi(Text);
			if (IsDlgButtonChecked(hDlg, 1004)) {
				M = 1;
			}
			else if (IsDlgButtonChecked(hDlg, 1005)) {
				M = 0;
			}
			bool isok = true;
			if (a<10 || a>50) {
				MessageBox(hDlg, TEXT("行数未在10-50之间"), TEXT("设置错误"), 0);
				isok = false;
			}
			if (b<10 || b>50) {
				MessageBox(hDlg,  TEXT("列数未在10-50之间"), TEXT("设置错误"), 0);
				isok = false;
			}
			if (c<0 || c>a*b*2/3) {
				MessageBox(hDlg,TEXT("地雷数不合理,不应该大于总格子数的2/3"), TEXT("设置错误"), 0);
				isok = false;
			}
			if (isok) {
				ROWS = a;
				COLUMNS = b;
				REST = c;
				EndDialog(hDlg, LOWORD(wParam));
				return (INT_PTR)TRUE;
			}
			return (INT_PTR)FALSE;
		}
		else if (LOWORD(wParam) == IDCANCEL) {
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
VOID CALLBACK CountDown(HWND hwnd, UINT message, UINT idTimer, DWORD dwTime)
{
		HDC hdc;
		hdc = GetDC(hwnd);
		if (cur_time ==1)
		{
			cur_time--;
			InvalidateRect(hwnd, NULL, TRUE);
			return;
		}

		else if(cur_time>0 ){
			cur_time--;
		}
		draw_num(hdc, WIDTH / 3, 15, cur_time % 10);
		draw_num(hdc, WIDTH / 3 - 30, 15, cur_time / 10 % 10);
		draw_num(hdc, WIDTH / 3 - 60, 15, cur_time / 100 % 10);
		DeleteDC(hdc);
}
VOID CALLBACK TimerScore(HWND hwnd, UINT message, UINT idTimer, DWORD dwTime)
{
	HDC hdc;
	hdc = GetDC(hwnd);
	draw_num(hdc, WIDTH / 3 * 2 + 37, 15, SCO % 10);
	draw_num(hdc, WIDTH / 3 * 2 + 7, 15, (SCO / 10) % 10);
	draw_num(hdc, WIDTH / 3 * 2 - 23, 15, (SCO / 100) % 10);
	DeleteDC(hdc);
}