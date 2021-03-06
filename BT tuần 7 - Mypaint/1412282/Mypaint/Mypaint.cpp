// Mypaint.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Mypaint.h"
#include <iostream>
#include <vector>
using namespace std;
#include <windowsX.h>
#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

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
    LoadStringW(hInstance, IDC_MYPAINT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MYPAINT));

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

    return (int) msg.wParam;
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

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MYPAINT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_MYPAINT);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

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
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
class CShape {
public:
	virtual void Draw(HDC hdc) = 0;
	virtual CShape* Create() = 0;
	virtual void SetData(int a, int b, int c, int d) = 0;
};

class CLine : public CShape {
private:
	int x1;
	int y1;
	int x2;
	int y2;
public:
	void Draw(HDC hdc) {
		MoveToEx(hdc, x1, y1, NULL);
		LineTo(hdc, x2, y2);
	}

	CShape* Create() { return new CLine; }

	void SetData(int a, int b, int c, int d) {
		x1 = a;
		y1 = b;
		x2 = c;
		y2 = d;
	}
};

class CRectangle : public CShape {
private:
	int x1;
	int y1;
	int x2;
	int y2;
public:
	void Draw(HDC hdc) {
		Rectangle(hdc, x1, y1, x2, y2);
	}

	CShape* Create() { return new CRectangle; }

	void SetData(int a, int b, int c, int d) {
		x1 = a;
		y1 = b;
		x2 = c;
		y2 = d;
	}
};

vector<CShape*> shapes;
vector<CShape*> prototypes;
int startX;
int startY;
int lastX;
int lastY;
int shift;
bool isDrawing = FALSE;
#define LINE 0
#define RECTANGLE 1
#define CIRCLE 2


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {

	case WM_CREATE:
	{
		prototypes.push_back(new CLine);
		prototypes.push_back(new CRectangle);

	}
	break;
	case WM_LBUTTONDOWN:
	{
		int x = GET_X_LPARAM(lParam);
		int y = GET_Y_LPARAM(lParam);
		if (!isDrawing)
		{
			isDrawing = TRUE;
			startX = x;
			startY = y;
			
		}
		
	}
	break;
	case WM_MOUSEMOVE: {
		int x = GET_X_LPARAM(lParam);
		int y = GET_Y_LPARAM(lParam);
		

		if (isDrawing) {
			lastX = x;
			lastY = y;

			WCHAR buffer[200];
			wsprintf(buffer, L"%d, %d", x, y);
			SetWindowText(hWnd, buffer);
		}
			InvalidateRect(hWnd, NULL, TRUE);
		}
	
	break;
	case WM_LBUTTONUP: {
		int x = GET_X_LPARAM(lParam);
		int y = GET_Y_LPARAM(lParam);

		CLine* line = new CLine;
		line->SetData(startX, startY, x, y);
		shapes.push_back(line);

		isDrawing = FALSE;

		InvalidateRect(hWnd, NULL, TRUE);
		

	}
	break;
	case WM_KEYDOWN:
		if (wParam == VK_SHIFT)
		{
			shift = 1;
		}
		break;
	case WM_KEYUP:
		if (wParam == VK_SHIFT)
		{
			shift = 0;
	}
		break;
	case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
			case ID_DRAW_LINE32771: {
				CShape* shape = prototypes[LINE]->Create();
				int k, l, m, n;
				k = 100;
				l = 100;
				m = 200;
				n = 200;
				shape->SetData(k, l, m, n);
				shapes.push_back(shape);

				InvalidateRect(hWnd, NULL, TRUE);
				
				/*CShape* shape = prototypes[ID_DRAW_LINE32771 - ID_DRAW_LINE32771]->Create(100, 100, 250, 200);
				shapes.push_back(shape);
				InvalidateRect(hWnd, NULL, TRUE);*/
			}
			break;
			case ID_DRAW_RECTANGLE32772:
			{
				CShape* shape = prototypes[RECTANGLE]->Create();
				shape->SetData(250, 250, 450, 200);
				shapes.push_back(shape);
				
				InvalidateRect(hWnd, NULL, TRUE);
				/*CShape* shape = prototypes[ID_DRAW_RECTANGLE32772 - ID_DRAW_LINE32771]->Create(100, 100, 250, 200);
				shapes.push_back(shape);
				InvalidateRect(hWnd, NULL, TRUE);*/
			}
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
			for (int i = 0; i < shapes.size(); i++) {
				shapes[i]->Draw(hdc);
			}

			if (isDrawing) {
				MoveToEx(hdc, startX, startY, NULL);
				LineTo(hdc, lastX, lastY);
			}
			if (shift == 1)
			{
				if (isDrawing) {
					CShape* shape = prototypes[RECTANGLE]->Create();
					shape->SetData(350, 350, 450, 450);
					shapes.push_back(shape);
				}
			}
			EndPaint(hWnd, &ps);

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
