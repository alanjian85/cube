#include <cstdlib>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <vector>

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <bx/math.h>

const int window_width = 640;
const int window_height = 480;

bgfx::ShaderHandle load_shader(const char* path) {
    std::ifstream file(path, std::ios::ate);
    std::vector<char> data;
    std::size_t size = file.tellg();
    data.resize(size);
    file.seekg(0, std::ios::beg);
    file.read(data.data(), size);
    const bgfx::Memory* memory = bgfx::copy(data.data(), data.size());
    memory->data[memory->size - 1] = '\0';
    bgfx::ShaderHandle shader = bgfx::createShader(memory);
    return shader;
}

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
    {-1.0f,  1.0f,  1.0f, 0xff000000 },
    { 1.0f,  1.0f,  1.0f, 0xff0000ff },
    {-1.0f, -1.0f,  1.0f, 0xff00ff00 },
    { 1.0f, -1.0f,  1.0f, 0xff00ffff },
    {-1.0f,  1.0f, -1.0f, 0xffff0000 },
    { 1.0f,  1.0f, -1.0f, 0xffff00ff },
    {-1.0f, -1.0f, -1.0f, 0xffffff00 },
    { 1.0f, -1.0f, -1.0f, 0xffffffff }
};

static const std::uint16_t cube_tri_list[] = {
    0, 1, 2,
    1, 3, 2,
    4, 6, 5,
    5, 6, 7,
    0, 2, 4,
    4, 2, 6,
    1, 5, 3,
    5, 7, 3,
    0, 4, 1,
    4, 5, 1,
    2, 3, 6,
    6, 3, 7
};

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Cube", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, window_width, window_height, SDL_WINDOW_SHOWN);

    SDL_SysWMinfo wmi;
    SDL_VERSION(&wmi.version);
    SDL_GetWindowWMInfo(window, &wmi);

    bgfx::PlatformData pd;
    pd.ndt = wmi.info.x11.display;
    pd.nwh = reinterpret_cast<void*>(wmi.info.x11.window);
    bgfx::setPlatformData(pd);

    bgfx::Init init;
    init.type = bgfx::RendererType::Count;
    init.resolution.width = window_width;
    init.resolution.height = window_height;
    init.resolution.reset = BGFX_RESET_VSYNC;
    bgfx::init(init);

    bgfx::setViewRect(0, 0, 0, window_width, window_height);
    bgfx::setViewClear(0,
                       BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH,
                       0x443355FF, 1.0f, 0);

    pos_color_vertex::init();
    bgfx::VertexBufferHandle vbh = bgfx::createVertexBuffer(
        bgfx::makeRef(cube_vertices, sizeof(cube_vertices)),
        pos_color_vertex::layout
    );

    bgfx::IndexBufferHandle ibh = bgfx::createIndexBuffer(
        bgfx::makeRef(cube_tri_list, sizeof(cube_tri_list))
    );

    bgfx::ShaderHandle vsh = load_shader("vs_cube.bin");
    bgfx::ShaderHandle fsh = load_shader("fs_cube.bin");
    bgfx::ProgramHandle program = bgfx::createProgram(vsh, fsh, true);

    bool quit = false;
    while (!quit) {
        SDL_Event event;
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                quit = true;
                spdlog::info("SDL quitted");
            }
        }

        bgfx::frame();

        bx::Vec3 at = {0.0f, 0.0f, 0.0f};
        bx::Vec3 eye = {0.0f, 0.0f, -5.0f};

        float view[16];
        bx::mtxLookAt(view, eye, at);

        float proj[16];
        bx::mtxProj(proj,
                    60.0f,
                    float(window_width) / float(window_height),
                    0.1f, 100.0f,
                    bgfx::getCaps()->homogeneousDepth);
        
        bgfx::setViewTransform(0, view, proj);

        float mtx[16];
        bx::mtxRotateXY(mtx, SDL_GetTicks() * 0.0025f, SDL_GetTicks() * 0.0025f);
        bgfx::setTransform(mtx);

        bgfx::setVertexBuffer(0, vbh);
        bgfx::setIndexBuffer(ibh);

        bgfx::submit(0, program);
        bgfx::frame();
    }

    bgfx::shutdown();
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}