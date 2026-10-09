#include "TUI.h"
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <stdexcept>

TUI::~TUI() {
	if (!_isRawMode) return;
	_isRawMode = false;
	SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), _savedInMode);
	SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), _savedOutMode);
}

TUI::TUI() {
	HANDLE hStdIn = GetStdHandle(STD_INPUT_HANDLE);
	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD inMode, outMode;

	// Setup the Input mode
	if (!GetConsoleMode(hStdIn, &inMode)) {
		throw std::runtime_error("GetConsoleMode failed");
	}
	_savedInMode = inMode;
	inMode &= ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT | ENABLE_PROCESSED_INPUT);

	if (!SetConsoleMode(hStdIn, inMode)) {
		throw std::runtime_error("SetConsoleMode failed");
	}
	_isRawMode = true;

	try {
		if (!GetConsoleMode(hStdOut, &outMode)) {
			throw std::runtime_error("GetConsoleMode failed");
		}
		_savedOutMode = outMode;
		outMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
		if (!SetConsoleMode(hStdOut, outMode)) {
			throw std::runtime_error("SetConsoleMode failed");
		}
	}
	catch (...) {
		SetConsoleMode(hStdIn, _savedInMode);
		_isRawMode = false;
		throw;
	}
}

Key TUI::ReadKey() {
	INPUT_RECORD irInBuf[128];
	HANDLE hStdIn = GetStdHandle(STD_INPUT_HANDLE);
	DWORD cNumRead;
	if (hStdIn == INVALID_HANDLE_VALUE)
	{
		throw std::runtime_error("GetStdHandle failed");
	}
	if (!_isRawMode)
	{
		throw std::runtime_error("Terminal is not in raw mode");
	}

	for (;;) {
		if(!ReadConsoleInputA(hStdIn, irInBuf, 1, &cNumRead))
			throw std::runtime_error("ReadConsoleInput failed");
		for (DWORD i = 0; i < cNumRead; i++) {
			if (irInBuf[i].EventType != KEY_EVENT)
				continue;
			if (!irInBuf[i].Event.KeyEvent.bKeyDown)
				continue;
			return KeyEventProc(irInBuf[i].Event.KeyEvent);
		}
	}
}

Key TUI::KeyEventProc(const KEY_EVENT_RECORD& ker) {
	switch (ker.wVirtualKeyCode) {
		case VK_UP: return Key::Up;
		case VK_DOWN: return Key::Down;
		case VK_RIGHT: return Key::Right;
		case VK_LEFT: return Key::Left;
		case VK_RETURN: return Key::Enter;
		case VK_ESCAPE: return Key::Escape;
		default: return Key::Unknown;
	}
}

void TUI::Clear() {
	std::fputs("\x1b[H", stdout);
}

/**
 * Runs the text-based user interface for the note manager.
 */
void TUI::run([[maybe_unused]] NoteManager& manager) {}
