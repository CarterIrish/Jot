#include "Config.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <stdexcept>

#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN
	#define NOMINMAX
	#include <windows.h>
#endif

/**
 * Loads the configuration from the config file.
 */
void Config::load() {
	// Validate path to config file
	std::filesystem::path configPath = getConfigPath();
	if (!std::filesystem::exists(configPath)) {
		firstRunSetup();
		save();
		return;
	}
	// Open the file stream for reading
	std::ifstream configFile(configPath);
	if (!configFile.is_open()) {
		std::cerr << "Failed to open config file: " << configPath << std::endl;
		firstRunSetup();
		save();
		return;
	}

	// Parse config json & set values
	try {
		nlohmann::json configJson = nlohmann::json::parse(configFile);
		_rootDir = configJson.value("root_dir", "");
		_editor = configJson.value("editor", "notepad");
	}
	catch (const std::exception& e) {
		std::cerr << "Failed to parse config file " << configPath << ": " << e.what() << std::endl;
		firstRunSetup();
		save();
		return;
	}

	if (_rootDir.empty()) {
		std::cerr << "Root directory is not set in the config file. Running startup setup..." << std::endl;
		firstRunSetup();
		save();
	}
}


/**
 * Saves the configuration to the config file.
 */
void Config::save() const {
	std::filesystem::path configPath = getConfigPath();
	try {
		nlohmann::json j = {
			{"root_dir", _rootDir},
			{"editor", _editor}
		};
		// Open the file stream for writing
		std::ofstream configFile(configPath);
		if (!configFile.is_open()) {
			std::cerr << "Failed to open config file for write: " << configPath << std::endl;
			return;
		}
		configFile << j.dump(4);

		// Close explicitly so the flush result can be checked
		configFile.close();
		if (!configFile.good()) {
			std::cerr << "Failed to write config file: " << configPath << std::endl;
		}
	}
	catch (const std::exception& e) {
		std::cerr << "Failed to write json to config file at " << configPath << ": " << e.what() << std::endl;
	}
}

/**
 * Gets the root directory.
 * @return The root directory.
 */
std::string Config::getRootDir() const {
    return _rootDir;
}

/**
 * Gets the path to the configuration file, which lives beside the executable.
 * @return The path to the configuration file.
 */
std::filesystem::path Config::getConfigPath() const {
#ifdef _WIN32
	wchar_t buffer[MAX_PATH]{};
	if (GetModuleFileNameW(nullptr, buffer, MAX_PATH) != 0) {
		return std::filesystem::path(buffer).parent_path() / "config.json";
	}
#else
	std::error_code ec;
	std::filesystem::path exe = std::filesystem::read_symlink("/proc/self/exe", ec);
	if (!ec) return exe.parent_path() / "config.json";
#endif
	// Could not locate the executable; fall back to the working directory
	return std::filesystem::current_path() / "config.json";
}

/**
 * Prompts for the notes directory, repeating until an existing one is given.
 * @throws std::runtime_error If the user quits or the input stream closes.
 */
void Config::firstRunSetup() {
	std::cout << "Welcome to Jot!\n";

	while (true) {
		std::cout << "Enter the full path to your notes directory (or press Enter alone to quit): ";

		// A closed stream never recovers, so it has to end the loop rather than retry
		if (!std::getline(std::cin, _rootDir)) {
			_rootDir.clear();
			throw std::runtime_error("jot requires a usable notes directory. No input was received.");
		}

		// Trim surrounding whitespace; an empty line means the user quit
		const std::size_t first = _rootDir.find_first_not_of(" \t");
		if (first == std::string::npos) {
			_rootDir.clear();
			throw std::runtime_error("jot requires a usable notes directory. Setup cancelled.");
		}
		_rootDir = _rootDir.substr(first, _rootDir.find_last_not_of(" \t") - first + 1);

		// Strip the quotes a pasted or dragged path often carries
		if (_rootDir.size() >= 2 && _rootDir.front() == '"' && _rootDir.back() == '"') {
			_rootDir = _rootDir.substr(1, _rootDir.size() - 2);
		}

		std::error_code ec;
		if (std::filesystem::is_directory(_rootDir, ec)) return;
		std::cerr << "\"" << _rootDir << "\" is not an existing directory. Please try again.\n";
	}
}
