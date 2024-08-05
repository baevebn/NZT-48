#include "main.h"

void InjectDLL()
{
	char EXE[MAX_PATH];
	sprintf(EXE, "%s.exe", ExtHack(pszDirectory("").c_str(), ".dll").c_str());

	DWORD pID = FindProcessByName(EXE);
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pID);

	if (pID != NULL)
	{
		TCHAR szExePath[MAX_PATH];
		DWORD cbNeeded = MAX_PATH;

		if (GetProcessImageFileName(hProcess, szExePath, cbNeeded))
		{
			SetConsoleTitle(szExePath);
		}

		char DLL[MAX_PATH];
		sprintf(DLL, "%s", ExtHack(pszDirectory("").c_str(), ".dll").c_str());
		DLLInjector(pID, pszDirectory(DLL).c_str());
	}

	CloseHandle(hProcess);
}

int main()
{
	setlocale(LC_ALL, "Russian");

	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	if (strstr(HackExeName().c_str(), "NZT-48.exe"))
	{
		InjectDLL();
	}
	else
	{
		rename(EXE().c_str(), "NZT-48.exe");
	}

	return 0;
}

