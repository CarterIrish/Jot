#include "NoteManager.h"
#include <filesystem>
#include <vector>
#include <string>
#include <memory>
#include <stdexcept>
#include <algorithm>

/**
 * Scans the specified directory and builds a tree of its contents.
 * @param rootDir The path to the directory to scan.
 */
void NoteManager::scan(const std::string& rootDir) {
	_rootNode = std::make_unique<DirNode>(buildTree(rootDir));
}

/**
 * Recursively builds a directory tree from the given directory path.
 * @param dirPath The path to the directory to build the tree from.
 * @return The root node of the built directory tree.
 */
DirNode NoteManager::buildTree(const std::string& dirPath) {
	// intialize notes and subDirs vectors
	std::vector<Note> notes;
	std::vector<std::unique_ptr<DirNode>> subDirs;
	std::filesystem::directory_iterator dirIter(dirPath);

	// Recursively iterate through the directory and its subdirectories
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
		keyedNotes.emplace_back(std::move(lower), &note);
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
	std::vector<std::pair<std::string, std::unique_ptr<DirNode>>> keyedSubDirs;
	for (std::unique_ptr<DirNode>& subDir : subDirs) {
		std::string lower = subDir->dirPath;
		std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
		keyedSubDirs.emplace_back(std::move(lower), std::move(subDir));
	}
	std::sort(keyedSubDirs.begin(), keyedSubDirs.end(), [](const auto& a, const auto& b) {
		return a.first < b.first;
		});
	// rebuild subDirs vector in sorted order
	std::vector<std::unique_ptr<DirNode>> sortedSubDirs;
	for (std::pair<std::string, std::unique_ptr<DirNode>>& pair : keyedSubDirs) {
		sortedSubDirs.push_back(std::move(pair.second));
	}
	subDirs = std::move(sortedSubDirs);

	// Return the constructed DirNode
	return DirNode(dirPath, std::move(notes), std::move(subDirs));
}

/**
 * Returns the root directory node.
 * @return The root directory node.
 */
const DirNode& NoteManager::getRootNode() const {
	if (_rootNode == nullptr) {
		throw std::runtime_error("Root node is not initialized. Call scan() first.");
	}
	return *_rootNode;
}

/**
 * Flattens all notes from the directory tree.
 * @return A vector containing all notes from the directory tree.
 */
std::vector<Note> NoteManager::flattenAll() {
	std::vector<Note> allNotes;
	flattenHelper(getRootNode(), allNotes);
	return allNotes;
}

/**
 * Recursively flattens the directory tree into a vector of notes.
 * @param node The current directory node.
 * @param allNotes The vector to store the flattened notes.
 */
void NoteManager::flattenHelper(const DirNode& node, std::vector<Note>& allNotes) {
	allNotes.insert(allNotes.end(), node.notes.begin(), node.notes.end());
	for (const auto& subDir : node.subDirs) {
		flattenHelper(*subDir, allNotes);
	}
}
