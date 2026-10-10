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
	//TUI tui;

	try {
		// Setup the run environment
		config.load();
		std::cout << "Root directory: " << config.getRootDir().u8string() << "\n";
		manager.scan(config.getRootDir()); // Create the note tree
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << "\n";
		return 1;
	}

#pragma region DebugOutput
	// Display the results of scan 
	std::cout << "Scanning completed.\n";
	for (const SkippedDir& skipped : manager.getSkippedDirs()) {
		std::cerr << "  warning: skipped \"" << skipped.dirPath.u8string() << "\" (" << skipped.reason << ")\n";
	}
	for (const Note& note : manager.flattenAll()) {
		std::cout << note.filePath.u8string() << "\n";
	}
#pragma endregion

	std::cout << "Press Enter to exit demo...";
	std::string unused;
	std::getline(std::cin, unused);
	return 0;
}
