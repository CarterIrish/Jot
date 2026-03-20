#include "NoteManager.h"

void NoteManager::scan(const std::string& rootDir) {}

const std::vector<Note>& NoteManager::getNotes() const {
    return notes;
}

void NoteManager::viewNote(const Note& note) {}

void NoteManager::editNote(const Note& note) {}
