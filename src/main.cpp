#include "Config.h"
#include "NoteManager.h"
#include "TUI.h"
#include <iostream>

int main() {
    Config config;
    config.load();

	std::cout << "Notes root directory: " << config.getRootDir() << std::endl;

	std::cin.get(); // Wait for user input before proceeding
    return 0;
}
