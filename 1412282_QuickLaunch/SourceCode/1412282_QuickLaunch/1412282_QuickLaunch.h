#pragma once

#include "resource.h"



void OnCreate(HWND hWnd);
void OnCommand(HWND hWnd, int id, HWND hwndCtl, UINT codeNotify);
void OnPaint(HWND hWnd);
void OnDestroy(HWND hWnd);
void ClickRun();
void SearchAppInList();
void OpenSearchBox();
void ScanFile(std::wstring &file_scan);
void NotiScanFile();
void CreateChart();

INT_PTR CALLBACK Dialog_WndProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK Waiting(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK Dialog_Statistics(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);