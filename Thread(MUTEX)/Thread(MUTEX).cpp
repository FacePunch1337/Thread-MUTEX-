// Thread(MUTEX).cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "Thread(MUTEX).h"

#define MAX_LOADSTRING      100
#define CMD_BUTTON_START    1003

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
HWND list1;
HWND list2;
HANDLE semaphore;
HANDLE mutex;



HANDLE hts[12]; // threads handles
int activeThreads = 0;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void                StartButton();

DWORD WINAPI ThreadProc(LPVOID);



int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDI_THREADMUTEX, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDI_THREADMUTEX));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_THREADMUTEX));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDI_THREADMUTEX);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Store instance handle in our global variable

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        semaphore = CreateSemaphore(NULL, 3, 3, NULL);
        if (semaphore == NULL) {
            MessageBoxW(NULL, L"Semaphore error", L"Error", MB_OK | MB_ICONSTOP);
            DestroyWindow(hWnd);
            return 0;
        }
        mutex = CreateMutex(NULL, FALSE, NULL);
        if (mutex == NULL) {
            MessageBoxW(NULL, L"Mutex error", L"Error", MB_OK | MB_ICONSTOP);
            DestroyWindow(hWnd);
            return 0;
        }
        CreateWindowW(L"Button", L"Start", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            10, 10, 75, 23, hWnd, (HMENU)CMD_BUTTON_START, hInst, NULL);
        list1 = CreateWindowW(L"Listbox", L"", WS_CHILD | WS_VISIBLE,
            10, 40, 250, 350, hWnd, 0, hInst, NULL);
        list2 = CreateWindowW(L"Listbox", L"", WS_CHILD | WS_VISIBLE,
            280, 40, 170, 350, hWnd, 0, hInst, NULL);
        break;
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Parse the menu selections:
        switch (wmId)
        {
        case CMD_BUTTON_START:
            StartButton();
            break;
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
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
        // TODO: Add any drawing code that uses hdc here...
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        CloseHandle(mutex);
        CloseHandle(semaphore);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
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



float deposit;
struct DepData {
    int month;
    float percent;
    DepData(int m, float p) : month{ m }, percent{ p } {}
};


void StartButton() {
    SendMessageW(list1, LB_RESETCONTENT, 0, 0);
    SendMessageW(list2, LB_RESETCONTENT, 0, 0);
    deposit = 100;
    activeThreads = 0;
    for (size_t i = 0; i < 12; i++)
    {
        hts[i] = CreateThread(NULL,
            0,
            ThreadProc,
            new DepData(1 + i, 10),
            0,
            NULL
        );
    }
}

DWORD WINAPI ThreadProc(LPVOID params) {
    DepData* data = (DepData*)params;
    WCHAR txt[100], total[50];

    DWORD waitRes = WaitForSingleObject(semaphore, 10);
    if (waitRes == WAIT_OBJECT_0) {
        waitRes = WaitForSingleObject(mutex, INFINITE);
        if (waitRes == WAIT_OBJECT_0) {
            activeThreads++;
            deposit *= 1 + data->percent / (float)100.0;
            _snwprintf_s(txt, 100, L"%d, %.2f = %.2f", data->month, data->percent, deposit);
            SendMessage(list1, LB_ADDSTRING, 100, (LPARAM)txt);
            _snwprintf_s(txt, 100, L"thread - %d, month - %d", activeThreads, data->month);
            SendMessage(list2, LB_ADDSTRING, 100, (LPARAM)txt);
            delete data;

            activeThreads--;
            ReleaseMutex(mutex);
        }
        ReleaseSemaphore(semaphore, 1, NULL);
    }
    else if (waitRes == WAIT_TIMEOUT) {
        _snwprintf_s(txt, 100, L"%d timeout", data->month);
        SendMessage(list2, LB_ADDSTRING, 100, (LPARAM)txt);
    }
    else {
        _snwprintf_s(txt, 100, L"%d error %d", data->month, GetLastError());
        SendMessage(list2, LB_ADDSTRING, 100, (LPARAM)txt);
    }

    return 0;
}