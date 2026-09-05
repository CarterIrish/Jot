#include "NoteManager.h"
#include <filesystem>



void NoteManager::scan(const std::string& rootDir) {
}

DirNode NoteManager::buildTree(const std::string& dirPath) {
    std::vector<Note> notes;
    std::vector<std::unique_ptr<DirNode>> subDirs;
    std::filesystem::directory_iterator dirIter(dirPath);

    for (const auto& entry : dirIter) {
        if (entry.is_directory()) {
			// Recursively build the tree for the subdirectory
			subDirs.push_back(std::make_unique<DirNode>(buildTree(entry.path().string())));
        }
        else {
            // Base case: create a Note object for the file, push to vector
            notes.push_back(Note(entry.path().filename().string(), entry.path().string()));
        }
    }
    return DirNode(dirPath, std::move(notes), std::move(subDirs));
}

const std::vector<Note>& NoteManager::getNotes() const {
    return notes;
}

void NoteManager::viewNote(const Note& note) {}

void NoteManager::editNote(const Note& note) {}
