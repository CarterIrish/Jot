#pragma once
#include <string>
#include <filesystem>

class Config {
public:
    void load();
    void save() const;
    std::string getRootDir() const;

private:
	std::filesystem::path getConfigPath() const;
	void firstRunSetup();

    std::string _rootDir;
    std::string _editor = "notepad";
};
