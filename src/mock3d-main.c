#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <stdint.h>
#include <stdbool.h>

#define WIDTH  320
#define HEIGHT 180
#define WINDOW_FLAGS 0
#define RENDER_FLAGS SDL_RENDERER_ACCELERATED
#define FRAME_DELAY 16

#define NAME "Mock3d"

#define DYN_ASSERT(asserted, ...) if(!asserted) {fprintf(stderr, __VA_ARGS__); exit(-1);}

typedef union __attribute__((packed)){
    struct __attribute__((packed)){
        uint8_t r;
        uint8_t g;
        uint8_t b;
    };
    uint8_t value[3];
} pixel_t;

typedef union {
    pixel_t pixels[WIDTH * HEIGHT];
    uint8_t bytes[WIDTH * HEIGHT * sizeof(pixel_t)];
} screen_t;
screen_t screen = {0};

struct Window_Data_S {
    SDL_Window*   window;
    SDL_Renderer* renderer;
    SDL_Texture*  texture;
    uint32_t width;
    uint32_t height;
    char* name;
    uint32_t window_flags;
    uint32_t render_flags;
    bool is_running;
} window_data;

uint8_t map_data[8][8] =
    {{1,1,1,1,1,1,1,1},
     {1,0,0,0,0,0,0,1},
     {1,0,0,0,0,0,0,1},
     {1,0,0,0,0,0,0,1},
     {1,0,0,0,0,0,0,1},
     {1,0,0,0,0,0,0,1},
     {1,0,0,0,0,0,0,1},
     {1,1,1,1,1,1,1,1}};


void process_input(struct Window_Data_S *window_data);
void render(screen_t *screen);

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

void render(screen_t *screen) {
    memset(screen, 32, WIDTH * HEIGHT * sizeof(pixel_t));
}

int main (int argv, char **argc) {
    window_data.width = WIDTH*4;
    window_data.height = HEIGHT*4;
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

    window_data.texture =
        SDL_CreateTexture(
                window_data.renderer,
                SDL_PIXELFORMAT_RGB24,
                SDL_TEXTUREACCESS_STREAMING,
                WIDTH,
                HEIGHT);
    DYN_ASSERT(window_data.texture,
               "Unable to create rendering texture (SDL_Texture), Diagnostic: %s", SDL_GetError());

    window_data.is_running = true;
    while(window_data.is_running) {

        process_input(&window_data);

        render(&screen);
        SDL_UpdateTexture(window_data.texture, NULL, &screen.bytes, WIDTH * 3);
        SDL_RenderCopyEx(
            window_data.renderer,
            window_data.texture,
            NULL,
            NULL,
            0.0,
            NULL,
            0);
        SDL_RenderPresent(window_data.renderer);

        SDL_Delay(FRAME_DELAY);
    }


    SDL_DestroyTexture(window_data.texture);
    SDL_DestroyRenderer(window_data.renderer);
    SDL_DestroyWindow(window_data.window);


    return 0;
}
