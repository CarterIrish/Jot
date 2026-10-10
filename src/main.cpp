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
		// Launch the TUI
		TUI tui(manager);
		tui.run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << "\n";
		return 1;
	}

#pragma region DebugOutput
	//// Display the results of scan 
	//std::cout << "Scanning completed.\n";
	//for (const SkippedDir& skipped : manager.getSkippedDirs()) {
	//	std::cerr << "  warning: skipped \"" << skipped.dirPath.u8string() << "\" (" << skipped.reason << ")\n";
	//}
	//for (const Note& note : manager.flattenAll()) {
	//	std::cout << note.filePath.u8string() << "\n";
	//}
#pragma endregion
	return 0;
}
