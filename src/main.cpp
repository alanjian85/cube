#include <cstdlib>
#include <iostream>

#include <SDL2/SDL.h>
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

    bool quit = false;
    while (!quit) {
        SDL_Event event;
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                quit = true;
                spdlog::info("SDL quitted");
            }
        }
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}