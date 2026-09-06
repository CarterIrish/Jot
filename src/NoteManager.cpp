#include "NoteManager.h"
#include <filesystem>
#include <vector>
#include <string>
#include <memory>
#include <stdexcept>
#include <algorithm>
#include <cctype>
#include <system_error>
#include <utility>
namespace {
	/**
	 * Checks whether a candidate directory is already on the current recursion path.
	 * Compares file identity rather than path text, so links and junctions that spell
	 * the same directory differently still match.
	 * @param candidate The directory to test.
	 * @param ancestors Paths of the directories currently being walked.
	 * @return True if candidate names the same directory as any ancestor.
	 */
	bool resolvesToAncestor(const std::filesystem::path& candidate, const std::vector<std::string>& ancestors) {
		for (const std::string& ancestor : ancestors) {
			std::error_code ec;
			if (std::filesystem::equivalent(candidate, ancestor, ec)) {
				return true;
			}
		}
		return false;
	}

	/**
	 * Creates a directory node that has no notes and no subdirectories.
	 * @param dirPath The path the node represents.
	 * @return An empty node for dirPath.
	 */
	std::unique_ptr<DirNode> makeEmptyNode(const std::string& dirPath) {
		return std::make_unique<DirNode>(dirPath, std::vector<Note>{}, std::vector<std::unique_ptr<DirNode>>{});
	}
}

/**
 * Scans the specified directory and builds a tree of its contents.
 * Clears any directories skipped by a previous scan.
 * @param rootDir The path to the directory to scan.
 */
void NoteManager::scan(const std::string& rootDir) {
	_skippedDirs.clear();
	std::vector<std::string> ancestors;
	_rootNode = std::make_unique<DirNode>(buildTree(rootDir, ancestors));
}

/**
 * Recursively builds a directory tree from the given directory path.
 * Subdirectories that resolve to an ancestor are added without being followed, and
 * subdirectories that cannot be read are recorded in the skipped list.
 * @param dirPath The path to the directory to build the tree from.
 * @param ancestors Paths of the directories currently being walked.
 * @return The root node of the built directory tree.
 */
DirNode NoteManager::buildTree(const std::string& dirPath, std::vector<std::string>& ancestors) {
	std::vector<Note> notes;
	std::vector<std::unique_ptr<DirNode>> subDirs;

	std::error_code ec;
	std::filesystem::directory_iterator dirIter(dirPath, ec);
	if (ec) {
		_skippedDirs.emplace_back(dirPath, ec.message());
		return DirNode(dirPath, std::move(notes), std::move(subDirs));
	}

	ancestors.push_back(dirPath);

	for (const auto& entry : dirIter) {
		if (entry.is_directory(ec)) {
			// Following a link back into our own path would recurse forever
			if (resolvesToAncestor(entry.path(), ancestors)) {
				subDirs.push_back(makeEmptyNode(entry.path().string()));
				continue;
			}
			subDirs.push_back(std::make_unique<DirNode>(buildTree(entry.path().string(), ancestors)));
		}
		else if (ec) {
			// Status is unreadable, so record the entry without guessing at its contents
			_skippedDirs.emplace_back(entry.path().string(), ec.message());
			subDirs.push_back(makeEmptyNode(entry.path().string()));
		}
		else {
			notes.push_back(Note(entry.path().filename().string(), entry.path().string()));
		}
	}

	ancestors.pop_back();

	// sort notes
	std::vector<std::pair<std::string, Note*>> keyedNotes;
	for (Note& note : notes) {
		std::string lower = note.filename;
		std::transform(lower.begin(), lower.end(), lower.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
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
		std::transform(lower.begin(), lower.end(), lower.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
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
 * Returns the directories that could not be read during the last scan.
 * @return The skipped directories, each paired with the reason it was skipped.
 */
const std::vector<SkippedDir>& NoteManager::getSkippedDirs() const {
	return _skippedDirs;
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
