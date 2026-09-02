#include "Config.h"
#include "NoteManager.h"
#include "TUI.h"

int main() {
    Config config;
    NoteManager manager;
    TUI tui;

    config.load();
    manager.scan(config.getRootDir());
    tui.run(manager);

    return 0;
}
