#include <cstdlib>
#include <iostream>

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <spdlog/spdlog.h>

const int window_width = 640;
const int window_height = 480;

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        spdlog::error("SDL could not initialize! SDL Error: {}\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    SDL_Window* window = SDL_CreateWindow("Cube", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, window_width, window_height, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        spdlog::error("Window could not be created! SDL Error: {}\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    SDL_SysWMinfo wmi;
    SDL_VERSION(&wmi.version);
    if (!SDL_GetWindowWMInfo(window, &wmi)) {
        spdlog::error("Window WMI info could not be fetched! SDL Error: {}\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    bgfx::PlatformData pd;
    pd.ndt = wmi.info.x11.display;
    pd.nwh = reinterpret_cast<void*>(wmi.info.x11.window);
    bgfx::setPlatformData(pd);
    bgfx::renderFrame();
    
    if (!bgfx::init()) {
        spdlog::error("Bgfx could not initialize!\n");
        return EXIT_FAILURE;
    }
    bgfx::reset(window_width, window_height, BGFX_RESET_VSYNC);
    bgfx::setDebug(BGFX_DEBUG_TEXT);
    bgfx::setViewRect(0, 0, 0, window_width, window_height);
    bgfx::setViewClear(0,
                       BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH,
                       0x443355FF, 1.0f, 0);
    bgfx::touch(0);

    bool quit = false;
    while (!quit) {
        SDL_Event event;
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                quit = true;
                spdlog::info("SDL quitted");
            }

            bgfx::frame();
        }
    }

    bgfx::shutdown();
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}