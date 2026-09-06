# jot

A CLI/TUI note manager written in C++17. Point it at a directory and it recursively
scans for your notes, sorting them case-insensitively as it goes.

`Config` and `NoteManager` are implemented. The `TUI` layer is still a stub, so the current
executable scans the notes directory and prints every note path it found.

## Building

**Requirements:** CMake >= 3.14, Visual Studio 2026 with the C++ workload.

```
git clone https://github.com/CarterIrish/Jot.git
cd Jot
```

Open the folder in Visual Studio 2026 via **File > Open > Folder**. VS will detect
`CMakeLists.txt` and configure automatically. Then **Build > Build All**.

The first configure downloads nlohmann/json v3.11.3 via CMake `FetchContent`, so it needs
network access. No system-wide install of the library is required.

Output: `out/build/x64-Debug/jot.exe` or `out/build/x64-Release/jot.exe`.

## First run

`jot` keeps `config.json` beside the executable. On first launch it prompts for the full path
to your notes directory and writes the file. The prompt trims whitespace, strips the surrounding
quotes a dragged-and-dropped path often carries, and re-asks until the path names an existing
directory. Pressing Enter alone exits.

If `config.json` is later corrupted, unreadable, or missing its `root_dir`, `jot` reports the
problem and falls back to the same setup prompt. `config.example.json` shows the expected shape;
`config.json` itself is gitignored so your local path is never committed.

## Architecture

- `Config` - loads and saves settings (root notes directory, editor) to `config.json`
- `NoteManager` - scans the notes directory and builds the in-memory note tree, kept UI-agnostic
  so it can back both a TUI and a future ImGui GUI
- `TUI` - terminal interface, drives `NoteManager`

`NoteManager::scan` builds a tree of `DirNode`s, each holding its `Note`s and its subdirectories.
Notes are sorted case-insensitively by filename and subdirectories by path.
`NoteManager::flattenAll` walks that tree to produce a single flat list. Every file is collected
regardless of extension; there is no filter yet.

## Symlinks and junctions

The scan follows symlinks and directory junctions like ordinary folders, which on its own would
recurse forever when a link points back up the tree. `NoteManager` tracks the chain of
directories it is currently inside, and a link resolving to one of them is added to the tree as
an empty node rather than followed. The link stays visible without its contents being scanned
twice or looping.

Directories are matched by file identity rather than by comparing path text, so the guard is not
fooled by case differences on NTFS, 8.3 short names, or the link indirection itself. This also
means junctions are covered, not just symlinks.

Only ancestors count. A directory reachable from two separate branches - two links pointing at
the same real folder, say - is expanded in both places, since that terminates fine.

A directory that cannot be read at all, such as a broken junction or one the OS denies access to,
is recorded as an empty node and reported as a warning after the scan rather than aborting it:

```
Scanning completed.
  warning: skipped "C:\Notes\archive" (Access is denied.)
```

`NoteManager::getSkippedDirs` exposes those entries, each paired with the reason it was skipped.

## Status

Working: config load/save with first-run setup, recursive directory scanning, sorting, symlink
and junction cycle handling, skipped-directory reporting.

Not yet implemented: the TUI itself (list rendering, cursor navigation, the View/Edit submenu)
and opening a note in the configured editor. The `editor` setting is stored but not used yet.
