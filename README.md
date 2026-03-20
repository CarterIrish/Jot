# jot

A portable CLI note manager for Windows `.txt` files. Scans a configured directory, presents an interactive list, and opens notes in Notepad.

## Building

**Requirements:** CMake >= 3.14, Visual Studio 2022 with C++ workload.

```
git clone https://github.com/CarterIrish/Jot.git
cd Jot
```

Open the folder in Visual Studio 2022 via **File > Open > Folder**. VS will detect `CMakeLists.txt` and configure automatically. Then **Build > Build All**.

Output: `out/build/x64-Debug/jot.exe` or `out/build/x64-Release/jot.exe`.

## Usage

On first launch, jot will prompt you for the path to your notes directory. This is saved to `config.json` next to the executable and loaded on subsequent runs.

You can paste a path copied with "Copy as path" from Windows Explorer — surrounding quotes are handled automatically.

## Configuration

`config.json` is generated on first run and gitignored. A template is provided in `config.example.json`:

```json
{
    "root_dir": "C:/Users/YourName/Notes",
    "editor": "notepad"
}
```

## Status

Currently in early alpha. See [releases](https://github.com/CarterIrish/Jot/releases) for what's implemented.
