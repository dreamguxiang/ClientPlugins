#pragma once
#include <windows.h>
#include <vector>
#include <iostream>

namespace mem
{
	uintptr_t FindAddr(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets);
	DWORD GetProcId(const wchar_t* procName);
	uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* modName);
	void WriteEx(uintptr_t fovAddr, std::vector<uint16_t> bytes, HANDLE hProcess);
	std::string ptrToStr(uintptr_t ptr);
	uintptr_t FindSig(const char* szSignature);
	
	template <typename T>
	T& Read(uintptr_t fovAddr, T& currFov,int size, HANDLE hProcess)
	{
		DWORD oldprotect;
		VirtualProtect((BYTE*)fovAddr, size, PAGE_EXECUTE_READWRITE, &oldprotect);
		ReadProcessMemory(hProcess, (BYTE*)fovAddr, &currFov, size, nullptr);
		VirtualProtect((BYTE*)fovAddr, size, oldprotect, &oldprotect);
		return currFov;
	}
	
	template <typename T>
	void Write(uintptr_t fovAddr, T& newFov, int size, HANDLE hProcess)
	{
		DWORD oldprotect;
		VirtualProtect((BYTE*)fovAddr, size, PAGE_EXECUTE_READWRITE, &oldprotect);
		WriteProcessMemory(hProcess, (BYTE*)fovAddr, &newFov, size, nullptr);
		VirtualProtect((BYTE*)fovAddr, size, oldprotect, &oldprotect);
	}

}