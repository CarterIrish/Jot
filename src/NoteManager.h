#pragma once
#include <string>
#include <vector>
#include <memory>
#include <utility>

struct Note {
	std::string filename;
	std::string path;

	Note(std::string filename, std::string path) : filename(filename), path(path) {}
};

struct DirNode {
	std::string dirPath;
	std::vector<Note> notes;
	std::vector<std::unique_ptr<DirNode>> subDirs;

	DirNode(std::string name, std::vector<Note> notes, std::vector<std::unique_ptr<DirNode>> subDirs)
		: dirPath(std::move(name)), notes(std::move(notes)), subDirs(std::move(subDirs)) {}
};

struct SkippedDir {
	std::string path;
	std::string reason;

	SkippedDir(std::string path, std::string reason)
		: path(std::move(path)), reason(std::move(reason)) {}
};

class NoteManager {
public:
	void scan(const std::string& rootDir);
	std::vector<Note> flattenAll();
	const DirNode& getRootNode() const;
	const std::vector<SkippedDir>& getSkippedDirs() const;

private:
	std::unique_ptr<DirNode> _rootNode;
	std::vector<SkippedDir> _skippedDirs;
	DirNode buildTree(const std::string& dirPath, std::vector<std::string>& ancestors);
	void flattenHelper(const DirNode& node, std::vector<Note>& allNotes);
};
