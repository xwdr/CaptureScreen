#pragma once
#include <Shlobj.h>               //ѡ���ļ��жԻ���
#include <Commdlg.h>              //�򿪱����ļ��Ի���
#include <iostream>
#include <string.h>
#include <psapi.h>
#include <fstream>
#include <stdio.h>
#include <time.h>
#include <string>
#include <string.h>
#include <commdlg.h>
#include "resource.h"

#pragma comment(lib,"Shell32.lib")
using namespace std;

class ScreenCapture
{
public:
    ScreenCapture();
    ~ScreenCapture();
    //LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);//ȫ�ֹ���
    string GetLocalTime();                                                   //��ȡ����ʱ��
    void mScreenCapture();                                                   //��ȫ��
    void PaintWndDialog(HDC hdc);                                            //������
    void GetScreenCapture();                                                 //��ʼ����ɫ����ɫ    
    void ReleaseBmp();                                                       //�ͷ���Դ
    void SaveScreenCapture(string strTime);                                  //�����ͼ
    string CreateSaveFoldr(string fileName);                                   //�����ļ�����·��
    void ShowTaryMsg();                                                      //����������ʾ
       
private:
    void CovertToGrayBitmap(HBITMAP hSourceBmp, HDC sourceDc);               //��ɫת��ɫ
    void SaveFileName(string strTime, BITMAPFILEHEADER bmfHeader, BITMAPINFOHEADER bmiHeader, DWORD dwBmpSize, char *bmpData); //�����ļ�

public:
    NOTIFYICONDATA  m_nid;                             //�������̲���
    HMENU           m_hMenu;                           //�������̲˵�
    LPCTSTR         m_szAPPName;
    HHOOK           m_Hook;                            //ȫ�ֹ���
    HWND            m_ghWnd;                           //ȫ�־��
    int             m_ixWidth;
    int             m_iyHeight;
    bool            m_bIsShow;                         //�Ƿ���ʾ��������
    bool            m_bMouseDown;                      //��갴��
    bool            m_bIsSelect;                       //�Ƿ���ʾ��ͼ����
    string          m_sfileName;
    RECT            m_rect;                            //���С

private:
    HDC            m_grayHdc;                         //�Ҷ�ͼ
    HBITMAP        m_grayBmp;
    HBITMAP        m_grayOld;
    HDC            m_cHdc;                            //��ɫͼ
    HBITMAP        m_cBmp;
    HBITMAP        m_oldBmp;

};
