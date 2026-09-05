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
	std::string dirPath;
	std::vector<Note> notes;
	std::vector<std::unique_ptr<DirNode>> subDirs;

	DirNode(std::string name, std::vector<Note> notes, std::vector<std::unique_ptr<DirNode>> subDirs)
		: dirPath(std::move(name)), notes(std::move(notes)), subDirs(std::move(subDirs)) {}
};

class NoteManager {
public:
	void scan(const std::string& rootDir);
	std::vector<Note> flattenAll();
	const DirNode& getRootNode() const;

private:
	std::unique_ptr<DirNode> _rootNode;
	DirNode buildTree(const std::string& dirPath);
	void flattenHelper(const DirNode& node, std::vector<Note>& allNotes);
};
