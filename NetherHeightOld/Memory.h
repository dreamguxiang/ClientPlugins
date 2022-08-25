#pragma once
#include <windows.h>
#include <vector>
#include <iostream>
#define INRANGE(x,a,b)   (x >= a && x <= b)
#define GET_BYTE( x )    (GET_BITS(x[0]) << 4 | GET_BITS(x[1]))
#define GET_BITS( x )    (INRANGE((x&(~0x20)),'A','F') ? ((x&(~0x20)) - 'A' + 0xa) : (INRANGE(x,'0','9') ? x - '0' : 0))
namespace mem
{
	uintptr_t FindAddr(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets);
	DWORD GetProcId(const wchar_t* procName);
	uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* modName);
	void WriteEx(uintptr_t fovAddr, std::vector<uint16_t> bytes, HANDLE hProcess);
	std::string ptrToStr(uintptr_t ptr);
	uintptr_t FindSig(const char* szSignature, uintptr_t rangeStart, HANDLE CurrentProcess);
	
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