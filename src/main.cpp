#include <cstdlib>
#include <cstdint>
#include <iostream>

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <spdlog/spdlog.h>

const int window_width = 640;
const int window_height = 480;

struct pos_color_vertex {
    float x;
    float y;
    float z;
    std::uint32_t abgr;
    
    static void init() {
        layout
            .begin()
                .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
                .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
            .end();
    }

    static bgfx::VertexLayout layout;
};

bgfx::VertexLayout pos_color_vertex::layout;

static pos_color_vertex cube_vertices[] = {
    { 0.5f,  0.5f, 0.0f, 0xff0000ff},
    { 0.5f, -0.5f, 0.0f, 0xff0000ff},
    {-0.5f, -0.5f, 0.0f, 0xff00ff00},
    {-0.5f,  0.5f, 0.0f, 0xff00ff00}
};

static const std::uint16_t cube_tri_list[] = {
    0, 1, 3,
    1, 2, 3
};

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

    bgfx::VertexBufferHandle vbh; 
    bgfx::IndexBufferHandle ibh;

    pos_color_vertex::init();
    vbh = bgfx::createVertexBuffer(
        bgfx::makeRef(cube_vertices, sizeof(cube_vertices)),
        pos_color_vertex::layout
    );

    ibh = bgfx::createIndexBuffer(
        bgfx::makeRef(cube_tri_list, sizeof(cube_tri_list))
    );

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