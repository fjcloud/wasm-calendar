#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengles2.h>
#include <emscripten.h>
#include <emscripten/html5.h>

#include "ui.h"
#include "event.h"
#include "storage.h"

using namespace calendar;

// Global state
SDL_Window* g_Window = nullptr;
SDL_GLContext g_GLContext = nullptr;
CalendarState* g_State = nullptr;
EventManager* g_EventManager = nullptr;
CalendarUI* g_UI = nullptr;

void main_loop() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        ImGui_ImplSDL2_ProcessEvent(&event);
        
        // Handle window resize
        if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
            int width = event.window.data1;
            int height = event.window.data2;
            glViewport(0, 0, width, height);
        }
    }

    // Get current window size for viewport
    int display_w, display_h;
    SDL_GetWindowSize(g_Window, &display_w, &display_h);

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    // Render the calendar UI
    g_UI->render();

    // Rendering
    ImGui::Render();
    SDL_GL_MakeCurrent(g_Window, g_GLContext);
    glViewport(0, 0, display_w, display_h);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(g_Window);
}

int main(int, char**) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        return -1;
    }

    // GL ES 2.0
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    // Create window - size will be controlled by CSS/canvas in WASM
    g_Window = SDL_CreateWindow("Calendar",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        1400, 900,  // Default size, overridden by canvas in browser
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    
    g_GLContext = SDL_GL_CreateContext(g_Window);
    SDL_GL_MakeCurrent(g_Window, g_GLContext);
    SDL_GL_SetSwapInterval(1);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(g_Window, g_GLContext);
    ImGui_ImplOpenGL3_Init("#version 100");

    // Initialize application state
    g_State = new CalendarState();
    g_EventManager = new EventManager();
    g_UI = new CalendarUI(*g_State, *g_EventManager);
    
    // Setup terminal style
    g_UI->setupTerminalStyle();
    
    // Load events from localStorage
    StorageManager::loadEventsFromStorage(g_EventManager->getAllEvents());

    // Main loop
    emscripten_set_main_loop(main_loop, 0, 1);

    // Cleanup
    delete g_UI;
    delete g_EventManager;
    delete g_State;
    
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(g_GLContext);
    SDL_DestroyWindow(g_Window);
    SDL_Quit();

  return 0;
}
