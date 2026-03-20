#pragma once
#include <string>
#include <vector>

struct Note {
    std::string filename;
    std::string path;
};

class NoteManager {
public:
    void scan(const std::string& rootDir);
    const std::vector<Note>& getNotes() const;
    void viewNote(const Note& note);
    void editNote(const Note& note);

private:
    std::vector<Note> notes;
};
