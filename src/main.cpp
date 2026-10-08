#include "Config.h"
#include "NoteManager.h"
#include "TUI.h"
#include <iostream>
#include <exception>
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
	TUI tui;

	try {
		config.load();
		std::cout << "Root directory: " << config.getRootDir() << "\n";
		manager.scan(config.getRootDir());
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << "\n";
		return 1;
	}

	std::cout << "Scanning completed.\n";
	for (const SkippedDir& skipped : manager.getSkippedDirs()) {
		std::cerr << "  warning: skipped \"" << skipped.path << "\" (" << skipped.reason << ")\n";
	}
	for (const Note& note : manager.flattenAll()) {
		std::cout << note.filePath.u8string() << "\n";
	}
	std::cout << "Press Enter to exit demo...";
	std::string unused;
	std::getline(std::cin, unused);
	//tui.run(manager);

	return 0;
}
