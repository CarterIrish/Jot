#pragma once

#ifndef _WIN32
#include <termios.h>
#endif

enum class Key {
    Unknown,
    Up,
    Down,
    Left,
    Right,
    Enter,
    Escape,
    Char,
};

struct KeyEvent {
    Key key = Key::Unknown;
    char ch = 0;
};

class Terminal {
public:
    Terminal(); // Enters Raw mode
    ~Terminal(); // Restores mode

    Terminal(const Terminal&) = delete;
    Terminal& operator=(const Terminal&) = delete;

	KeyEvent readKey();

private:
#ifdef _WIN32
    unsigned long _savedInMode = 0;
    unsigned long _savedOutMode = 0;
#else
    [[maybe_unused]] termios _savedMode{};
#endif
};
