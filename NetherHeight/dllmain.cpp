// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include "Memory.h"
#include "../include/xorstr.hpp"
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH: {
        static const uintptr_t rangeStart = (uintptr_t)GetModuleHandleA("Minecraft.Windows.exe");
        //auto fovAddr2 = mem::FindSig(xorstr_("48 ?? ?? ?? 48 8B ?? ?? 48 8B 01 FF ?? ?? ?? ?? ?? 48 8B C8 48 ?? ?? ?? E9 ?? ?? ?? ??"));
       // std::cout << fovAddr2 << std::endl;
        //std::cout << fovAddr2 - (uintptr_t)GetModuleHandleA("Minecraft.Windows.exe") << std::endl;
		
        auto fovAddr = mem::FindSig(xorstr_("C7 44 ?? ?? ?? ?? ?? ?? 49 8D ?? ?? 49 89 ?? ?? 4D 89 ?? ?? 44 8B ?? ?? ?? 44 ?? ?? ?? ?? ?? ?? E8 ?? ?? ?? ?? 90 48 ?? ?? ?? ?? ?? ?? 48 89 03 48 ?? ?? ?? "));
        fovAddr = fovAddr + 6;
        int currFov;
        std::cout << "地狱最大高度修改前：" << mem::Read(fovAddr, currFov, 2, GetCurrentProcess()) << std::endl;
        mem::WriteEx(fovAddr, { 0x00,0x01 }, GetCurrentProcess());
        std::cout << "地狱最大高度修改后：" << mem::Read(fovAddr, currFov, 2, GetCurrentProcess()) << std::endl;
    }
    case DLL_THREAD_ATTACH: 
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

