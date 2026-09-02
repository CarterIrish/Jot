# jot

A CLI/TUI note manager. Currently a bare skeleton: `Config`, `NoteManager`, and `TUI` are stubbed out with no logic implemented yet.

## Building

**Requirements:** CMake >= 3.14, Visual Studio 2022 with C++ workload.

```
git clone https://github.com/CarterIrish/Jot.git
cd Jot
```

Open the folder in Visual Studio 2022 via **File > Open > Folder**. VS will detect `CMakeLists.txt` and configure automatically. Then **Build > Build All**.

Output: `out/build/x64-Debug/jot.exe` or `out/build/x64-Release/jot.exe`.

## Architecture

- `Config` - loads and saves settings (root notes directory, editor) to `config.json`
- `NoteManager` - scans the notes directory and manages the in-memory note list, kept UI-agnostic so it can back both a TUI and a future ImGui GUI
- `TUI` - terminal interface, drives `NoteManager`

`config.example.json` shows the expected shape of the config file once `Config` is implemented.

## Status

Skeleton only. Nothing is implemented yet.
