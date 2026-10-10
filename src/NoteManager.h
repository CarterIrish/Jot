#pragma once
#include <filesystem>
#include <memory>
#include <string>
#include <utility>
#include <vector>


struct Note {
	std::filesystem::path filePath;
	
	explicit Note(std::filesystem::path path) : filePath(std::move(path)) {}
};

struct DirNode {
	std::filesystem::path dirPath;
	std::vector<Note> notes;
	std::vector<std::unique_ptr<DirNode>> subDirs;

	DirNode(std::filesystem::path path, std::vector<Note> notes, std::vector<std::unique_ptr<DirNode>> subDirs)
		: dirPath(std::move(path)), notes(std::move(notes)), subDirs(std::move(subDirs)) {}
};

struct SkippedDir {
	std::filesystem::path dirPath;
	std::string reason;

	SkippedDir(std::filesystem::path dirPath, std::string reason)
		: dirPath(std::move(dirPath)), reason(std::move(reason)) {}
};

class NoteManager {
public:
	void scan(const std::filesystem::path& rootDir);
	std::vector<Note> flattenAll();
	const DirNode& getRootNode() const;
	const std::vector<SkippedDir>& getSkippedDirs() const;

private:
	std::unique_ptr<DirNode> _rootNode;
	std::vector<SkippedDir> _skippedDirs;
	DirNode buildTree(const std::filesystem::path& dirPath, std::vector<std::filesystem::path>& ancestors);
	void flattenHelper(const DirNode& node, std::vector<Note>& allNotes);
};
