// 1412282_QuickLaunch.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "1412282_QuickLaunch.h"
#include "Data.h"
#include "Hook.h"
#include "Notification.h"
#include "ScanApp.h"
#include <string>
#include <windowsx.h>
#include <commctrl.h>
#include <shellapi.h>
#include "Statistics.h"
using namespace std;
#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#pragma comment(lib, "ComCtl32.lib")
#define MAX_LOADSTRING 100
#define MAX_LENGTH 255
// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
HWND global_hWnd = NULL;
HWND dialog_hWnd = NULL;
HWND txtSearch = NULL;
HWND button_App[10];
HFONT hFont;
DataFile dataFile; // List object chứa dữ liệu
vector<int> index_file; // list thứ tự file các button tìm kiếm
int start = 0;
HWND wait_hWnd;
HANDLE scan_Handle;

wstring folder_scanning;
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
    LoadStringW(hInstance, IDC_MY1412282QUICKLAUNCH, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MY1412282QUICKLAUNCH));

    MSG msg;

    // Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		SetForegroundWindow(dialog_hWnd);
		SetFocus(txtSearch);
		SetActiveWindow(dialog_hWnd);
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MY1412282QUICKLAUNCH));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_MY1412282QUICKLAUNCH);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDC_MY1412282QUICKLAUNCH));

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

   global_hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
	   400, 400, 700, 300, nullptr, nullptr, hInstance, nullptr);
   if (!global_hWnd)
   {
      return FALSE;
   }
   INITCOMMONCONTROLSEX icc;
   icc.dwSize = sizeof(icc);
   icc.dwICC = ICC_WIN95_CLASSES;
   InitCommonControlsEx(&icc);
   // Lấy font hệ thống
   LOGFONT lf;
   GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
   hFont = CreateFont(-15, 0,
	   lf.lfEscapement, lf.lfOrientation, lf.lfWeight,
	   lf.lfItalic, lf.lfUnderline, lf.lfStrikeOut, lf.lfCharSet,
	   lf.lfOutPrecision, lf.lfClipPrecision, lf.lfQuality,
	   lf.lfPitchAndFamily, lf.lfFaceName);

   doInstallHook(global_hWnd);
   CreateNotifyIcon(global_hWnd, hInst);
   UpdateWindow(global_hWnd);
   vector<wstring> folder_scan;
   folder_scan.push_back(L"C:\\Program Files");// thư mục app 64 bit
   folder_scan.push_back(L"C:\\Program Files (x86)");//thư mục app 32 bit
   folder_scan.push_back(L"C:\\Windows\\System32");

   if (dataFile.GetListNode().Size())
	   OpenSearchBox();
   else {
	   for (int i = 0; i < (int)folder_scan.size(); i++)
	   {
		   ScanFile(folder_scan[i]);
	   }

	   OpenSearchBox();
   }

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
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		HANDLE_MSG(hWnd, WM_COMMAND, OnCommand);
		HANDLE_MSG(hWnd, WM_DESTROY, OnDestroy);

	case WM_OPENSEARCHBOX:
		OpenSearchBox();
		break;

	case WM_NOTIMESSAGE:
		switch (lParam)
		{

		case WM_LBUTTONDBLCLK:
			OpenSearchBox();
			break;
		case WM_RBUTTONDOWN:
			ShowContextMenu(hWnd);
			break;
		default:
			return 0;
		}
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}



LRESULT CALLBACK edit_subProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	
	switch (msg)
	{
	
	case WM_KEYUP:
		if (GetAsyncKeyState(VK_LCONTROL))
		{
			for (int i = 0; i < 9; i++)
			{
				if (LOWORD(wParam) == 0x31 + i)
				{
					if (index_file[i] != -1)
					{
						dataFile.UpdateList(index_file[i]);
						dataFile.GetListNode().Excute(index_file[i]);
						EndDialog(dialog_hWnd, FALSE);
					}
				}
			}
			if (LOWORD(wParam) == 0x30)
			{
				if (index_file[10] != -1)
				{
					dataFile.UpdateList(index_file[10]);
					dataFile.GetListNode().Excute(index_file[10]);
					EndDialog(dialog_hWnd, FALSE);
				}
			}
		}
		else SearchAppInList();
		switch (LOWORD(wParam))
		{
		
		case VK_RETURN:
			ClickRun();
			break;
		case VK_ESCAPE:
			EndDialog(dialog_hWnd, FALSE);
			break;
		}
	
		break;
	
	default:
		return DefSubclassProc(hWnd, msg, wParam, lParam);
		
	}
	return DefSubclassProc(hWnd, msg, wParam, lParam);
}

INT_PTR CALLBACK Dialog_WndProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	
	dialog_hWnd = hDlg;
	
	switch (message)
	{
		
		case WM_INITDIALOG:
		{
			txtSearch = GetDlgItem(hDlg, IDC_DIALOG_EDIT);
			OnCreate(hDlg);
			SetWindowSubclass(txtSearch, edit_subProc, 0, 0);

			SetWindowPos(global_hWnd, HWND_TOPMOST, 400, 400, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			SetForegroundWindow(dialog_hWnd);
			SetFocus(txtSearch);
			SetActiveWindow(dialog_hWnd);
			return (INT_PTR)TRUE;
		}
		case WM_COMMAND:
			int id = LOWORD(wParam);
			//click Ctrl + number (from 1 to 10) to run
			for (int i = 0; i < 10; i++)
			{
				if (id == IDC_BUTTONAPP0 + i)
				{
					if (index_file[i] != -1)
					{
						dataFile.UpdateList(index_file[i]);
						dataFile.GetListNode().Excute(index_file[i]);
						EndDialog(dialog_hWnd, FALSE);
					}
				}
			}
			switch (id) {
			
			case WM_DESTROY:
				EndDialog(dialog_hWnd, FALSE);
				break;
			
			}
	}
	
	return (INT_PTR)FALSE;
}

void OpenSearchBox()
{
	if (!IsWindowVisible(dialog_hWnd))
		DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG), global_hWnd, Dialog_WndProc);
	else {
		SetWindowPos(global_hWnd, HWND_TOPMOST, 400, 400, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		SetForegroundWindow(dialog_hWnd);
		SetFocus(txtSearch);
		SetActiveWindow(dialog_hWnd);
	}
}


void OnCommand(HWND hWnd, int wmId, HWND hwndCtl, UINT codeNotify)
{
	// Parse the menu selections:
	switch (wmId)
	{
	case NOTI_STATICS:
		CreateChart();
		break;
	case NOTI_SCAN:
		NotiScanFile();
		break;
	case NOTI_EXIT:
		OnDestroy(hWnd);
	case IDM_ABOUT:
		DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
		break;
	
	default:
		return;
	}
}


void OnCreate(HWND hWnd)
{
	INITCOMMONCONTROLSEX icc;
	icc.dwSize = sizeof(icc);
	icc.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&icc);
	// Lấy font hệ thống
	LOGFONT lf;
	GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
	
	HFONT tFont = CreateFont(-25, 0,
		lf.lfEscapement, lf.lfOrientation, lf.lfWeight,
		lf.lfItalic, lf.lfUnderline, lf.lfStrikeOut, lf.lfCharSet,
		lf.lfOutPrecision, lf.lfClipPrecision, lf.lfQuality,
		lf.lfPitchAndFamily, lf.lfFaceName);
	
	for (int i = 0; i < 10; i++)
	{
		button_App[i] = CreateWindowEx(0, L"BUTTON", L"Run", WS_CHILD  | BS_TOP, 15 + i * 120, 60, 110, 60, dialog_hWnd, (HMENU)(110 + i), hInst, NULL);
		SendMessage(button_App[i], WM_SETFONT, WPARAM(hFont), TRUE);
		
	}
	//print number
	for (int i = 0; i < 10; i++)
	{
		wchar_t buffer[5];
		swprintf_s(buffer, 4, L"%d", (i+1) % 10);
		HWND number = CreateWindowEx(0, L"STATIC", buffer, WS_CHILD | WS_VISIBLE , 55 + i * 120, 125, 30, 30, dialog_hWnd, NULL, hInst, NULL);
		SendMessage(number, WM_SETFONT, WPARAM(tFont), TRUE);
	}
	SendMessage(txtSearch, WM_SETFONT, WPARAM(tFont), TRUE);

	SearchAppInList();
}




void SearchAppInList()
{
	
	WCHAR text_find[MAX_LOADSTRING];
	GetDlgItemText(dialog_hWnd, IDC_DIALOG_EDIT, text_find, MAX_LOADSTRING);
	
	dataFile.GetListNode().Find(wstring(text_find), index_file); // add index file vào list
	int count_button = 0;
	SHFILEINFO shfi;
	for(int i = 0; i < (int)index_file.size(); i++)
	{
		if (index_file[i] != -1)
		{
			SetWindowText(button_App[count_button], dataFile.GetListNode()[index_file[i]].GetName().c_str());
			//lấy icon file
			SHGetFileInfo(dataFile.GetListNode()[index_file[i]].GetPath().c_str(), FILE_ATTRIBUTE_NORMAL, &shfi, sizeof(SHFILEINFO),
				SHGFI_USEFILEATTRIBUTES | SHGFI_ICON | SHGFI_SHELLICONSIZE);
			SendMessage(button_App[count_button], BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)shfi.hIcon);

			ShowWindow(button_App[count_button], SW_SHOW);
			count_button++;
		}
		else {
			ShowWindow(button_App[i], SW_HIDE);
		}
	}

	
}

void ClickRun()
{
	if (!index_file.empty())
	{
		dataFile.UpdateList(index_file[0]);
		dataFile.GetListNode().Excute(index_file[0]);
		EndDialog(dialog_hWnd, FALSE);
	}
}



DWORD WINAPI ScanThread(LPVOID lpParameter)
{
	List_Nodes *temp_list_file = new List_Nodes();
	
	wstring buffer = *(wstring*)lpParameter;
	temp_list_file->AddPathScan(buffer);
	if (temp_list_file->Size() != 0)
	{
		EndDialog(wait_hWnd, FALSE);
		dataFile.AddListFile(*temp_list_file);
		
	}
	delete temp_list_file;
	
	return 0;
}

void NotiScanFile()
{
	EndDialog(dialog_hWnd, FALSE);
	//Create List Files
	string str_buff = BrowseFolder("");
	wstring wstr_buffer;
	wstr_buffer.assign(str_buff.begin(), str_buff.end());
	ScanFile(wstr_buffer);
	MessageBox(dialog_hWnd, L"Scan file done!!", L"", 0);
	OpenSearchBox();
	
}

void ScanFile(wstring &file_scan)
{
	folder_scanning = file_scan;
	HANDLE hMutex = CreateMutex(0, FALSE, NULL);

	//TODO: Tạo thread mới để scan -> stopable
	DWORD scanThreadID;

	scan_Handle = CreateThread(NULL, 0, ScanThread, &file_scan, 0, &scanThreadID);
	DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_WAIT), global_hWnd, Waiting);
	WaitForSingleObject(scan_Handle, INFINITE);

	if (scan_Handle != NULL)
		CloseHandle(scan_Handle);

	CloseHandle(hMutex);

}
HWND text;
INT_PTR CALLBACK Waiting(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	wait_hWnd = hDlg;
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		WCHAR buffer[255];
		wsprintf(buffer, L"Folder [%s] is scanning, Please wait a minutes,\n\tIf you want to stop scan, click [Stop]", folder_scanning.c_str());
		text = CreateWindowEx(0, L"STATIC", buffer, WS_CHILD | WS_VISIBLE, 30, 40, 400, 200, hDlg, NULL, hInst, NULL);
		SendMessage(text, WM_SETFONT, WPARAM(hFont), TRUE);
		
		SetWindowPos(hDlg, HWND_TOPMOST, 400, 400, 0, 0, SWP_NOSIZE);
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == ID_WAIT_STOP)
		{
			if (scan_Handle != NULL)
				TerminateThread(scan_Handle, 0);
				scan_Handle = NULL;
				EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

HWND statistics_hWnd;
void CreateChart()
{
	DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_STATICS), global_hWnd, Dialog_Statistics);
	
}

void DrawStatistics(int begin = 0)
{
	RECT client;
	GetClientRect(statistics_hWnd, &client);
	int width = client.right - client.left;
	int bottom = client.bottom - client.top - 130;

	Statistics statis;
	statis.Init(statistics_hWnd, dataFile.GetListNode(), begin, bottom, 80);

	PAINTSTRUCT ps;

	HDC hdc = BeginPaint(statistics_hWnd, &ps);
	Rectangle(hdc, 85, 30, 30 + 85, bottom + 10); // cột giá trị cao nhất
	WCHAR buffer[10];
	swprintf_s(buffer, 10, L"%d", dataFile.GetListNode()[0].GetPri());
	HWND text = CreateWindowEx(0, L"STATIC", buffer, WS_CHILD | BS_TOP | WS_VISIBLE, 95, 420, 20, 20, statistics_hWnd, NULL, hInst, NULL);

	statis.Draw(hdc);
	EndPaint(statistics_hWnd, &ps);
	
	SHFILEINFO shfi;
	for (int i = 0; i < 10; i++)
	{
		swprintf_s(buffer, 10, L"%d", dataFile.GetListNode()[i+ begin].GetPri());
		text = CreateWindowEx(0, L"STATIC", buffer, WS_CHILD | BS_TOP | WS_VISIBLE, 180 + i*90, 420, 20, 20, statistics_hWnd, NULL, hInst, NULL);

		button_App[i] = CreateWindowEx(0, L"BUTTON", dataFile.GetListNode()[i + begin].GetName().c_str(), WS_CHILD | BS_TOP | WS_VISIBLE, 140 + i * 90, bottom + 50, 80, 60, statistics_hWnd, (HMENU)(110 + i), hInst, NULL);
		SendMessage(button_App[i], WM_SETFONT, WPARAM(hFont), TRUE);
		//lấy icon file
		SHGetFileInfo(dataFile.GetListNode()[i + begin].GetPath().c_str(), FILE_ATTRIBUTE_NORMAL, &shfi, sizeof(SHFILEINFO),
			SHGFI_USEFILEATTRIBUTES | SHGFI_ICON | SHGFI_SHELLICONSIZE);
		SendMessage(button_App[i], BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)shfi.hIcon);

	}
}

INT_PTR CALLBACK Dialog_Statistics(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	
	statistics_hWnd = hDlg;
	
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	
	case WM_INITDIALOG:
		SetWindowPos(statistics_hWnd, HWND_TOPMOST, 400, 400, 0, 0, SWP_NOSIZE);
		return (INT_PTR)TRUE;
	case WM_PAINT:
		DrawStatistics(start);
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON_NEXT_STATICS:
			if (start < dataFile.GetListNode().Size() - 10)
				start += 10;
			InvalidateRect(hDlg, NULL, TRUE);
			break;
		case IDC_BUTTON_PREV:
			if (start >= 10)
				start -= 10;
			InvalidateRect(hDlg, NULL, TRUE);
			break;
		case WM_DESTROY:
			EndDialog(statistics_hWnd, FALSE);
			break;
		default:
			break;
		}
	}
	return (INT_PTR)FALSE;
}
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
		SetWindowPos(hDlg, HWND_TOPMOST, 400, 400, 0, 0, SWP_NOSIZE);
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


void OnDestroy(HWND hWnd)
{
	DeleteNotifyIcon(hWnd);
	doRemoveHook(hWnd);
	PostQuitMessage(0);
}


void OnPaint(HWND hWnd)
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hWnd, &ps);

	EndPaint(hWnd, &ps);
}
