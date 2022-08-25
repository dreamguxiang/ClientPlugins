#include <iostream>
#include "Memory.h"
#include <xorstr.hpp>

void NetherHeight(uintptr_t moduleBase, HANDLE hProcess)
{
    std::cout << "修改中，请稍后！" << std::endl;
    int currFov;
    auto fovAddr = mem::FindSig(xorstr_("C7 44 ?? ?? ?? ?? ?? ?? 49 8D ?? ?? 49 89 ?? ?? 4D 89 ?? ?? 44 8B ?? ?? ?? 44 ?? ?? ?? ?? ?? ?? E8 ?? ?? ?? ?? 90 48 ?? ?? ?? ?? ?? ??"), moduleBase, hProcess);
    fovAddr = fovAddr + 6;
    std::cout <<"地狱最大高度修改前："<< mem::Read(fovAddr, currFov, 2, hProcess) << std::endl;
    mem::WriteEx(fovAddr, {0x00,0x01 }, hProcess);
    std::cout <<"地狱最大高度修改后：" << mem::Read(fovAddr, currFov,2,  hProcess) << std::endl;
}
