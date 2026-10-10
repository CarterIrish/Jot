#pragma once
#include "NoteManager.h"
#include <string>
#include <vector>


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
	TUI();
	void run();
private:
	std::vector<BrowserEntry> _browserEntries;
	std::size_t _selectedIndex = 0;
};
