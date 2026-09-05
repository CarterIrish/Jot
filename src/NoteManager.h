#pragma once
#include <string>
#include <vector>
#include <memory>

struct Note {
    std::string filename;
    std::string path;

	Note(std::string filename, std::string path) : filename(filename), path(path) {}
};

struct DirNode {
    std::string name;
    std::vector<Note> notes;
    std::vector<std::unique_ptr<DirNode>> subDirs;

    DirNode(std::string name, std::vector<Note> notes, std::vector<std::unique_ptr<DirNode>> subDirs)
        : name(std::move(name)), notes(std::move(notes)), subDirs(std::move(subDirs)) {}
};

class NoteManager {
public:
    void scan(const std::string& rootDir);
    const std::vector<Note>& getNotes() const;
    void viewNote(const Note& note);
    void editNote(const Note& note);
    std::vector<Note> flattenAll(const std::string& rootDir);
	DirNode buildTree(const std::string& dirPath);

private:
    std::vector<Note> notes;
};
