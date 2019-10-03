#include "stdafx.h"
#include "ScrCapManger.h"
using namespace std;

ScreenCapture::ScreenCapture()
:m_hMenu(NULL)
,m_szAPPName(L"��ͼ����")
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
    struct tm t;                                                           //tm�ṹָ��
    time_t now;                                                            //����time_t���ͱ���
    time(&now);                                                            //��ȡϵͳ���ں�ʱ��
    localtime_s(&t, &now);                                                 //��ȡ�������ں�ʱ��
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
//    if (nCode >= 0)  //ע�����0
//    {
//        PKBDLLHOOKSTRUCT pKeyStruct = (PKBDLLHOOKSTRUCT)lParam;  //���Ի�ȡ�ײ���̲���
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
//                        InvalidateRgn(m_ghWnd, NULL, false); //ʹ����ָ�����򲻻�����������봰��ˢ������ʹ֮������ػ�
//                    }
//                    else
//                    {
//                        //SendMessage(g_hWnd, WM_CLOSE, NULL, NULL);
//                        //DestroyWindow(g_hWnd);
//                        ShowWindow(m_ghWnd, SW_HIDE); //���ؽ���
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
//                //ShowWindow(g_hWnd, SW_HIDE); //���ؽ���
//            }
//        }
//    }
//    return CallNextHookEx(m_Hook, nCode, wParam, lParam); ////�ô��������������
//}

void ScreenCapture::PaintWndDialog(HDC hdc)
{
    //��������Ŀ��
    int width, height;

    //��Ļ��ͼ
    HDC mHdc;
    HBITMAP mBmp;
    HBITMAP mOldBmp;

    //����
    LOGPEN pen;
    POINT penWidth;
    penWidth.x = 3;
    penWidth.y = 3;
    pen.lopnStyle = PS_SOLID; //ʵ��
    pen.lopnColor = 0xFFFF00;
    pen.lopnWidth = penWidth;
    HPEN hPen = CreatePenIndirect(&pen);  //����һ�����ʽṹ

    //��ˢ
    LOGBRUSH brush;
    brush.lbStyle = BS_NULL;
    HBRUSH hBrush = CreateBrushIndirect(&brush);

    //��ͼ
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
        Rectangle(mHdc, m_rect.left, m_rect.top, m_rect.right, m_rect.bottom);  //���ʻ����Σ���ˢ��䣻Ҫ���ҵ����λ�ã��ŷ��ɻ�ͼ
    }

    //������ͼ
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
    m_cHdc = CreateCompatibleDC(srcHdc);         //Ϊ��Ļ�豸�������������ݵ��ڴ���������
    m_ixWidth = GetDeviceCaps(srcHdc, HORZRES);
    m_iyHeight = GetDeviceCaps(srcHdc, VERTRES);
    m_cBmp = CreateCompatibleBitmap(srcHdc, m_ixWidth, m_iyHeight); //����һ����Ļ��������ƥ���λͼ
    m_oldBmp = (HBITMAP)SelectObject(m_cHdc, m_cBmp);          //��λͼѡ���ڴ���������
    BitBlt(m_cHdc, 0, 0, m_ixWidth, m_iyHeight, srcHdc, 0, 0, SRCCOPY); //����Ļ�������������ڴ���������

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
    int bytePerPixel = 4;   //Ĭ��32λ
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

    BITMAPINFOHEADER bmiHeader; //��Ϣͷ
    BITMAPFILEHEADER bmfHeader; //�ļ�ͷ
    //����bmp�ļ���Ϣͷ
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

    //����bmp�ļ�ͷ
    DWORD dwBmpSize = ((width * bmiHeader.biBitCount + 31) / 32) * 4 * height;  //����ͼƬ��С
    DWORD dwSizeofDIB = dwBmpSize + sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
    bmfHeader.bfType = 0x4D42; //BM 
    bmfHeader.bfSize = dwSizeofDIB;
    bmfHeader.bfReserved1 = 0;
    bmfHeader.bfReserved2 = 0;
    bmfHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER)+(DWORD)sizeof(BITMAPINFOHEADER);

    //��ȡλͼ����
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

    //�򿪱����ļ��Ի���
    OPENFILENAME ofn = { 0 };
    TCHAR strFilename[MAX_PATH] = { 0 };

    MultiByteToWideChar(CP_ACP, 0, (LPCSTR)strName.c_str(), -1, strFilename, len);
    ofn.lStructSize = sizeof(OPENFILENAME);                                          //�ṹ���С
    ofn.hwndOwner = NULL;                                                            //ӵ���Ŵ��ڣ�Ϊnull��ʾ����ʱ�Ǿ�̬��
    ofn.lpstrFilter = TEXT("�����ļ�\0 * .*\0C / C++ Flie\0 * .cpp; *.c; *.h\0\0");  //�����ļ�
    ofn.nFilterIndex = 1;                                                            //����������
    ofn.lpstrFile = strFilename;                                                       //���յ��ļ�
    ofn.nMaxFile = sizeof(strFilename);                                              //����������
    ofn.lpstrInitialDir = NULL;                                                      //��ʼ��Ŀ¼ΪĬ��
    //ofn.lpstrTitle = TEXT("��ѡ��һ���ļ�");                                         //�Ի������
    //ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;            //�ļ�Ŀ¼������ڣ�����ֻ���ļ�

    //if (GetOpenFileName(&ofn))
    //{
    //	MessageBox(NULL, (LPCTSTR)strFilename, TEXT("ѡ����ļ�"), 0);
    //}
    //else
    //{
    //	MessageBox(NULL, TEXT("��ѡ��һ���ļ�"), NULL, MB_ICONERROR);
    //	//goto loop;
    //}

    ofn.Flags = OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;          //Ŀ¼������ڣ������ļ�ǰ��������
    ofn.lpstrTitle = TEXT("���浽");                                                 //ʹ��ϵͳĬ�ϱ������ռ���
    ofn.lpstrDefExt = TEXT("bmp");                                                   //Ĭ��׷�ӵ���չ��

    if (GetSaveFileName(&ofn))
    {
        MessageBox(NULL, strFilename, TEXT("���浽"), 0);
    }
    else
    {
        DWORD merror = CommDlgExtendedError();
        MessageBox(NULL, TEXT("������һ���ļ���"), NULL, MB_ICONERROR);
    }

    //���ļ��жԻ���
    //TCHAR szBuffer[MAX_PATH] = { 0 };
    //BROWSEINFO bi = { 0 };
    //bi.hwndOwner = NULL;                                                       //ӵ���Ŵ��ھ����ΪNULL��ʾ�Ի����Ƿ�ģ̬�ģ�ʵ��Ӧ����һ�㶼Ҫ��������
    //bi.pszDisplayName = szBuffer;                                              //�����ļ��еĻ�����
    //bi.lpszTitle = TEXT("ѡ��һ���ļ���");                                     //����
    //bi.ulFlags = BIF_NEWDIALOGSTYLE;                                           //���ڿ��Ե�����С�����½��ļ��а�ť
    //LPITEMIDLIST idl = SHBrowseForFolder(&bi);
    //if (SHGetPathFromIDList(idl, szBuffer))                                    //����Ŀ��־���б�ת��Ϊ�ĵ�ϵͳ·��
    //{
    //	MessageBox(NULL, szBuffer, TEXT("��ѡ����ļ���"), 0);
    //}
    //else
    //{
    //	MessageBox(NULL, TEXT("��ѡ��һ���ļ���"), NULL, MB_ICONERROR);
    //}

    //ȷ����Ҫ���ֽ���
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