# ClientPlugins ~~图一乐-~~
这是一些针对客户端的小插件,可直接注入或者利用ClientTools工具实现加载

## 功能
1. 修改客户端地狱最大高度
2. 暂定

## ~可能对你有用~
MaterialBinLoader目录中含有`HookAPI.h`,提供了基本的范围查找与HOOK等功能 

你需要在你的项目中包含include文件夹与lib文件夹所有内容（暂时还没设计好）

然后在初始化时写如下代码   
```c++
#include <Windows.h>
#include "HookAPI.h"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        Utils::hModuleName = "Minecraft.Windows.exe";
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
```
其中`Utils::hModuleName`可自定义程序名

