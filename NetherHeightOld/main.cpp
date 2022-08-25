#include <iostream>
#include "types.h"
#include "Memory.h"

int main() {
start:
    int startChoice = -1;
    DWORD procId = mem::GetProcId(L"Minecraft.Windows.exe");
    if (procId == 0) {
        std::cout << "No use finding Minecraft.Windows!" << std::endl;
        Sleep(1000);
        system("cls");
        goto start;
    }
	
    uintptr_t moduleBase = mem::GetModuleBaseAddress(procId, L"Minecraft.Windows.exe");
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, procId);
	
    std::cout << "[1] Nether Height\n[2] Exit\nChoice => ";
    std::cin >> startChoice;
    system("cls");
    switch (startChoice) {
    case 1: {
        NetherHeight(moduleBase, hProcess);
        Sleep(2000);
        system("cls");
        break;
    }
    case 2:
        std::cout << "Exit!\n";
        Sleep(1000);
        system("cls");
        return 0;
    default:
        if (startChoice != -1) {
            std::cout << "\nInvalid Choice\n";
            std::cout << "Back to Start...\n";
            Sleep(1000);
            system("cls");
            goto start;
        }
        break;
    }

    if (startChoice == -1) {
        return 0;
    }

    Sleep(800);
    system("cls");
    std::cout << "Back to Start...\n";
    Sleep(800);
    system("cls");
    goto start;
}