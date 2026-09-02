#include "Config.h"
#include "NoteManager.h"
#include "TUI.h"
#include <iostream>

int main() {
    Config config;
    NoteManager manager;
    TUI tui;

    try {
        config.load();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

	std::cout << "Root directory: " << config.getRootDir() << std::endl;
    //manager.scan(config.getRootDir());
    //tui.run(manager);

    return 0;
}
