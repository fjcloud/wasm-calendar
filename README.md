# Terminal Calendar

A retro terminal-style calendar with WebAssembly. Interactive time grid with drag-and-drop, right-click event creation, and localStorage persistence.

## Features

- **Three View Modes**: Day, Week (Monday-Sunday), and Month views with smooth switching
- **Fully Responsive**: Adapts to any window size - resize your browser and watch it scale
- **Time Grid View**: Google Calendar-style hourly grid (7am-11pm) with proportional sizing
- **Drag & Drop**: Click and drag events to reschedule them instantly
- **Right-Click Creation**: Right-click any time slot to create an event with pre-filled time
- **Event Duration**: Create events with start and end times, or mark as all-day
- **Visual Timeline**: Events appear as colored blocks positioned by time with duration-based height
- **Auto-save**: Events persist in browser localStorage
- **No Scrolling**: Everything fits in viewport with dynamic grid sizing
- **Classic Terminal**: Timeless phosphor green aesthetic
- **WebAssembly**: Runs entirely in browser, no backend needed

## Quick Start

### Build

```bash
make build
```

Outputs WASM files to `docs/` directory.

### Serve

```bash
make serve
# Opens http://localhost:8000
```

Or use any static file server:
```bash
cd docs
python3 -m http.server 8080
```

## Usage

- **View Modes**: Click `DAY`, `WEEK`, or `MONTH` buttons to switch views
  - **Day View**: Single day with 7am-11pm time grid that fills entire window
  - **Week View**: Monday-Sunday grid with hourly time slots (always shows full week)
  - **Month View**: Traditional calendar grid
- **Navigation**: Use `< PREV` / `NEXT >` to navigate. `TODAY` jumps to current date
- **Day Selection**: Click day headers in Week view or calendar cells in Month view

### Creating Events

- **Right-click any time slot** in Day/Week view → popup opens with time pre-filled
- Or select a date and click `ADD EVENT` button in Month view
- Choose all-day or adjust start/end times
- Enter description and `SAVE`

### Managing Events

- **Drag and drop** - left-click and drag events to reschedule them
- Events appear as colored blocks in time grid, sized by duration
- In Month view, click `[DEL]` to remove events
- All changes auto-save to localStorage

### Grid Features

- **Fully responsive** - resizes smoothly with browser window
- **Dynamic sizing** - time grid and month view adapt to any viewport size
- **Optimized hours** - 7am to 11pm range for maximum slot visibility
- **No scrolling** - everything visible at once
- **Proportional layout** - all dimensions calculated as percentages for consistent scaling
- **Pixel-perfect alignment** - day headers align exactly with grid columns

## Development

### Project Structure

```
src/
├── main.cpp         # Entry point
├── event.*          # Event management
├── calendar.*       # Date calculations
├── storage.*        # LocalStorage persistence
├── ui.h             # UI interface
├── ui_core.cpp      # Main rendering & setup
├── ui_views.cpp     # Day/Week/Month views
├── ui_events.cpp    # Event dialogs
└── ui_timegrid.cpp  # Time grid & drag-drop
```

**Modular Design**: UI split into 4 focused modules for better maintainability:
- `ui_core.cpp` - Main rendering, view switching, styling
- `ui_views.cpp` - Day/Week/Month view logic
- `ui_events.cpp` - Event creation/editing dialogs
- `ui_timegrid.cpp` - Time grid rendering and drag-drop interaction

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

- **Event Management**: CRUD operations, time sorting, drag-and-drop rescheduling
- **Calendar Logic**: Date math, week calculations, Monday-based week system
- **Storage**: JSON serialization to localStorage with auto-save
- **UI**: ImGui with custom terminal styling and direct DrawList rendering for grid
- **Interaction**: Mouse position detection for right-click creation and drag-drop

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
