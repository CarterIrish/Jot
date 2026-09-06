#include "Config.h"
#include "NoteManager.h"
#include "TUI.h"
#include <iostream>
#include <exception>


int main() {
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
		std::cout << note.path << "\n";
	}
	std::cout << "Press Enter to exit demo...";
	std::getline(std::cin, std::string());
	//tui.run(manager);

	return 0;
}
