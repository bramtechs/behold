#include "platform.hpp"

void OpenDebugConsole() {
    std::cout << "Not using Windows, can't allocate console!" << std::endl;
}

void CloseDebugConsole() {
}
