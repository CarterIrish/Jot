#include "TUI.h"
#include  <iostream>
#include "NoteManager.h"

TUI::TUI(NoteManager& noteManager) : _noteManager(noteManager) {}

/**
 * Starts the TUI and waits for user input to exit.
 */
void TUI::run()
{
	_isRunning = true;
	// Build the browser entries from the note manager's root node
	_browserEntries = buildBrowserEntries(_noteManager.getRootNode());
	while (_isRunning)
	{
		clampScroll(_term.rows()-1);
		render();
		KeyEvent e = _term.readKey();
		switch (e.key) {
		case Key::Escape:
			_isRunning = false;
			break;
		case Key::Up:
			if (_selectedIndex > 0) _selectedIndex--;
			break;
		case Key::Down:
			if (_selectedIndex + 1 < _browserEntries.size()) _selectedIndex++;
			break;
		case Key::Enter:
			if (_browserEntries[_selectedIndex].type == EntryType::Dir) {
				// Navigate into the dir
				_browserEntries = buildBrowserEntries(*_browserEntries[_selectedIndex].dirNode);
				_selectedIndex = 0;
				_scrollOffset = 0;	
			}
			break;
		}
	}
}

/**
 * Renders a frame to the terminal.
 */
void TUI::render() const {
	
	// determine the amount of space I have to print the entries
	std::size_t visibleRows = _term.rows() - 1;
	std::size_t end = std::min(_scrollOffset + visibleRows, _browserEntries.size());


	std::string frame;
	frame += "\033[?2026h"; // sync update
	frame += "\033[?25l"; // Hide the cursor
	frame += "\033[H"; // Move the cursor to top left

	for (std::size_t i = _scrollOffset; i < end; i++) {
		const BrowserEntry& entry = _browserEntries[i];
		if (i == _selectedIndex) frame += "\033[7m"; // Invert colors for selected entry
		frame += entry.type == EntryType::Dir ? "[DIR] " : "[NOTE] ";
		frame += entry.displayName;
		frame += "\033[K\033[0m\n";
	}
	frame += "Use arrow keys to navigate, Enter to open, Esc to exit. \033[K";
	frame += "\033[J"; // Clear to end of screen
	frame += "\033[?2026l"; // End the sync update

	std::cout << frame << std::flush;
}

/**
 * Builds a list of browser entries from a directory node.
 * @param node The directory node to build entries from.
 * @return A vector of browser entries.
 */
std::vector<TUI::BrowserEntry> TUI::buildBrowserEntries(const DirNode& node)
{
	// Create the vector of entries for a directory
	std::vector<TUI::BrowserEntry> entries;
	entries.reserve(node.subDirs.size() + node.notes.size()); // Avoid multiple reallocations 
	// Organize the dir first
	for (const std::unique_ptr<DirNode>& subDir : node.subDirs) {
		entries.push_back({ EntryType::Dir, subDir->dirPath.filename().u8string(), subDir.get(), nullptr });
	}
	for (const Note& note : node.notes) {
		entries.push_back({ EntryType::Note, note.filePath.filename().u8string(), nullptr, &note });
	}
	return entries;
}

void TUI::clampScroll(std::size_t visibleRows) {
	if (_selectedIndex > _scrollOffset + visibleRows - 1) {
		_scrollOffset = _selectedIndex - visibleRows + 1;
	}
	else if (_selectedIndex < _scrollOffset) {
		_scrollOffset = _selectedIndex;
	}
}
