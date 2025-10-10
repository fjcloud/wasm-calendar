#!/bin/bash
set -e

echo "============================================"
echo "  WASM Calendar - Build Script"
echo "============================================"

# Create output directory
mkdir -p /app/dist

echo ""
echo "[1/4] Compiling Dear ImGui library..."
emcc -c imgui/imgui.cpp -o imgui.o -Iimgui
emcc -c imgui/imgui_demo.cpp -o imgui_demo.o -Iimgui
emcc -c imgui/imgui_draw.cpp -o imgui_draw.o -Iimgui
emcc -c imgui/imgui_tables.cpp -o imgui_tables.o -Iimgui
emcc -c imgui/imgui_widgets.cpp -o imgui_widgets.o -Iimgui
emcc -c imgui/backends/imgui_impl_sdl2.cpp -o imgui_impl_sdl2.o -Iimgui -s USE_SDL=2
emcc -c imgui/backends/imgui_impl_opengl3.cpp -o imgui_impl_opengl3.o -Iimgui -s USE_SDL=2

echo "[2/4] Compiling core domain modules..."
emcc -c src/core/event.cpp -o event.o -Isrc -Iimgui -s USE_SDL=2
emcc -c src/core/calendar.cpp -o calendar.o -Isrc -Iimgui -s USE_SDL=2
emcc -c src/core/storage.cpp -o storage.o -Isrc -Iimgui -s USE_SDL=2

echo "[3/4] Compiling UI modules..."
emcc -c src/ui/ui_core.cpp -o ui_core.o -Isrc -Iimgui -Iimgui/backends -s USE_SDL=2
emcc -c src/ui/ui_views.cpp -o ui_views.o -Isrc -Iimgui -Iimgui/backends -s USE_SDL=2
emcc -c src/ui/ui_events.cpp -o ui_events.o -Isrc -Iimgui -Iimgui/backends -s USE_SDL=2
emcc -c src/ui/ui_timegrid.cpp -o ui_timegrid.o -Isrc -Iimgui -Iimgui/backends -s USE_SDL=2

echo "      Compiling main entry point..."
emcc -c src/main.cpp -o main.o -Isrc -Iimgui -Iimgui/backends -s USE_SDL=2

echo "[4/4] Linking WASM application..."
emcc -o /app/dist/index.html \
    main.o event.o calendar.o storage.o \
    ui_core.o ui_views.o ui_events.o ui_timegrid.o \
    imgui.o imgui_demo.o imgui_draw.o imgui_tables.o imgui_widgets.o \
    imgui_impl_sdl2.o imgui_impl_opengl3.o \
    -s USE_SDL=2 \
    -s USE_WEBGL2=1 \
    -s FULL_ES3=1 \
    -s WASM=1 \
    -s ALLOW_MEMORY_GROWTH=1 \
    -s NO_EXIT_RUNTIME=1 \
    -s ASSERTIONS=1 \
    --shell-file /app/shell.html \
    -O2

echo ""
echo "============================================"
echo "  Build Complete!"
echo "============================================"
ls -lh /app/dist/
echo ""

