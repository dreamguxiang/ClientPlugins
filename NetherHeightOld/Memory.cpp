#include "Memory.h"
#include <TlHelp32.h>
#include <Windows.h>
#include <string>
#include <cstdarg>
#include <fileapi.h>
#include <Psapi.h>
#include <iostream>
#include <vector>
#include <xinput.h>
#include <sstream>
#include <map>
#include <chrono>

void mem::WriteEx(uintptr_t fovAddr, std::vector<uint16_t> bytes, HANDLE hProcess)
{
	DWORD oldprotect;
	for (int i = 0; i < bytes.size(); ++i) {
		VirtualProtect(hProcess, 1, PAGE_EXECUTE_READWRITE, &oldprotect);
		WriteProcessMemory(hProcess, (BYTE*)fovAddr, &bytes[i], 1, nullptr);
		VirtualProtect(hProcess, 1, oldprotect, &oldprotect);
		fovAddr++;
	}
}


uintptr_t mem::FindAddr(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets)
{
	uintptr_t addr = ptr;
	for (unsigned int i = 0; i < offsets.size(); ++i)
	{
		ReadProcessMemory(hProc, (BYTE*)addr, &addr, sizeof(addr), 0);
		addr += offsets[i];
	}
	return addr;
}


DWORD mem::GetProcId(const wchar_t* procName)
{
	DWORD procId = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnap != INVALID_HANDLE_VALUE)
	{
		PROCESSENTRY32 procEntry;
		procEntry.dwSize = sizeof(procEntry);

		if (Process32First(hSnap, &procEntry))
		{
			do
			{
				if (!_wcsicmp(procEntry.szExeFile, procName))
				{
					procId = procEntry.th32ProcessID;
					break;
				}
			} while (Process32Next(hSnap, &procEntry));

		}
	}
	CloseHandle(hSnap);
	return procId;
}



uintptr_t mem::GetModuleBaseAddress(DWORD procId, const wchar_t* modName)
{
	uintptr_t modBaseAddr = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);
	if (hSnap != INVALID_HANDLE_VALUE)
	{
		MODULEENTRY32 modEntry;
		modEntry.dwSize = sizeof(modEntry);
		if (Module32First(hSnap, &modEntry))
		{
			do
			{
				if (!_wcsicmp(modEntry.szModule, modName))
				{
					modBaseAddr = (uintptr_t)modEntry.modBaseAddr;
					break;
				}
			} while (Module32Next(hSnap, &modEntry));
		}
	}
	CloseHandle(hSnap);
	return modBaseAddr;
}

uintptr_t mem::FindSig(const char* szSignature, uintptr_t rangeStart, HANDLE CurrentProcess) {
		const char* pattern = szSignature;
		uintptr_t firstMatch = 0;
		MODULEINFO miModInfo;

		GetModuleInformation(CurrentProcess, (HMODULE)rangeStart, &miModInfo, sizeof(MODULEINFO));

		const uintptr_t rangeEnd = rangeStart + miModInfo.SizeOfImage;

		BYTE patByte = GET_BYTE(pattern);
		const char* oldPat = pattern;

		for (uintptr_t pCur = rangeStart; pCur < rangeEnd; pCur++) {
			if (!*pattern)
				return firstMatch;

			while (*(PBYTE)pattern == ' ')
				pattern++;

			if (!*pattern)
				return firstMatch;

			if (oldPat != pattern) {
				oldPat = pattern;
				if (*(PBYTE)pattern != '\?')
					patByte = GET_BYTE(pattern);
			};
			BYTE temp;
			if (*(PBYTE)pattern == '\?' || Read(pCur, temp,1, CurrentProcess) == patByte) {
				if (!firstMatch)
					firstMatch = pCur;

				if (!pattern[2])
					return firstMatch;

				pattern += 2;
			}
			else {
				pattern = szSignature;
				firstMatch = 0;
			};
		};

		return NULL;

	};

std::string mem::ptrToStr(uintptr_t ptr) {
	std::ostringstream ss;
	ss << std::hex << ((UINT64)ptr) << std::endl;
	return ss.str();
}