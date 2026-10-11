#pragma once
#include "NoteManager.h"
#include <string>
#include <vector>
#include  "Terminal.h"

class TUI 
{
	enum class EntryType {
		Dir,
		Note
	};
	
	struct BrowserEntry {
		EntryType type;
		std::string displayName;
		const DirNode* dirNode = nullptr; // Only used if type == DirEntry
		const Note* note = nullptr; // Only used if type == NoteEntry
	};

	struct NavFrame {
		const DirNode* node;
		std::size_t selectedIndex;
		std::size_t scrollOffset;
	};

public:
	explicit TUI(NoteManager& noteManager);
	void run();
private:
	void render() const;
	static std::vector<BrowserEntry> buildBrowserEntries(const DirNode& node);
	void clampScroll(std::size_t visibleRows);

	NavFrame& current() { return _dirStack.back(); }
	const NavFrame& current() const { return _dirStack.back(); }

	std::vector<BrowserEntry> _browserEntries;
	NoteManager& _noteManager;
	Terminal _term;
	bool _isRunning = true;
	std::vector<NavFrame> _dirStack; // Stack to keep track of the directory navigation
};

