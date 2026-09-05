#include "NoteManager.h"
#include <filesystem>
#include <vector>
#include <string>
#include <memory>
#include <stdexcept>
#include <algorithm>

void NoteManager::scan(const std::string& rootDir) {
	_rootNode = std::make_unique<DirNode>(buildTree(rootDir));
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

	// sort notes
	std::vector<std::pair<std::string, Note*>> keyedNotes;
	for (Note& note : notes) {
		std::string lower = note.filename;
		std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
		keyedNotes.emplace_back(lower, &note);
	}
	std::sort(keyedNotes.begin(), keyedNotes.end(), [](const auto& a, const auto& b) {
		return a.first < b.first;
		});
	// Rebuild the notes vector in sorted order
	std::vector<Note> sortedNotes;
	for (std::pair<std::string, Note*>& pair : keyedNotes) {
		sortedNotes.push_back(std::move(*pair.second));
	}
	notes = std::move(sortedNotes);

	// sort subDirs
	std::sort(subDirs.begin(), subDirs.end(), [](const std::unique_ptr<DirNode>& a, const std::unique_ptr<DirNode>& b) {
		return a->dirPath < b->dirPath;
	});
	return DirNode(dirPath, std::move(notes), std::move(subDirs));
}

const DirNode& NoteManager::getRootNode() const {
	if (_rootNode == nullptr) {
		throw std::runtime_error("Root node is not initialized. Call scan() first.");
	}
	return *_rootNode;
}

std::vector<Note> NoteManager::flattenAll() {
	std::vector<Note> allNotes;
	flattenHelper(getRootNode(), allNotes);
	return allNotes;
}

void NoteManager::flattenHelper(const DirNode& node, std::vector<Note>& allNotes) {
	allNotes.insert(allNotes.end(), node.notes.begin(), node.notes.end());
	for (const auto& subDir : node.subDirs) {
		flattenHelper(*subDir, allNotes);
	}
}
