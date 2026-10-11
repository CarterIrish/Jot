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
	_dirStack.clear();
	_dirStack.push_back({ &_noteManager.getRootNode(), 0, 0 });
	_browserEntries = buildBrowserEntries(*current().node);	
	while (_isRunning)
	{
		clampScroll(_term.rows() - 1);
		render();
		KeyEvent e = _term.readKey();
		switch (e.key) {
		case Key::Escape:
			_isRunning = false;
			break;
		case Key::Up:
			if (current().selectedIndex > 0) current().selectedIndex--;
			break;
		case Key::Down:
			if (current().selectedIndex + 1 < _browserEntries.size()) current().selectedIndex++;
			break;
		case Key::Enter:
			if (!_browserEntries.empty() && _browserEntries[current().selectedIndex].type == EntryType::Dir) {
				const DirNode& dirNode = *_browserEntries[current().selectedIndex].dirNode;
				if (dirNode.subDirs.empty() && dirNode.notes.empty()) {
					// TODO: need UI feedback to indicate why the user cannot navigate into the directory
					break;
				}
				// The parent frame keeps its cursor, so Left returns to the same spot
				_dirStack.push_back({ &dirNode, 0, 0 });
				_browserEntries = buildBrowserEntries(dirNode);
			}
			break;
		case Key::Left:
			if (_dirStack.size() > 1) {
				_dirStack.pop_back();
				_browserEntries = buildBrowserEntries(*current().node);
			}
			break;
		}
	}
}


void TUI::render() const {
	const NavFrame& nav = current();
	std::size_t visibleRows = _term.rows() - 1;
	std::size_t end = std::min(nav.scrollOffset + visibleRows, _browserEntries.size());

	std::string frame;
	frame += "\033[?2026h"; // sync update
	frame += "\033[?25l";   // Hide the cursor
	frame += "\033[H";      // Move the cursor to top left

	for (std::size_t i = nav.scrollOffset; i < end; i++) {
		const BrowserEntry& entry = _browserEntries[i];
		if (i == nav.selectedIndex) frame += "\033[7m"; // Invert colors for selected entry
		frame += entry.type == EntryType::Dir ? "[DIR] " : "[NOTE] ";
		frame += entry.displayName;
		frame += "\033[K\033[0m\n";
	}
	frame += "Use arrow keys to navigate, Enter to open, Left to go back, Esc to exit. \033[K";
	frame += "\033[J";      // Clear to end of screen
	frame += "\033[?2026l"; // End the sync update

	std::cout << frame << std::flush;
}

void TUI::clampScroll(std::size_t visibleRows) {
	NavFrame& nav = current();
	if (nav.selectedIndex > nav.scrollOffset + visibleRows - 1) {
		nav.scrollOffset = nav.selectedIndex - visibleRows + 1;
	}
	else if (nav.selectedIndex < nav.scrollOffset) {
		nav.scrollOffset = nav.selectedIndex;
	}
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


