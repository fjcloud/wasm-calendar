# Terminal Calendar

A retro terminal-style calendar with WebAssembly. Monthly and weekly views, event scheduling with times, localStorage persistence.

## Features

- 📅 **Monthly/Weekly Views**: Toggle between full month grid and detailed week agenda
- ⏰ **Timed Events**: Schedule events with 24-hour time format
- 💾 **Auto-save**: Events persist in browser localStorage
- 🖥️ **Classic Terminal**: Timeless phosphor green aesthetic
- 🚀 **WebAssembly**: Runs entirely in browser, no backend needed

## Quick Start

### Build

```bash
make build
```

Outputs WASM files to `docs/` directory.

### Serve

```bash
make serve
# Opens http://localhost:8080
```

Or use any static file server:
```bash
cd docs
python3 -m http.server 8080
```

## Usage

- **[WEEK VIEW]** / **[MONTH VIEW]**: Toggle between views
- **< PREV / NEXT >**: Navigate months or weeks
- **TODAY**: Jump to current date
- **ADD EVENT**: Schedule events with time
- Click dates to select, events show as `[HH:MM] Description`

## Development

### Project Structure

```
src/
├── main.cpp        # Entry point
├── event.*         # Event management
├── calendar.*      # Date calculations
├── storage.*       # LocalStorage persistence
└── ui.*            # ImGui rendering
```

### Making Changes

```bash
# Edit source files in src/
vim src/ui.cpp

# Rebuild
make build

# Test
make serve
```

### Architecture

- **Event Management**: CRUD operations, time sorting
- **Calendar Logic**: Date math, week calculations
- **Storage**: JSON serialization to localStorage
- **UI**: ImGui with terminal styling

## Tech Stack

- C++17 with modular architecture
- Dear ImGui for UI
- SDL2 for window/input
- Emscripten 4.0.15-arm64 for WASM compilation
- OpenGL ES 2.0 for rendering

## Browser Support

Modern browsers with WebAssembly:
- Chrome/Edge 57+
- Firefox 52+
- Safari 11+

## License

MIT

## Credits

- [Dear ImGui](https://github.com/ocornut/imgui)
- [Emscripten](https://emscripten.org/)
- [SDL2](https://www.libsdl.org/)
