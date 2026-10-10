#include "Terminal.h"
#include <stdexcept>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

namespace {
	KeyEvent translateKey(const KEY_EVENT_RECORD& rec) {
		switch (rec.wVirtualKeyCode) {
		case VK_UP: return { Key::Up };
		case VK_DOWN: return { Key::Down };
		case VK_LEFT: return { Key::Left };
		case VK_RIGHT: return { Key::Right };
		case VK_RETURN: return { Key::Enter };
		case VK_ESCAPE: return { Key::Escape };
		}
		const char ch = rec.uChar.AsciiChar;
		if (ch >= 32 && ch <= 126) {
			return { Key::Char, ch };
		}
		return { Key::Unknown };
	}
}

Terminal::Terminal() {
	HANDLE hStdIn = GetStdHandle(STD_INPUT_HANDLE);
	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

	DWORD inMode = 0;
	if (!GetConsoleMode(hStdIn, &inMode)) {
		throw std::runtime_error("stdin is not a console");
	}

	_savedInMode = inMode;
	inMode &= ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT | ENABLE_PROCESSED_INPUT);
	if (!SetConsoleMode(hStdIn, inMode)) {
		throw std::runtime_error("could not enter raw input mode");
	}

	DWORD outMode = 0;
	if (!GetConsoleMode(hStdOut, &outMode) ||
		!SetConsoleMode(hStdOut, outMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING)) {
		SetConsoleMode(hStdIn, _savedInMode);
		throw std::runtime_error("could not enable ANSI output");
	}
	_savedOutMode = outMode;
}

Terminal::~Terminal() {
	SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), _savedInMode);
	SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), _savedOutMode);
}

KeyEvent Terminal::readKey() {
	HANDLE hStdIn = GetStdHandle(STD_INPUT_HANDLE);
	INPUT_RECORD rec;
	DWORD count = 0;
	for (;;) {
		if (!ReadConsoleInputA(hStdIn, &rec, 1, &count)) {
			throw std::runtime_error("failed to read console input");
		}
		if (count == 1 && rec.EventType == KEY_EVENT && rec.Event.KeyEvent.bKeyDown) {
			return translateKey(rec.Event.KeyEvent);
		}
	}
}

#else
Terminal::Terminal() {
	throw std::runtime_error("Terminal input is not implemented on this platform yet");
}

Terminal::~Terminal() {
	// No-op
}

KeyEvent Terminal::readKey() {
	return {};
}

#endif
