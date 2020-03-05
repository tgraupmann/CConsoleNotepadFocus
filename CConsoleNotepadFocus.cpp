#include <cstdio>
#include <windows.h>
#include <tlhelp32.h>
#include <string>
#include <tchar.h>
#include <vector>

using namespace std;

void GetAllWindowsFromProcessID(DWORD dwProcessID)
{
	wprintf(L"Found windows for process=%d\r\n", dwProcessID);

	HWND hCurWnd = NULL;
	vector<HWND> visited;
	do
	{
		hCurWnd = FindWindowEx(NULL, hCurWnd, NULL, NULL);

		DWORD checkProcessID = 0;
		GetWindowThreadProcessId(hCurWnd, &checkProcessID);
		if (dwProcessID == checkProcessID)
		{
			int len = GetWindowTextLength(hCurWnd) + 1;
			vector<wchar_t> buf(len);
			GetWindowText(hCurWnd, &buf[0], len);
			wstring stxt = &buf[0];

			if (stxt.compare(L"Untitled - Notepad") != 0)
			{
				continue;
			}

			wprintf(L"Found process=%d hWnd %s\n", dwProcessID, stxt.c_str());

			if (std::find(visited.begin(), visited.end(), hCurWnd) != visited.end())
			{
				continue;
			}
			else
			{
				visited.push_back(hCurWnd);
			}

			if (IsIconic(hCurWnd))
			{
				wprintf(L"Window is minimized!\r\n");
				SendMessage(hCurWnd, WM_SYSCOMMAND, SC_RESTORE, 0); //show Window if minimized
				::SetForegroundWindow(hCurWnd);
				::SetFocus(hCurWnd);
				::SetActiveWindow(hCurWnd);
			}
			else if (IsWindowVisible(hCurWnd))
			{
				wprintf(L"Window is visible!\r\n");
				::SetForegroundWindow(hCurWnd);
				::SetFocus(hCurWnd);
				::SetActiveWindow(hCurWnd);
			}
			else
			{
				//window is closed
				wprintf(L"Window is not visible!\r\n");
			}
		}
	} while (hCurWnd != NULL);
}

int main()
{
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (Process32First(snapshot, &entry) == TRUE)
	{
		while (Process32Next(snapshot, &entry) == TRUE)
		{
			wstring processName = entry.szExeFile;
			if (processName.compare(L"notepad.exe") == 0)
			{
				GetAllWindowsFromProcessID(entry.th32ProcessID);
			}
		}
	}

	CloseHandle(snapshot);
}
