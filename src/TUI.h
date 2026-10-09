#pragma once
#include "NoteManager.h"
#ifndef _WIN32
#include <termios.h>
#else
struct _KEY_EVENT_RECORD;
#endif

enum class Key {
	Up = 1,
	Down = 2,
	Left = 3,
	Right = 4,
	Enter = 5,
	Escape = 6,
	Open = 7,
	Unknown = 0
};

class TUI {
public:
	TUI();
	~TUI();
	TUI(const TUI&) = delete;
	TUI& operator=(const TUI&) = delete;

	Key ReadKey();
	void Clear();
    void run(NoteManager& manager);
private:
#ifdef _WIN32
	static Key KetEventProc(const _KEY_EVENT_RECORD& ker);
#endif
	bool _isRawMode = false;

#ifdef _WIN32
	unsigned long _savedInMode = 0;
	unsigned long _savedOutMode = 0;
#else
	termios _savedMode{};
#endif

};