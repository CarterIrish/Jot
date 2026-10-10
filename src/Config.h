#pragma once
#include <filesystem>
#include <string>

class Config {
public:
    void load();
    void save() const;
    std::filesystem::path getRootDir() const;

private:
	std::filesystem::path getConfigPath() const;
	void firstRunSetup();

    std::string _rootDir;
    std::string _editor = "notepad";
};
