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

# Copy build script
COPY build.sh /app/build.sh
RUN chmod +x /app/build.sh

# Run the build
RUN /app/build.sh

WORKDIR /app/dist
