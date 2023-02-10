#include "platform.hpp"

#include <iostream>
#include <windows.h>

void OpenDebugConsole() {
    if (AllocConsole()) {
        freopen("CONIN$", "r", stdin);
        freopen("CONOUT$", "w", stdout);
        freopen("CONOUT$", "w", stderr);
    }
    std::cout << "Allocated Windows console" << std::endl;
}

void CloseDebugConsole() {
    FreeConsole();
}
