#include <iostream>
#include <string>
#include "getPowerPlanNames.h"


#include "resource.h"

#ifdef _MSC_VER

#   include <windows.h>
#   include <shellapi.h>

#endif

#include <tchar.h>

using namespace std;

#define     MAX_LOADSTRING 100
#define     WM_USER_SHELLICON WM_USER + 1



// Global Variables:
HINSTANCE           hInst;                                  // Reference to the current instance
NOTIFYICONDATA      nidApp;
HMENU               hPopMenu;
TCHAR               szTitle[MAX_LOADSTRING];                // The title bar text
TCHAR               szWindowClass[MAX_LOADSTRING];          // The main window class name
TCHAR               szApplicationToolTip[MAX_LOADSTRING];   // The main window class name
BOOL                bDisable = FALSE;                       // Teep application state

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);


//
// Function:    _tWinMain
//
// Purpose:     This is the entry point for win32 apps.
//
// Comments:    I believe cmake needs to know that this is a win32 GUI app, hence
//              the `WIN32` declaration in CMakeLists.txt. WIN32 requires these arguments, but we aren't using them
//              here, so we're declaring them unused, so we won't get warnings for it.
//
int WINAPI _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR lpCmdLine,
                     int nCmdShow)
                     {

    // WIN32 requires these params, but we don't use them, and I don't want warnings for this from CLion
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Initialize global strings.
    // LoadString: https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-loadstringa
    //
    // This just loads string resources into the WIN32 instance.
    //
    LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadString(hInstance, IDC_SYSTRAYDEMO, szWindowClass, MAX_LOADSTRING);

    // Attempt to init app, fail if it can't
    MyRegisterClass(hInstance);
    if (!InitInstance (hInstance, nCmdShow)) return FALSE;

    // Load the accelerator table.
    // Accelerator Tables: https://learn.microsoft.com/en-us/windows/win32/learnwin32/accelerator-tables
    //
    // These are used for key combination/key navigation menus. They need more configuration to actually be used
    // for key navigation, so in this case I think they're being used as just an easy way to make a context menu.
    //
    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SYSTRAYDEMO));

    // Main message loop.
    //
    // Message loop info: https://learn.microsoft.com/en-us/windows/win32/winmsg/using-messages-and-message-queues
    // GetMessage: https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getmessage
    //
    // Pretty simple in theory, GetMessage just waits for the next message in a queue. It (supposedly) handles the
    // queuing on its own, which is handy. I believe it's using the resource int that's passed to each action to
    // basically attach a numerical ID to each action so that we don't need to use some sort of pass-trough variable
    // to indicate which action was performed. I.E., `About` button has a resource ID of 104, which we attach to the
    // about button. When that button is pressed, it knows that resource 104 was performed, and message queue knows to
    // dispatch that message to whatever handles resource 104. I think.
    //
    MSG msg;
    while(GetMessage(&msg, nullptr, 0, 0)){
        if(!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    // Return the msg result, if any.
    return (int) msg.wParam;
}


ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style			= CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc	= WndProc;
    wcex.cbClsExtra		= 0;
    wcex.cbWndExtra		= 0;
    wcex.hInstance		= hInstance;
    wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SYSTRAYDEMO));
    wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_SYSTRAYDEMO);
    wcex.lpszClassName	= szWindowClass;
    wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassEx(&wcex);
}


//
// Function InitInstance
//
// Purpose: Saves the global instance reference and creates/spawns the window.
//
// Comments: For WIN32 apps, even if you just want to
//           make a systray icon app, it requires a "window," even if it isn't visible.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
    // Save the hInstance to the global reference. hInstance is the local instance that was just created at this point,
    // it needs to be made global for later use.
    hInst = hInstance;

    // Create the window.
    // CreateWindow: https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-createwindowa
    HWND hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr,
                        nullptr, hInstance, nullptr);

    if(!hWnd) return FALSE;

    // Load the app icon
    // LoacIcon: https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-loadicona
    //
    // TODO: This function has been deprecated in favor of LoadImage:
    //  https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-loadimagea
    //
    HICON hMainIcon = LoadIcon(hInstance, (LPCTSTR)MAKEINTRESOURCE(IDI_SYSTRAYDEMO));

    // Build the notification tray icon functionality.
    // NOTIFTYICONDATA: https://learn.microsoft.com/en-us/windows/win32/api/shellapi/ns-shellapi-notifyicondataa
    nidApp.cbSize = sizeof(NOTIFYICONDATA);
    nidApp.hWnd = (HWND) hWnd; // Specify which window it's attached to
    nidApp.uID = IDI_SYSTRAYDEMO; // Resource ID
    nidApp.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP; // See above link for explanation of these
    nidApp.hIcon = hMainIcon; // Use the icon already described in the main app window
    nidApp.uCallbackMessage = WM_USER_SHELLICON; // Where to send callback messages

    // Load tooltip resource
    LoadString(hInstance, IDS_APPTOOLTIP, nidApp.szTip, MAX_LOADSTRING);

    // Sends the icon to the tray
    // Shell_NotifyIcon: https://learn.microsoft.com/en-us/windows/win32/api/shellapi/nf-shellapi-shell_notifyicona
    Shell_NotifyIcon(NIM_ADD, &nidApp);

    return TRUE;
}


//
// Function: WndProc
//
// Purpose: handles the window procedure. The DispatchMessage() in the main function calls this function from the
//          main message loop.
//
// Comments:
//
// See https://learn.microsoft.com/en-us/windows/win32/learnwin32/writing-the-window-procedure
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    int wmId, wmEvent;
    POINT lpClickPoint;

    switch(message) {

        // This handles shellicon interaction. Any event having to do with the shell icon will be caputured here
        case WM_USER_SHELLICON:
            switch(LOWORD(lParam)){
                case WM_RBUTTONDOWN: // the systray icon has been right clicked
                    // TODO: refactor this
                    UINT uFlag = MF_BYPOSITION|MF_STRING;
                    GetCursorPos(&lpClickPoint);
                    hPopMenu = CreatePopupMenu();
                    InsertMenu(hPopMenu,0xFFFFFFFF,MF_BYPOSITION|MF_STRING,IDM_ABOUT,_T("About"));
                    if ( bDisable == TRUE )
                    {
                        uFlag |= MF_GRAYED;
                    }
                    InsertMenu(hPopMenu,0xFFFFFFFF,uFlag,IDM_TEST2,_T("Test 2")); // Test 2
                    InsertMenu(hPopMenu,0xFFFFFFFF,uFlag,IDM_TEST1,_T("Test 1")); // Test 1
                    InsertMenu(hPopMenu,0xFFFFFFFF,MF_SEPARATOR,IDM_SEP,_T("SEP"));
                    if ( bDisable == TRUE )
                    {
                        InsertMenu(hPopMenu,0xFFFFFFFF,MF_BYPOSITION|MF_STRING,IDM_ENABLE,_T("Enable"));
                    }
                    else
                    {
                        InsertMenu(hPopMenu,0xFFFFFFFF,MF_BYPOSITION|MF_STRING,IDM_DISABLE,_T("Disable"));
                    }
                    InsertMenu(hPopMenu,0xFFFFFFFF,MF_SEPARATOR,IDM_SEP,_T("SEP"));
                    InsertMenu(hPopMenu,0xFFFFFFFF,MF_BYPOSITION|MF_STRING,IDM_EXIT,_T("Exit"));

                    SetForegroundWindow(hWnd);
                    TrackPopupMenu(hPopMenu,TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_BOTTOMALIGN,lpClickPoint.x, lpClickPoint.y,0,hWnd,NULL);
                    return TRUE;
            }
            break;


        case WM_COMMAND:
            wmId = LOWORD(wParam);
            wmEvent = HIWORD(wParam);

            switch(wmId){
                case IDM_ABOUT:
                    DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                    break;
                case IDM_TEST1:
                    MessageBox(nullptr, _T("This is test 1 TEST"), _T("Test 1"), MB_OK);
                    break;
                case IDM_TEST2:
                    MessageBox(nullptr, _T("This is test 2 TEST"), _T("Test 2"), MB_OK);
                    break;
                case IDM_DISABLE:
                    bDisable = TRUE;
                    break;
                case IDM_ENABLE:
                    bDisable = FALSE;
                    break;
                case IDM_EXIT:
                    Shell_NotifyIcon(NIM_DELETE, &nidApp);
                    DestroyWindow(hWnd);
                    break;
                default:
                    return DefWindowProc(hWnd, message, wParam, lParam);
            }
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// TODO: add the About function from the end of the example project

//int main() {
//    HWND hWnd;
//    HICON hMainIcon;
//
//
//    nidApp.cbSize = sizeof(NOTIFYICONDATA);
//    nidApp.hWnd = (HWND) hWnd;
//    nidApp.uID = IDI_SYSTRAYDEMO;
//    nidApp.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
//    nidApp.hIcon = hMainIcon;
//    nidApp.uCallbackMessage = WM_USER_SHELLICON;
//    LoadString(hInstance, IDS_APPTOOLTIP,nidApp.szTip,MAX_LOADSTRING);
//
//    Shell_NotifyIcon(NIM_ADD, &nidApp);
//
////    for(auto & plan : getPowerPlanNames())
////        std::cout << plan << endl;
////    return 0;
//}