#include "Config.h"
#include "NoteManager.h"
#include "TUI.h"
#include <exception>
#include <iostream>
#include <string>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#endif


int main() {
	#ifdef _WIN32
		SetConsoleOutputCP(CP_UTF8);
	#endif
	Config config;
	NoteManager manager;


	try {
		// Setup the run environment
		config.load();
		manager.scan(config.getRootDir()); // Create the note tree
		std::cout << "Scanning completed.\n";
		for (const SkippedDir& skipped : manager.getSkippedDirs()) {
			std::cerr << "  [WARNING] Skipped \"" << skipped.dirPath.u8string() << "\" (" << skipped.reason << ")\n";
		}
		// Launch and run the TUI
		TUI tui(manager);
		tui.run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << "\n";
		return 1;
	}
	return 0;
}
