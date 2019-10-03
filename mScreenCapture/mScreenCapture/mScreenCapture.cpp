#include "mScreenCapture.h"
#include "stdafx.h"
#include "ScrCapManger.h"

using namespace std;
#define MAX_LOADSTRING 100

// 全局变量: 
HINSTANCE hInst;								// 当前实例
TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名

// 此代码模块中包含的函数的前向声明: 
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK  KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);


ScreenCapture  *screenCpature = new ScreenCapture();

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPTSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    MSG msg;
    HACCEL hAccelTable;

    // 初始化全局字符串
    LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadString(hInstance, IDC_MSCREENCAPTURE, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化: 
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MSCREENCAPTURE));

    // 主消息循环: 
    while (GetMessage(&msg, NULL, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    delete screenCpature;
    return (int)msg.wParam;
}

//目的:注册窗口类。
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MSCREENCAPTURE));
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCE(IDC_MSCREENCAPTURE);
    wcex.lpszClassName = szWindowClass;
    //wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
    wcex.hIconSm = NULL;

    return RegisterClassEx(&wcex);
}

/*
函数:  InitInstance(HINSTANCE, int)
目的:  保存实例句柄并创建主窗口
在此函数中，我们在全局变量中保存实例句柄并
创建和显示主程序窗口。
*/
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    HWND hWnd;
    hInst = hInstance; // 将实例句柄存储在全局变量中

    // hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
    //   CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
    hWnd = CreateWindowEx(WS_EX_TOPMOST | WS_EX_TOOLWINDOW, szWindowClass, szTitle, WS_POPUP | WS_VISIBLE,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

    if (!hWnd)
    {
        return FALSE;
    }

    screenCpature->m_ghWnd = hWnd;
    ShowWindow(hWnd, SW_SHOWNORMAL | SW_MAXIMIZE);        //SW_SHOWNORMAL激活显示窗口 | SW_MAXIMIZE最大化指定窗口
    UpdateWindow(hWnd);

    SetTimer(hWnd, 3, 1000, NULL);
    return TRUE;
}

/*
函数:  WndProc(HWND, UINT, WPARAM, LPARAM)
目的:    处理主窗口的消息。
WM_COMMAND	- 处理应用程序菜单
WM_PAINT	- 绘制主窗口
WM_DESTROY	- 发送退出消息并返回
*/
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int wmId, wmEvent;
    PAINTSTRUCT ps;
    HDC hdc;

    //定义托盘	
    UINT WM_TASKBARCREATED; //防止explorer.exe奔溃，托盘图标消失
    POINT pt;
    int mid;

    //定义鼠标
    POINT mousePoint;

    switch (message)
    {
    case WM_COMMAND:
        wmId = LOWORD(wParam);
        wmEvent = HIWORD(wParam);
        // 分析菜单选择: 
        switch (wmId)
        {
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        break;
    case WM_CREATE:  //窗口创建时的消息
        if (NULL == screenCpature->m_Hook)
        {
            SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, hInst, 0);
        }

        WM_TASKBARCREATED = RegisterWindowMessage(TEXT("TaskbarCreated"));
        screenCpature->m_nid.cbSize = sizeof(NOTIFYICONDATA);
        screenCpature->m_nid.hWnd = hWnd;
        screenCpature->m_nid.uID = IDI_TARY;
        screenCpature->m_nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP | NIF_INFO;  //NIF_INFO托盘消息气泡
        screenCpature->m_nid.uCallbackMessage = WM_USER;
        screenCpature->m_nid.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_TARY));
        lstrcpy(screenCpature->m_nid.szTip, screenCpature->m_szAPPName);           //标准提示字符串
        Shell_NotifyIcon(NIM_ADD, &screenCpature->m_nid);

        screenCpature->GetScreenCapture();
        screenCpature->m_hMenu = CreatePopupMenu();
        AppendMenu(screenCpature->m_hMenu, MF_STRING, IDR_START, L"开始");
        AppendMenu(screenCpature->m_hMenu, MF_STRING, IDR_PAUSE, L"暂停");
        AppendMenu(screenCpature->m_hMenu, MF_STRING, IDR_EXIT, L"退出");
        break;
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        // TODO:  在此添加任意绘图代码...绘制主窗口，使其变成灰度背景
        screenCpature->PaintWndDialog(hdc);
        EndPaint(hWnd, &ps);
        break;
    case WM_USER:
        if (lParam == WM_LBUTTONDOWN)
        {
            //MessageBox(hWnd,TEXT("鼠标按下"),szAPPName,MB_OK);
            ShowWindow(screenCpature->m_ghWnd, SW_SHOW);
        }
        if (lParam == WM_LBUTTONDBLCLK)
        {
            SendMessage(hWnd, WM_CLOSE, wParam, lParam);
        }
        if (lParam == WM_RBUTTONDOWN)
        {
            GetCursorPos(&pt);
            ::SetForegroundWindow(hWnd);  //处理左键其它，菜单不消失
            //EnableMenuItem(hMenu, IDR_PAUSE, MF_GRAYED);
            mid = TrackPopupMenu(screenCpature->m_hMenu, TPM_RETURNCMD, pt.x, pt.y, NULL, hWnd, NULL); //显示菜单
            if (mid == IDR_START)
            {
                //MessageBox(hWnd, TEXT("start"), szAPPName, MB_OK);
                //ScreenCapture();
            }
            if (mid == IDR_PAUSE)
            {
                MessageBox(hWnd, TEXT("pause"), screenCpature->m_szAPPName, MB_OK);
            }
            if (mid == IDR_EXIT)
            {
                SendMessage(hWnd, WM_CLOSE, wParam, lParam);
            }
            if (mid == 0)
            {
                PostMessage(hWnd, WM_LBUTTONDOWN, NULL, NULL);
            }
        }
        break;
    case WM_TIMER:
        screenCpature->ShowTaryMsg();
        KillTimer(hWnd, wParam);
        break;
    case WM_LBUTTONDOWN:  //鼠标按下
        if (!screenCpature->m_bIsSelect)
        {
            screenCpature->m_bMouseDown = true;
            GetCursorPos(&mousePoint);
            screenCpature->m_rect.left = screenCpature->m_rect.right = mousePoint.x;
            screenCpature->m_rect.top = screenCpature->m_rect.bottom = mousePoint.y;
            InvalidateRgn(hWnd, NULL, false); //InvalidateRgn()函数把各个分区颜色清除，然后填充
        }
        else
        {
            screenCpature->m_bIsSelect = false;
            InvalidateRgn(hWnd, NULL, false);
        }
        break;
    case WM_LBUTTONUP: //鼠标释放
        if ((!screenCpature->m_bIsSelect) && screenCpature->m_bMouseDown)
        {
            screenCpature->m_bMouseDown = false;
            screenCpature->m_bIsSelect = true;
            GetCursorPos(&mousePoint);
            screenCpature->m_rect.right = mousePoint.x;
            screenCpature->m_rect.bottom = mousePoint.y;
            InvalidateRgn(hWnd, NULL, false);
        }
        break;
    case WM_MOUSEMOVE: //鼠标移动
        if ((!screenCpature->m_bIsSelect) && screenCpature->m_bMouseDown)
        {
            GetCursorPos(&mousePoint);
            screenCpature->m_rect.right = mousePoint.x;
            screenCpature->m_rect.bottom = mousePoint.y;
            InvalidateRgn(hWnd, NULL, false);
        }
        break;
    case WM_DESTROY:
        UnhookWindowsHookEx(screenCpature->m_Hook);   //卸载钩子
        Shell_NotifyIcon(NIM_DELETE, &screenCpature->m_nid);
        if (screenCpature->m_bIsShow)
        {
            screenCpature->ReleaseBmp();
        }
        PostQuitMessage(0);
        break;
        if (message == WM_TASKBARCREATED)
        {
            //Shell_NotifyIcon(NIM_ADD, &nid);
            SendMessage(hWnd, WM_CREATE, wParam, lParam);
        }
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

LRESULT CALLBACK  KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode >= 0)  //注意等于0
    {
        PKBDLLHOOKSTRUCT pKeyStruct = (PKBDLLHOOKSTRUCT)lParam;  //可以获取底层键盘操作
        if (wParam == WM_KEYUP)
        {
            //if (GetKeyState(VK_ESCAPE) < 0)
            if (pKeyStruct->vkCode == VK_ESCAPE)
            {
                if (screenCpature->m_bIsShow)
                {
                    if (screenCpature->m_bIsSelect)
                    {
                        screenCpature->m_bIsSelect = false;
                        InvalidateRgn(screenCpature->m_ghWnd, NULL, false); //使窗口指定区域不活动，并将它加入窗口刷新区，使之可随后被重画
                    }
                    else
                    {
                        //SendMessage(g_hWnd, WM_CLOSE, NULL, NULL);
                        //DestroyWindow(g_hWnd);
                        ShowWindow(screenCpature->m_ghWnd, SW_HIDE); //隐藏界面
                    }
                }
            }
            else if (pKeyStruct->vkCode == VK_RETURN)
            {
                if (screenCpature->m_bIsSelect)
                {
                    screenCpature->m_sfileName = screenCpature->GetLocalTime();
                    string strFilePath = screenCpature->CreateSaveFoldr(screenCpature->m_sfileName);
                    Sleep(1000);
                    screenCpature->SaveScreenCapture(strFilePath);
                }
            }
            //ctrl+x
            else if (((GetKeyState(VK_CONTROL) & 0x80) == 0x80) && (pKeyStruct->vkCode == 0x58))
            {
                //ScreenCapture();
                //ShowWindow(g_hWnd, SW_HIDE); //隐藏界面
            }
        }
    }
    return CallNextHookEx(screenCpature->m_Hook, nCode, wParam, lParam); ////让代码继续后续处理
}


