#include "mScreenCapture.h"
#include "stdafx.h"
#include "ScrCapManger.h"

using namespace std;
#define MAX_LOADSTRING 100

// ȫ�ֱ���: 
HINSTANCE hInst;								// ��ǰʵ��
TCHAR szTitle[MAX_LOADSTRING];					// �������ı�
TCHAR szWindowClass[MAX_LOADSTRING];			// ����������

// �˴���ģ���а����ĺ�����ǰ������: 
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

    // ��ʼ��ȫ���ַ���
    LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadString(hInstance, IDC_MSCREENCAPTURE, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // ִ��Ӧ�ó����ʼ��: 
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MSCREENCAPTURE));

    // ����Ϣѭ��: 
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

//Ŀ��:ע�ᴰ���ࡣ
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
����:  InitInstance(HINSTANCE, int)
Ŀ��:  ����ʵ�����������������
�ڴ˺����У�������ȫ�ֱ����б���ʵ�������
��������ʾ�����򴰿ڡ�
*/
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    HWND hWnd;
    hInst = hInstance; // ��ʵ������洢��ȫ�ֱ�����

    // hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
    //   CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
    hWnd = CreateWindowEx(WS_EX_TOPMOST | WS_EX_TOOLWINDOW, szWindowClass, szTitle, WS_POPUP | WS_VISIBLE,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

    if (!hWnd)
    {
        return FALSE;
    }

    screenCpature->m_ghWnd = hWnd;
    ShowWindow(hWnd, SW_SHOWNORMAL | SW_MAXIMIZE);        //SW_SHOWNORMAL������ʾ���� | SW_MAXIMIZE���ָ������
    UpdateWindow(hWnd);

    SetTimer(hWnd, 3, 1000, NULL);
    return TRUE;
}

/*
����:  WndProc(HWND, UINT, WPARAM, LPARAM)
Ŀ��:    ���������ڵ���Ϣ��
WM_COMMAND	- ����Ӧ�ó���˵�
WM_PAINT	- ����������
WM_DESTROY	- �����˳���Ϣ������
*/
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int wmId, wmEvent;
    PAINTSTRUCT ps;
    HDC hdc;

    //��������	
    UINT WM_TASKBARCREATED; //��ֹexplorer.exe����������ͼ����ʧ
    POINT pt;
    int mid;

    //�������
    POINT mousePoint;

    switch (message)
    {
    case WM_COMMAND:
        wmId = LOWORD(wParam);
        wmEvent = HIWORD(wParam);
        // �����˵�ѡ��: 
        switch (wmId)
        {
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        break;
    case WM_CREATE:  //���ڴ���ʱ����Ϣ
        if (NULL == screenCpature->m_Hook)
        {
            SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, hInst, 0);
        }

        WM_TASKBARCREATED = RegisterWindowMessage(TEXT("TaskbarCreated"));
        screenCpature->m_nid.cbSize = sizeof(NOTIFYICONDATA);
        screenCpature->m_nid.hWnd = hWnd;
        screenCpature->m_nid.uID = IDI_TARY;
        screenCpature->m_nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP | NIF_INFO;  //NIF_INFO������Ϣ����
        screenCpature->m_nid.uCallbackMessage = WM_USER;
        screenCpature->m_nid.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_TARY));
        lstrcpy(screenCpature->m_nid.szTip, screenCpature->m_szAPPName);           //��׼��ʾ�ַ���
        Shell_NotifyIcon(NIM_ADD, &screenCpature->m_nid);

        screenCpature->GetScreenCapture();
        screenCpature->m_hMenu = CreatePopupMenu();
        AppendMenu(screenCpature->m_hMenu, MF_STRING, IDR_START, L"��ʼ");
        AppendMenu(screenCpature->m_hMenu, MF_STRING, IDR_PAUSE, L"��ͣ");
        AppendMenu(screenCpature->m_hMenu, MF_STRING, IDR_EXIT, L"�˳�");
        break;
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        // TODO:  �ڴ���������ͼ����...���������ڣ�ʹ���ɻҶȱ���
        screenCpature->PaintWndDialog(hdc);
        EndPaint(hWnd, &ps);
        break;
    case WM_USER:
        if (lParam == WM_LBUTTONDOWN)
        {
            //MessageBox(hWnd,TEXT("��갴��"),szAPPName,MB_OK);
            ShowWindow(screenCpature->m_ghWnd, SW_SHOW);
        }
        if (lParam == WM_LBUTTONDBLCLK)
        {
            SendMessage(hWnd, WM_CLOSE, wParam, lParam);
        }
        if (lParam == WM_RBUTTONDOWN)
        {
            GetCursorPos(&pt);
            ::SetForegroundWindow(hWnd);  //��������������˵�����ʧ
            //EnableMenuItem(hMenu, IDR_PAUSE, MF_GRAYED);
            mid = TrackPopupMenu(screenCpature->m_hMenu, TPM_RETURNCMD, pt.x, pt.y, NULL, hWnd, NULL); //��ʾ�˵�
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
    case WM_LBUTTONDOWN:  //��갴��
        if (!screenCpature->m_bIsSelect)
        {
            screenCpature->m_bMouseDown = true;
            GetCursorPos(&mousePoint);
            screenCpature->m_rect.left = screenCpature->m_rect.right = mousePoint.x;
            screenCpature->m_rect.top = screenCpature->m_rect.bottom = mousePoint.y;
            InvalidateRgn(hWnd, NULL, false); //InvalidateRgn()�����Ѹ���������ɫ�����Ȼ�����
        }
        else
        {
            screenCpature->m_bIsSelect = false;
            InvalidateRgn(hWnd, NULL, false);
        }
        break;
    case WM_LBUTTONUP: //����ͷ�
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
    case WM_MOUSEMOVE: //����ƶ�
        if ((!screenCpature->m_bIsSelect) && screenCpature->m_bMouseDown)
        {
            GetCursorPos(&mousePoint);
            screenCpature->m_rect.right = mousePoint.x;
            screenCpature->m_rect.bottom = mousePoint.y;
            InvalidateRgn(hWnd, NULL, false);
        }
        break;
    case WM_DESTROY:
        UnhookWindowsHookEx(screenCpature->m_Hook);   //ж�ع���
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
    if (nCode >= 0)  //ע�����0
    {
        PKBDLLHOOKSTRUCT pKeyStruct = (PKBDLLHOOKSTRUCT)lParam;  //���Ի�ȡ�ײ���̲���
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
                        InvalidateRgn(screenCpature->m_ghWnd, NULL, false); //ʹ����ָ�����򲻻�����������봰��ˢ������ʹ֮������ػ�
                    }
                    else
                    {
                        //SendMessage(g_hWnd, WM_CLOSE, NULL, NULL);
                        //DestroyWindow(g_hWnd);
                        ShowWindow(screenCpature->m_ghWnd, SW_HIDE); //���ؽ���
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
                //ShowWindow(g_hWnd, SW_HIDE); //���ؽ���
            }
        }
    }
    return CallNextHookEx(screenCpature->m_Hook, nCode, wParam, lParam); ////�ô��������������
}


