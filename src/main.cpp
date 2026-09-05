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
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}

	std::cout << "Root directory: " << config.getRootDir() << std::endl;
	
	manager.scan(config.getRootDir());
	std::cout << "Scanning completed." << std::endl;
	for (const Note& note : manager.flattenAll()) {
		std::cout << note.path << std::endl;
	}
	//tui.run(manager);

	return 0;
}
