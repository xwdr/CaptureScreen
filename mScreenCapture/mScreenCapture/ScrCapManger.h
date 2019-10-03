#pragma once
#include <Shlobj.h>               //选择文件夹对话框
#include <Commdlg.h>              //打开保存文件对话框
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
    //LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);//全局钩子
    string GetLocalTime();                                                   //获取本地时间
    void mScreenCapture();                                                   //截全屏
    void PaintWndDialog(HDC hdc);                                            //截区域
    void GetScreenCapture();                                                 //初始化彩色，灰色    
    void ReleaseBmp();                                                       //释放资源
    void SaveScreenCapture(string strTime);                                  //保存截图
    string CreateSaveFoldr(string fileName);                                   //创建文件保存路径
    void ShowTaryMsg();                                                      //托盘气泡显示
       
private:
    void CovertToGrayBitmap(HBITMAP hSourceBmp, HDC sourceDc);               //彩色转灰色
    void SaveFileName(string strTime, BITMAPFILEHEADER bmfHeader, BITMAPINFOHEADER bmiHeader, DWORD dwBmpSize, char *bmpData); //保存文件

public:
    NOTIFYICONDATA  m_nid;                             //定义托盘参数
    HMENU           m_hMenu;                           //定义托盘菜单
    LPCTSTR         m_szAPPName;
    HHOOK           m_Hook;                            //全局钩子
    HWND            m_ghWnd;                           //全局句柄
    int             m_ixWidth;
    int             m_iyHeight;
    bool            m_bIsShow;                         //是否显示截屏区域
    bool            m_bMouseDown;                      //鼠标按下
    bool            m_bIsSelect;                       //是否显示截图区域
    string          m_sfileName;
    RECT            m_rect;                            //框大小

private:
    HDC            m_grayHdc;                         //灰度图
    HBITMAP        m_grayBmp;
    HBITMAP        m_grayOld;
    HDC            m_cHdc;                            //彩色图
    HBITMAP        m_cBmp;
    HBITMAP        m_oldBmp;

};
