#include "Config.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#endif


std::filesystem::path Config::getConfigPath() const
{
#ifdef _WIN32
    char buf[MAX_PATH];
    GetModuleFileNameA(nullptr, buf, MAX_PATH);
    return std::filesystem::path(buf).parent_path() / "config.json";
#else
    return std::filesystem::current_path() / "config.json";
#endif
}

void Config::firstRunSetup()
{
    std::cout << "Welcome to Jot!\n";
    std::cout << "Enter the full path to your notes directory: ";
	std::getline(std::cin, _rootDir);
    if(_rootDir.size() >= 2 && _rootDir.front() == '"' && _rootDir.back() == '"') {
        _rootDir = _rootDir.substr(1, _rootDir.size() - 2);
	}
}

void Config::load() {
	std::filesystem::path path = getConfigPath();
    if (!std::filesystem::exists(path))
    {
        firstRunSetup();
        save();
        return;
    }

    std::ifstream file(path);
    if (!file.is_open())
    {
        std::cerr << "Failed to open config file for reading: " << path << std::endl;
        return;
    }

    try
    {
        nlohmann::json j = nlohmann::json::parse(file);
		_rootDir = j.value("root_dir", "");
        _editor = j.value("editor", "notepad");
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error parsing config file: " << e.what() << "\nRe-running first-time setup.\n";
		firstRunSetup();
		save();
        return;
    }

    if(_rootDir.empty()) {
        std::cerr << "root_dir is empty, re-running first-time setup.\n";
        firstRunSetup();
        save();
	}
}

void Config::save() const
{
    nlohmann::json j;
    try
    {
        j["root_dir"] = _rootDir;
        j["editor"] = _editor;
		std::ofstream file(getConfigPath());
        file << j.dump(4);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error saving config file: " << e.what() << std::endl;
    }
}

std::string Config::getRootDir() const {
    return _rootDir;
}


