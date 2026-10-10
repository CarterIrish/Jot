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

public:
	explicit TUI(NoteManager& noteManager);
	void run();
private:
	void render() const;
	static std::vector<BrowserEntry> buildBrowserEntries(const DirNode& node);

	std::vector<BrowserEntry> _browserEntries;
	std::size_t _selectedIndex = 0;
	NoteManager& _noteManager;
	Terminal _term;
};

