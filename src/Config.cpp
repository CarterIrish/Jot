#include "Config.h"

void Config::load() {}

void Config::save() const {}

std::string Config::getRootDir() const {
    return _rootDir;
}

std::filesystem::path Config::getConfigPath() const {
    return {};
}

void Config::firstRunSetup() {}
