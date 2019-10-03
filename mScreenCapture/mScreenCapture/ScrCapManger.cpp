#include "stdafx.h"
#include "ScrCapManger.h"
using namespace std;

ScreenCapture::ScreenCapture()
:m_hMenu(NULL)
,m_szAPPName(L"截图程序")
,m_Hook(NULL)
,m_ixWidth(0)
,m_iyHeight(0)
,m_bIsShow(true)
,m_bIsSelect(false)
,m_bMouseDown(false)
,m_ghWnd(NULL)
,m_sfileName("")
{

}

ScreenCapture::~ScreenCapture()
{
    ReleaseBmp();
}

void ScreenCapture::ShowTaryMsg()
{
    lstrcpy(m_nid.szInfoTitle, m_szAPPName);
    lstrcpy(m_nid.szInfo, TEXT("this is a message!"));
    m_nid.uTimeout = 1000;
    Shell_NotifyIcon(NIM_MODIFY, &m_nid);
}

string ScreenCapture::GetLocalTime()
{
    struct tm t;                                                           //tm结构指针
    time_t now;                                                            //声明time_t类型变量
    time(&now);                                                            //获取系统日期和时间
    localtime_s(&t, &now);                                                 //获取当地日期和时间
    char strTime[1024] = { 0 };
    sprintf_s(strTime, "%02d-%02d-%02d.bmp", t.tm_hour, t.tm_min, t.tm_sec); // t.tm_year + 1900, t.tm_mon + 1, t.tm_mday,
    return strTime;
}

void ScreenCapture::SaveFileName(string strTime, BITMAPFILEHEADER bmfHeader, BITMAPINFOHEADER bmiHeader, DWORD dwBmpSize, char *bmpData)
{
    ofstream of(strTime, ios_base::binary);
    of.write((char *)&bmfHeader, sizeof(BITMAPFILEHEADER));
    of.write((char *)&bmiHeader, sizeof(BITMAPINFOHEADER));
    of.write(bmpData, dwBmpSize);
    of.close();
}

//LRESULT CALLBACK  ScreenCapture::KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
//{
//    if (nCode >= 0)  //注意等于0
//    {
//        PKBDLLHOOKSTRUCT pKeyStruct = (PKBDLLHOOKSTRUCT)lParam;  //可以获取底层键盘操作
//        if (wParam == WM_KEYUP)
//        {
//            //if (GetKeyState(VK_ESCAPE) < 0)
//            if (pKeyStruct->vkCode == VK_ESCAPE)
//            {
//                if (m_bIsShow)
//                {
//                    if (m_bIsSelect)
//                    {
//                        m_bIsSelect = false;
//                        InvalidateRgn(m_ghWnd, NULL, false); //使窗口指定区域不活动，并将它加入窗口刷新区，使之可随后被重画
//                    }
//                    else
//                    {
//                        //SendMessage(g_hWnd, WM_CLOSE, NULL, NULL);
//                        //DestroyWindow(g_hWnd);
//                        ShowWindow(m_ghWnd, SW_HIDE); //隐藏界面
//                    }
//                }
//            }
//            else if (pKeyStruct->vkCode == VK_RETURN)
//            {
//                if (m_bIsSelect)
//                {
//                    m_sfileName = GetLocalTime();
//                    CreateSaveFoldr(m_sfileName);
//                    SaveScreenCapture(m_sfileName);
//                }
//            }
//            //ctrl+x
//            else if (((GetKeyState(VK_CONTROL) & 0x80) == 0x80) && (pKeyStruct->vkCode == 0x58))
//            {
//                //ScreenCapture();
//                //ShowWindow(g_hWnd, SW_HIDE); //隐藏界面
//            }
//        }
//    }
//    return CallNextHookEx(m_Hook, nCode, wParam, lParam); ////让代码继续后续处理
//}

void ScreenCapture::PaintWndDialog(HDC hdc)
{
    //设置区域的宽高
    int width, height;

    //屏幕截图
    HDC mHdc;
    HBITMAP mBmp;
    HBITMAP mOldBmp;

    //画笔
    LOGPEN pen;
    POINT penWidth;
    penWidth.x = 3;
    penWidth.y = 3;
    pen.lopnStyle = PS_SOLID; //实线
    pen.lopnColor = 0xFFFF00;
    pen.lopnWidth = penWidth;
    HPEN hPen = CreatePenIndirect(&pen);  //创建一个画笔结构

    //画刷
    LOGBRUSH brush;
    brush.lbStyle = BS_NULL;
    HBRUSH hBrush = CreateBrushIndirect(&brush);

    //截图
    mHdc = CreateCompatibleDC(hdc);
    mBmp = CreateCompatibleBitmap(hdc, m_ixWidth, m_iyHeight);
    mOldBmp = (HBITMAP)SelectObject(mHdc, mBmp);
    BitBlt(mHdc, 0, 0, m_ixWidth, m_iyHeight, m_grayHdc, 0, 0, SRCCOPY);

    SelectObject(mHdc, hBrush);
    SelectObject(mHdc, hPen);
    if (m_bIsSelect || m_bMouseDown)
    {
        width = m_rect.right - m_rect.left;
        height = m_rect.bottom - m_rect.top;
        BitBlt(mHdc, m_rect.left, m_rect.top, width, height, m_cHdc, m_rect.left, m_rect.top, SRCCOPY);
        Rectangle(mHdc, m_rect.left, m_rect.top, m_rect.right, m_rect.bottom);  //画笔画矩形，画刷填充；要先找到鼠标位置，才方可绘图
    }

    //结束截图
    BitBlt(hdc, 0, 0, m_ixWidth, m_iyHeight, mHdc, 0, 0, SRCCOPY);
    SelectObject(mHdc, mBmp);
    DeleteObject(mBmp);
    DeleteDC(mHdc);
}

void ScreenCapture::GetScreenCapture()
{
    HDC srcHdc;
    srcHdc = CreateDC(L"DISPLAY", NULL, NULL, NULL);
    //srcHdc = GetDC(NULL);
    m_cHdc = CreateCompatibleDC(srcHdc);         //为屏幕设备描述符表创建兼容的内存描述符表
    m_ixWidth = GetDeviceCaps(srcHdc, HORZRES);
    m_iyHeight = GetDeviceCaps(srcHdc, VERTRES);
    m_cBmp = CreateCompatibleBitmap(srcHdc, m_ixWidth, m_iyHeight); //创建一个屏幕描述符相匹配的位图
    m_oldBmp = (HBITMAP)SelectObject(m_cHdc, m_cBmp);          //将位图选择到内存描述符中
    BitBlt(m_cHdc, 0, 0, m_ixWidth, m_iyHeight, srcHdc, 0, 0, SRCCOPY); //把屏幕描述符拷贝到内存描述符中

    m_grayBmp = CreateCompatibleBitmap(srcHdc, m_ixWidth, m_iyHeight);
    m_grayHdc = CreateCompatibleDC(srcHdc);
    m_grayOld = (HBITMAP)SelectObject(m_grayHdc, m_grayBmp);
    BitBlt(m_grayHdc, 0, 0, m_ixWidth, m_iyHeight, srcHdc, 0, 0, SRCCOPY);
    m_grayBmp = (HBITMAP)SelectObject(m_grayHdc, m_grayOld);
    CovertToGrayBitmap(m_grayBmp, m_grayHdc);
    m_grayOld = (HBITMAP)SelectObject(m_grayHdc, m_grayBmp);
    DeleteDC(srcHdc);
}

void ScreenCapture::CovertToGrayBitmap(HBITMAP hSourceBmp, HDC sourceDc)
{
    HBITMAP retBmp = hSourceBmp;
    BITMAPINFO bmpInfo;
    ZeroMemory(&bmpInfo, sizeof(BITMAPINFO));
    bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    GetDIBits(sourceDc, retBmp, 0, 0, NULL, &bmpInfo, DIB_RGB_COLORS);
    BYTE* bits = new BYTE[bmpInfo.bmiHeader.biSizeImage];
    GetBitmapBits(retBmp, bmpInfo.bmiHeader.biSizeImage, bits);
    int bytePerPixel = 4;   //默认32位
    if (bmpInfo.bmiHeader.biBitCount == 24)
    {
        bytePerPixel = 3;
    }
    for (DWORD i = 0; i < bmpInfo.bmiHeader.biSizeImage; i += bytePerPixel)
    {
        BYTE r = *(bits + i);
        BYTE g = *(bits + i + 1);
        BYTE b = *(bits + i + 2);
        *(bits + i) = *(bits + i + 1) = *(bits + i + 2) = (r + b + g) / 3;
    }
    SetBitmapBits(retBmp, bmpInfo.bmiHeader.biSizeImage, bits);
    delete[] bits;
}

void ScreenCapture::SaveScreenCapture(string strTime)
{
    HDC hMeDc, hScrDc;
    HBITMAP hMeOld, hMeBmp;
    int width, height;
    hScrDc = CreateDC(L"DISPLAY", NULL, NULL, NULL);
    hMeDc = CreateCompatibleDC(hScrDc);
    width = m_rect.right - m_rect.left;
    height = m_rect.bottom - m_rect.top;
    hMeBmp = CreateCompatibleBitmap(hScrDc, width, height);

    hMeOld = (HBITMAP)SelectObject(hMeDc, hMeBmp);
    BitBlt(hMeDc, 0, 0, width, height, m_cHdc, m_rect.left, m_rect.top, SRCCOPY);
    hMeBmp = (HBITMAP)SelectObject(hMeDc, hMeOld);

    BITMAPINFOHEADER bmiHeader; //信息头
    BITMAPFILEHEADER bmfHeader; //文件头
    //设置bmp文件信息头
    bmiHeader.biBitCount = 16;
    bmiHeader.biClrImportant = 0;
    bmiHeader.biClrUsed = 0;
    bmiHeader.biCompression = BI_RGB;
    bmiHeader.biHeight = height;
    bmiHeader.biWidth = width;
    bmiHeader.biPlanes = 1;
    bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmiHeader.biSizeImage = 0;
    bmiHeader.biXPelsPerMeter = 0;
    bmiHeader.biYPelsPerMeter = 0;

    //设置bmp文件头
    DWORD dwBmpSize = ((width * bmiHeader.biBitCount + 31) / 32) * 4 * height;  //设置图片大小
    DWORD dwSizeofDIB = dwBmpSize + sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
    bmfHeader.bfType = 0x4D42; //BM 
    bmfHeader.bfSize = dwSizeofDIB;
    bmfHeader.bfReserved1 = 0;
    bmfHeader.bfReserved2 = 0;
    bmfHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER)+(DWORD)sizeof(BITMAPINFOHEADER);

    //获取位图数据
    char *bmpData = new char[dwBmpSize];
    ZeroMemory(bmpData, dwBmpSize);
    GetDIBits(hMeDc, hMeBmp, 0, (UINT)height, bmpData, (BITMAPINFO*)&bmiHeader, DIB_RGB_COLORS);

    SaveFileName(strTime, bmfHeader, bmiHeader, dwBmpSize, bmpData);

    m_bIsSelect = false;
    m_bIsShow = false;
    ShowWindow(m_ghWnd, SW_HIDE);

    delete[]bmpData;
    DeleteDC(hScrDc);
    DeleteDC(hMeDc);
    DeleteObject(hMeBmp);
    DeleteObject(hMeOld);
}

string ScreenCapture::CreateSaveFoldr(string fileName)
{
    string strName = fileName;
    int len = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)strName.c_str(), -1, NULL, 0);

    //打开保存文件对话框
    OPENFILENAME ofn = { 0 };
    TCHAR strFilename[MAX_PATH] = { 0 };

    MultiByteToWideChar(CP_ACP, 0, (LPCSTR)strName.c_str(), -1, strFilename, len);
    ofn.lStructSize = sizeof(OPENFILENAME);                                          //结构体大小
    ofn.hwndOwner = NULL;                                                            //拥有着窗口，为null表示窗口时非静态的
    ofn.lpstrFilter = TEXT("所有文件\0 * .*\0C / C++ Flie\0 * .cpp; *.c; *.h\0\0");  //过滤文件
    ofn.nFilterIndex = 1;                                                            //过滤器索引
    ofn.lpstrFile = strFilename;                                                       //接收的文件
    ofn.nMaxFile = sizeof(strFilename);                                              //缓冲区长度
    ofn.lpstrInitialDir = NULL;                                                      //初始化目录为默认
    //ofn.lpstrTitle = TEXT("请选择一个文件");                                         //对话框标题
    //ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;            //文件目录必须存在，隐藏只读文件

    //if (GetOpenFileName(&ofn))
    //{
    //	MessageBox(NULL, (LPCTSTR)strFilename, TEXT("选择的文件"), 0);
    //}
    //else
    //{
    //	MessageBox(NULL, TEXT("请选择一个文件"), NULL, MB_ICONERROR);
    //	//goto loop;
    //}

    ofn.Flags = OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;          //目录必须存在，覆盖文件前发出警告
    ofn.lpstrTitle = TEXT("保存到");                                                 //使用系统默认标题留空即可
    ofn.lpstrDefExt = TEXT("bmp");                                                   //默认追加的扩展名

    if (GetSaveFileName(&ofn))
    {
        MessageBox(NULL, strFilename, TEXT("保存到"), 0);
    }
    else
    {
        DWORD merror = CommDlgExtendedError();
        MessageBox(NULL, TEXT("请输入一个文件名"), NULL, MB_ICONERROR);
    }

    //打开文件夹对话框
    //TCHAR szBuffer[MAX_PATH] = { 0 };
    //BROWSEINFO bi = { 0 };
    //bi.hwndOwner = NULL;                                                       //拥有着窗口句柄，为NULL表示对话框是非模态的，实际应用中一般都要有这个句柄
    //bi.pszDisplayName = szBuffer;                                              //接收文件夹的缓冲区
    //bi.lpszTitle = TEXT("选择一个文件夹");                                     //标题
    //bi.ulFlags = BIF_NEWDIALOGSTYLE;                                           //窗口可以调整大小，有新建文件夹按钮
    //LPITEMIDLIST idl = SHBrowseForFolder(&bi);
    //if (SHGetPathFromIDList(idl, szBuffer))                                    //把项目标志符列表转换为文档系统路径
    //{
    //	MessageBox(NULL, szBuffer, TEXT("你选择的文件夹"), 0);
    //}
    //else
    //{
    //	MessageBox(NULL, TEXT("请选择一个文件夹"), NULL, MB_ICONERROR);
    //}

    //确定需要的字节数
    char *pBuffer;
    int nLen = WideCharToMultiByte(CP_ACP, 0, ofn.lpstrFile, -1, NULL, 0, NULL, FALSE);
    if (nLen !=0 )
    {
        pBuffer = (char*)malloc(nLen);
        WideCharToMultiByte(CP_ACP, 0, ofn.lpstrFile, -1, pBuffer, nLen, NULL, FALSE);
        return pBuffer;
        free(pBuffer);
    }

    return NULL;
}

void ScreenCapture::ReleaseBmp()
{
    DeleteObject(m_cBmp);
    DeleteObject(m_oldBmp);
    SelectObject(m_grayHdc, m_grayBmp);
    DeleteObject(m_grayBmp);
    DeleteObject(m_grayOld);
}