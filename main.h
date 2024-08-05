#pragma once
#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <algorithm>
#include <Psapi.h>
#include <cstdlib>
#include <tlhelp32.h>
#include <Shlwapi.h>
#include <locale>
#include <filesystem>
#include <vector>
#include <stdarg.h>
#include <cstdarg>

#pragma comment(lib,"Shlwapi.lib")
#pragma comment(lib,"wbemuuid.lib")

using namespace std;
using namespace filesystem;
namespace fs = filesystem;

inline string GetPathToHard()
{
	char buffer[MAX_PATH];
	GetSystemDirectory(buffer, sizeof(buffer));

	string windowsDir(buffer);
	size_t pos = windowsDir.find('\\', 3);

	string diskPath = windowsDir.substr(0, pos - 7);

	char cmd[256];
	sprintf(cmd, "%s", diskPath.c_str());

	return cmd;
}

inline string HackExeName()
{
	const size_t len = 260;
	LPSTR buffer = new TCHAR[len];

	GetModuleFileName(NULL, buffer, len);
	char* szHackName = PathFindFileName(buffer);

	return szHackName;
}

inline string EXE()
{
	const size_t len = MAX_PATH;
	LPSTR buffer = new TCHAR[len];

	GetModuleFileName(GetModuleHandle(NULL), buffer, len);
	return buffer;
}

inline string pszDirectory(const char* pszName)
{
	TCHAR buffer[MAX_PATH];
	GetCurrentDirectory(sizeof(buffer), buffer);

	char str[MAX_PATH];
	sprintf(str, "%s\\", buffer);

	string pDir = str;
	return (pDir + pszName);
}

inline DWORD FindProcessByName(const char* procname)
{
	HANDLE hSnapshot;
	PROCESSENTRY32 pe;

	int pid = 0;
	BOOL hResult;

	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (INVALID_HANDLE_VALUE == hSnapshot)
	{
		return 0;
	}

	pe.dwSize = sizeof(PROCESSENTRY32);
	hResult = Process32First(hSnapshot, &pe);

	while (hResult)
	{
		if (strcmp(procname, pe.szExeFile) == 0)
		{
			pid = pe.th32ProcessID;
			break;
		}

		hResult = Process32Next(hSnapshot, &pe);
	}

	CloseHandle(hSnapshot);

	return pid;
}

inline int DLLInjector(DWORD pID, const char* dllPath)
{
	LPVOID LoadLibAddr;
	LPVOID BaseAddr;
	HANDLE remThread;

	int dllLength = strlen(dllPath) + 1;

	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, 0, pID);

	if (hProc == INVALID_HANDLE_VALUE)
	{
		return -1;
	}

	LoadLibAddr = (LPVOID)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");

	if (!LoadLibAddr)
	{
		return -1;
	}

	BaseAddr = VirtualAllocEx(hProc, NULL, dllLength, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

	if (!BaseAddr)
	{
		return -1;
	}

	if (!WriteProcessMemory(hProc, BaseAddr, dllPath, dllLength, NULL))
	{
		return -1;
	}

	remThread = CreateRemoteThread(hProc, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibAddr, BaseAddr, 0, NULL);

	if (!remThread)
	{
		return -1;
	}

	WaitForSingleObject(remThread, INFINITE);

	VirtualFreeEx(hProc, BaseAddr, dllLength, MEM_RELEASE);

	if (CloseHandle(remThread) == 0)
	{
		return -1;
	}

	if (CloseHandle(hProc) == 0)
	{
		return -1;
	}

	return 0;
}

inline string ExtHack(const char* pszPath, const char* pszName)
{
	string pszStr;

	for (const auto& entry : directory_iterator(pszPath))
	{
		if (is_regular_file(entry))
		{
			string fileName = entry.path().filename().string();

			if (fileName.find(pszName) != string::npos)
			{
				pszStr = fileName.substr(0, fileName.find_last_of(".")).c_str();
			}
		}
	}

	return pszStr;
}
