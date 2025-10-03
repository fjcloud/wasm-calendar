FROM emscripten/emsdk:4.0.15-arm64

# Install git for cloning Dear ImGui
RUN apt-get update && apt-get install -y git && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /app

# Clone Dear ImGui and its SDL2 backend
RUN git clone --depth 1 --branch v1.91.5 https://github.com/ocornut/imgui.git

# Copy source files
COPY src/ /app/src/
COPY web/shell.html /app/shell.html

# Build script
RUN echo '#!/bin/bash\n\
set -e\n\
\n\
# Compile ImGui sources\n\
emcc -c imgui/imgui.cpp -o imgui.o -Iimgui\n\
emcc -c imgui/imgui_demo.cpp -o imgui_demo.o -Iimgui\n\
emcc -c imgui/imgui_draw.cpp -o imgui_draw.o -Iimgui\n\
emcc -c imgui/imgui_tables.cpp -o imgui_tables.o -Iimgui\n\
emcc -c imgui/imgui_widgets.cpp -o imgui_widgets.o -Iimgui\n\
emcc -c imgui/backends/imgui_impl_sdl2.cpp -o imgui_impl_sdl2.o -Iimgui -s USE_SDL=2\n\
emcc -c imgui/backends/imgui_impl_opengl3.cpp -o imgui_impl_opengl3.o -Iimgui -s USE_SDL=2\n\
\n\
# Compile application modules\n\
emcc -c src/event.cpp -o event.o -Isrc -Iimgui -s USE_SDL=2\n\
emcc -c src/calendar.cpp -o calendar.o -Isrc -Iimgui -s USE_SDL=2\n\
emcc -c src/storage.cpp -o storage.o -Isrc -Iimgui -s USE_SDL=2\n\
emcc -c src/ui_core.cpp -o ui_core.o -Isrc -Iimgui -Iimgui/backends -s USE_SDL=2\n\
emcc -c src/ui_views.cpp -o ui_views.o -Isrc -Iimgui -Iimgui/backends -s USE_SDL=2\n\
emcc -c src/ui_events.cpp -o ui_events.o -Isrc -Iimgui -Iimgui/backends -s USE_SDL=2\n\
emcc -c src/ui_timegrid.cpp -o ui_timegrid.o -Isrc -Iimgui -Iimgui/backends -s USE_SDL=2\n\
emcc -c src/main.cpp -o main.o -Isrc -Iimgui -Iimgui/backends -s USE_SDL=2\n\
\n\
# Link everything\n\
emcc -o /app/dist/index.html \\\n\
    main.o event.o calendar.o storage.o \\\n\
    ui_core.o ui_views.o ui_events.o ui_timegrid.o \\\n\
    imgui.o imgui_demo.o imgui_draw.o imgui_tables.o imgui_widgets.o \\\n\
    imgui_impl_sdl2.o imgui_impl_opengl3.o \\\n\
    -s USE_SDL=2 \\\n\
    -s USE_WEBGL2=1 \\\n\
    -s FULL_ES3=1 \\\n\
    -s WASM=1 \\\n\
    -s ALLOW_MEMORY_GROWTH=1 \\\n\
    -s NO_EXIT_RUNTIME=1 \\\n\
    -s ASSERTIONS=1 \\\n\
    --shell-file /app/shell.html \\\n\
    -O2\n\
\n\
echo "Build complete! Output files in /app/dist/"\n\
ls -lh /app/dist/\n\
' > /app/build.sh && chmod +x /app/build.sh

# Create dist directory
RUN mkdir -p /app/dist

# Run the build
RUN /app/build.sh

WORKDIR /app/dist
