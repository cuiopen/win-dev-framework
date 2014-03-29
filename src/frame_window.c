/* 
 * 本软件为免费、开源软件。
 * 本软件的版权(包括源码及二进制发布版本)归一切公众所有。
 * 您可以自由使用、传播本软件。
 * 您也可以以任何形式、任何目的使用本软件(包括源码及二进制发布版本)，而不受任何版权限制。
 * =====================
 * 作者: 孙明保
 * 邮箱: sunmingbao@126.com
 */
#include <windows.h>
#include <stdlib.h>
#include "global_symbols.h"
#include "app_property.h"
#include "res.h"

#include "common.h"
#include "utils.h"
#include "file_open_history.h"

TCHAR   szFrameWinClassName[] = TEXT ("frame_win") ;
HWND    hwnd_frame;

HWND hwndTip;

int display_toolbar=1;
int display_statusbar=1;

int TOOLBAR_BUTTON3_DOWN;
char doc_file_path[MAX_FILE_PATH_LEN];
char file_to_open[MAX_FILE_PATH_LEN];

void resize_window(HWND hwnd)
{
    RECT rect;
    GetClientRect(hwnd,&rect);
    SendMessage(hwnd, WM_SIZE, SIZE_RESTORED, MAKELPARAM(rect.right, rect.bottom));
}

void set_frame_title(TCHAR *file_name)
{
    TCHAR info[128];
    sprintf(info, TEXT("%s - %s"), szAppName, file_name);
    SetWindowText(hwnd_frame, info);

}


int doc_save_proc()
{
    int ret;
    if (doc_modified)
    {
        ret=AskConfirmation_3state(hwnd_frame, TEXT("是否保存当前文件?"), szAppName);
        if (IDCANCEL == ret) return 1;
        if (IDYES == ret)
        {
            SendMessage(hwnd_frame, WM_COMMAND, IDM_FILE_SAVE, 0);
            return doc_modified;
        }
    }

    return 0;
}

void open_file()
{
    if (doc_save_proc()) return;

    load_doc_file(file_to_open);;
    strcpy(doc_file_path, file_to_open);
    set_frame_title(strrchr(doc_file_path, '\\')+1);
    update_statusbar();
    update_file_open_history(doc_file_path);
}

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc ;
    PAINTSTRUCT ps ;
    static int cxClient, cyClient;
    HMENU hMenu;
    POINT point;
    RECT		rect ;
    int  item_id;
    
    int ret;

    switch (message)
    {
        case WM_CREATE:
            hwnd_frame = hwnd;
            
            hwndTip =CreateWindowEx(NULL, TOOLTIPS_CLASS, NULL,
                        WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP |TTS_BALLOON,
                        CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,
                        hwnd,
                        NULL,
                        g_hInstance, 
                        NULL);
            
            CreateToolbar();

            CreateStatusBar();

            DragAcceptFiles(hwnd, TRUE);

    set_frame_title(TEXT("无标题"));

            ret=get_last_doc_file(file_to_open);

            if (0==ret) open_file();

            return 0 ;


        case WM_SIZE:
             MoveWindow	(	hwnd_toolbar, 	0, 0,
                cxClient, toolbar_height, TRUE) ;
             
      	    MoveWindow	(	hwnd_statusbar, 0, cyClient-statusbar_height,
                cxClient, statusbar_height, TRUE) ;

          	return 0 ;


        case WM_PAINT :
            hdc = BeginPaint (hwnd, &ps) ;
            //draw some thing here
            EndPaint (hwnd, &ps) ;
            return 0 ;

        case WM_DROPFILES :
            DragQueryFile((HDROP)wParam,
                          0,
                          file_to_open,
                          sizeof(file_to_open));
            DragFinish((HDROP)wParam);
            open_file();
            return 0;

        case WM_INITMENU:

            if (lParam == 0)
            {
                hMenu = GetMenu(hwnd);
                hMenu = GetSubMenu(hMenu, 0);
                hMenu = GetSubMenu(hMenu, 5);
                populate_recent_files(hMenu);
            }


            return 0;

        case 	WM_COMMAND:
            hMenu = GetMenu (hwnd) ;
            item_id = LOWORD(wParam);
            switch (item_id)
            {
                case    IDM_APP_ABOUT:
                DialogBox (g_hInstance, TEXT("ABOUTBOX"), hwnd, AboutDlgProc) ;
               	return 0 ;

                case    IDM_GET_NEW_VERSION:
                ShellExecute(NULL, "open"
                    , "http://sourceforge.net/projects/xb-ether-tester/files/latest/download?source=directory"
                    , NULL, NULL, SW_SHOWNORMAL);

               	return 0 ;

                case    IDM_GET_SOURCE:
                ShellExecute(NULL, "open"
                    , "http://sourceforge.net/projects/xb-ether-tester/files/v2.x/"
                    , NULL, NULL, SW_SHOWNORMAL);

               	return 0 ;

                case    IDM_OPEN_OFFICIAL_SITE:
                ShellExecute(NULL, "open"
                    , "http://sunmingbao.freevar.com/"
                    , NULL, NULL, SW_SHOWNORMAL);

               	return 0 ;

                case    IDM_APP_EXIT:
       				SendMessage (hwnd, WM_CLOSE, 0, 0) ;
       				return 0 ;

                case    IDM_APP_TOOLBAR_WIN:
                    if (display_toolbar)
                    {
                        display_toolbar = 0;
           				ShowWindow(hwnd_toolbar, 0);
                        UpdateWindow (hwnd_frame) ;
                        CheckMenuItem (hMenu, item_id, MF_UNCHECKED) ;

                    }
                    else
                    {
                        display_toolbar= 1;
           				ShowWindow(hwnd_toolbar, 1);
                        UpdateWindow (hwnd_frame) ;
                        CheckMenuItem (hMenu, item_id, MF_CHECKED) ;
                    }
                    resize_window(hwnd_frame);
       				return 0 ;

                case    IDM_APP_STATUSBAR_WIN:
                    if (display_statusbar)
                    {
                        display_statusbar = 0;
           				ShowWindow(hwnd_statusbar, 0);
                        UpdateWindow (hwnd_frame) ;
                        CheckMenuItem (hMenu, item_id, MF_UNCHECKED) ;

                    }
                    else
                    {
                        display_statusbar= 1;
           				ShowWindow(hwnd_statusbar, 1);
                        UpdateWindow (hwnd_frame) ;
                        CheckMenuItem (hMenu, item_id, MF_CHECKED) ;
                    }
                    resize_window(hwnd_frame);
       				return 0 ;


                case    IDT_TOOLBAR_BUTTON1:
                    WinPrintf(hwnd, "IDT_TOOLBAR_BUTTON1");
               	    return 0 ;

                case    IDT_TOOLBAR_BUTTON2:
                    WinPrintf(hwnd, "IDT_TOOLBAR_BUTTON2");
       				return 0 ;

                case    IDT_TOOLBAR_BUTTON3:
                    TOOLBAR_BUTTON3_DOWN = !TOOLBAR_BUTTON3_DOWN;
                    SendMessage(hwnd_toolbar, TB_CHECKBUTTON, IDT_TOOLBAR_BUTTON3, TOOLBAR_BUTTON3_DOWN);
       				return 0 ;

                case    IDM_FILE_SAVE:
                {
                    if (doc_file_path[0]==0)
                    {
                        SendMessage(hwnd, WM_COMMAND, IDM_FILE_SAVE_AS, 0);
                        return 0 ;
                    }
                    save_doc_file(doc_file_path);
                   	return 0 ;
                }

                case    IDM_FILE_SAVE_AS:
                {
                        ret=get_save_file_name(file_to_open, hwnd, DOC_FILE_FILTER, DOC_FILE_SUFFIX);
                        if (ret) return 0 ;
                        save_doc_file(file_to_open);
                        open_file();
                       	return 0 ;
                }
                

                case    IDM_FILE_NEW:
                {
                    if (doc_save_proc()) return 0;
                    doc_file_path[0]=0;
                    set_frame_title(TEXT("无标题"));
                    doc_modified=0;
                    return 0 ;
                }
                
                case    IDM_FILE_OPEN:
                {
                    if (get_open_file_name(file_to_open, hwnd, DOC_FILE_FILTER))
                        return 0;
                    open_file();
                   	return 0 ;
                }

                case    ID_FILE_RECENT_FILE_BEGIN+0:
                case    ID_FILE_RECENT_FILE_BEGIN+1:
                case    ID_FILE_RECENT_FILE_BEGIN+2:
                case    ID_FILE_RECENT_FILE_BEGIN+3:
                case    ID_FILE_RECENT_FILE_BEGIN+4:
                case    ID_FILE_RECENT_FILE_BEGIN+5:
                case    ID_FILE_RECENT_FILE_BEGIN+6:
                case    ID_FILE_RECENT_FILE_BEGIN+7:
                case    ID_FILE_RECENT_FILE_BEGIN+8:
                case    ID_FILE_RECENT_FILE_BEGIN+9:
                {
                    get_file_path_by_idx(file_to_open
                        , item_id - ID_FILE_RECENT_FILE_BEGIN);

                    open_file();
                    return 0 ;

                }


            }

            return 0;

     	case	WM_CLOSE:
            if (doc_save_proc()) return 0;
            DestroyWindow (hwnd) ;
            return 0 ;

        case    WM_DESTROY :
            PostQuitMessage (0) ;
            return 0 ;
    }
    return DefWindowProc (hwnd, message, wParam, lParam) ;
}

int register_frame()
{
    WNDCLASS    wndclass;
    wndclass.style      = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc= WndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance  = g_hInstance;
    wndclass.hIcon      = LoadIcon (g_hInstance, TEXT("my_frame_icon"));
#if 0
    /* 如果想换光标，就用如下代码 */
    wndclass.hCursor    = LoadCursor (g_hInstance, TEXT("my_cursor"));
#else
    wndclass.hCursor      = LoadCursor (NULL, IDC_ARROW);
#endif

    wndclass.hbrBackground  = (HBRUSH)GetStockObject(GRAY_BRUSH);
    wndclass.lpszMenuName   = TEXT("MY_MENU");
    wndclass.lpszClassName  = szFrameWinClassName;

    if (!RegisterClass (&wndclass))
     {
        MessageBox (NULL, TEXT("Register frame window class failed.\r\nProgram requires Windows NT!"),
          szAppName, MB_ICONERROR) ;
        return FAIL;
     }

    return SUCCESS;

}


int create_windows(int iCmdShow)
{
    hwnd_frame = CreateWindow (szFrameWinClassName, szAppName,
            WS_OVERLAPPEDWINDOW|WS_MAXIMIZE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,
            CW_USEDEFAULT, CW_USEDEFAULT,
            CW_USEDEFAULT, CW_USEDEFAULT,
            NULL, NULL, g_hInstance, NULL) ;

    
    ShowWindow (hwnd_frame, SW_MAXIMIZE) ;
    UpdateWindow (hwnd_frame) ;

    return 0;

}




