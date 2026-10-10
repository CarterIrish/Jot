#include "TUI.h"
#include  <iostream>
#include "NoteManager.h"
TUI::TUI(NoteManager& noteManager) : _noteManager(noteManager) {}

/**
 * Starts the TUI and waits for user input to exit.
 */
void TUI::run()
{
	// Build the browser entries from the note manager's root node
	_browserEntries = buildBrowserEntries(_noteManager.getRootNode());
	render();
	std::cout << "press any key to exit...\n";
	KeyEvent e = _term.readKey();
	std::cout << "You pressed: " << static_cast<int>(e.key) << " \'" << e.ch << "\'\n";
}

/**
 * Renders a frame to the terminal.
 */
void TUI::render() const {
	std::string frame;
	for (const BrowserEntry& entry : _browserEntries) {
		switch (entry.type) {
			case TUI::EntryType::Dir:
				frame += "[DIR] ";
				break;
			case TUI::EntryType::Note:
				frame += "[NOTE] ";
				break;
			default:
				frame += "[UNKNOWN] ";
				break;
		}
		frame += entry.displayName;
		frame += "\n";
	}
	std::cout << frame << std::flush;
}

/**
 * Builds a list of browser entries from a directory node.
 * @param node The directory node to build entries from.
 * @return A vector of browser entries.
 */
std::vector<TUI::BrowserEntry> TUI::buildBrowserEntries(const DirNode& node)
{
	std::vector<TUI::BrowserEntry> entries;
	entries.reserve(node.subDirs.size() + node.notes.size());
	for (const std::unique_ptr<DirNode>& subDir : node.subDirs) {
		entries.push_back({ EntryType::Dir, subDir->dirPath.filename().u8string(), subDir.get(), nullptr });
	}
	for (const Note& note : node.notes) {
		entries.push_back({ EntryType::Note, note.filePath.filename().u8string(), nullptr, &note });
	}
	return entries;
}

