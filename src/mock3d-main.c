#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <stdint.h>
#include <stdbool.h>

#define WIDTH  640
#define HEIGHT 420
#define WINDOW_FLAGS 0
#define RENDER_FLAGS SDL_RENDERER_ACCELERATED
#define FRAME_DELAY 16

#define NAME "Mock3d"

#define DYN_ASSERT(asserted, ...) if(!asserted) {fprintf(stderr, __VA_ARGS__); exit(-1);}

struct Window_Data_S {
    SDL_Window*   window;
    SDL_Renderer* renderer;
    uint32_t width;
    uint32_t height;
    char* name;
    uint32_t window_flags;
    uint32_t render_flags;
    bool is_running;
} window_data;


void process_input(struct Window_Data_S *window_data);

void process_input(struct Window_Data_S *window_data) {
    SDL_Event event;

    while(SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            window_data->is_running = false;
            break;
        }
    }
}

int main (int argv, char **argc) {
    window_data.width = WIDTH;
    window_data.height = HEIGHT;
    window_data.name = NAME;
    window_data.window_flags = WINDOW_FLAGS;
    window_data.render_flags = RENDER_FLAGS;

    DYN_ASSERT(!SDL_Init(SDL_INIT_VIDEO), 
               "Unable to initialize SDL, Diagnostic: %s", SDL_GetError());


    window_data.window = 
        SDL_CreateWindow(
            window_data.name, 
            SDL_WINDOWPOS_UNDEFINED, 
            SDL_WINDOWPOS_UNDEFINED,
            window_data.width,
            window_data.height,
            window_data.window_flags);
    DYN_ASSERT(window_data.window,
               "Unable to open SDL_Window, Diagostic: %s", SDL_GetError());

    window_data.renderer =
        SDL_CreateRenderer(
                window_data.window,
                -1,
                window_data.render_flags);
    DYN_ASSERT(window_data.renderer,
               "Unable to open SDL_Renderer, Diagostic: %s", SDL_GetError());


    window_data.is_running = true;
    while(window_data.is_running) {
        process_input(&window_data);

        SDL_Delay(FRAME_DELAY);
    }

    return 0;
}
