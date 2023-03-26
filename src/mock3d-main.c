#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#define WIDTH  320
#define HEIGHT 180
#define WINDOW_FLAGS 0
#define RENDER_FLAGS SDL_RENDERER_ACCELERATED
#define FRAME_DELAY 16
#define SCALE_FACTOR 4
#define NAME "Mock3d"

#define TEX_WIDTH 64
#define TEX_HEIGHT 64

#define sign(x) (x) < 0 ? -1 : 1

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

typedef union {
    struct __attribute__((packed)){
        double x;
        double y;
    };
    double v[2];
} v2f_t;

typedef union {
    struct __attribute__((packed)){
        uint32_t x;
        uint32_t y;
    };
    uint32_t v[2];
} v2i_t;

struct Player_S {
    v2f_t pos;
    v2f_t dir;
    v2f_t plane;
} player;

bool keys[4] = {0};

uint8_t map_data[8][8] =
    {{1,2,1,2,1,2,1,1},
     {1,0,0,0,0,0,0,1},
     {1,0,0,0,0,0,0,2},
     {3,0,0,0,1,0,0,1},
     {2,0,0,3,2,0,0,3},
     {2,0,0,0,0,0,0,1},
     {1,0,0,0,0,0,0,1},
     {1,1,1,2,1,3,1,1}};

pixel_t textures[TEX_HEIGHT * TEX_WIDTH][4];


void generate_textures(pixel_t textures[TEX_HEIGHT * TEX_WIDTH][4]);
void process_input(struct Window_Data_S *window_data);
void render(screen_t *screen);

void generate_textures(pixel_t textures[TEX_HEIGHT * TEX_WIDTH][4]){
    for (int tex_i = 0; tex_i < 4; tex_i++) {
        for( uint32_t tex_coord = 0; tex_coord < TEX_HEIGHT * TEX_WIDTH; tex_coord++){
            uint32_t grad256 = ((256 * tex_coord)  / (TEX_HEIGHT * TEX_WIDTH));
            textures[tex_coord][0] = 
                ((tex_coord * tex_coord) / (TEX_HEIGHT * TEX_HEIGHT)) -
                ((tex_coord * tex_coord) % (TEX_HEIGHT * TEX_HEIGHT)) == 0 ?
                    (pixel_t) {.r = 255, .g = 0, .b = 0} :
                    (pixel_t) {0};
            textures[tex_coord][1] =
                (tex_coord / TEX_HEIGHT) -
                (tex_coord % TEX_HEIGHT) == 0 ?
                    (pixel_t) {.r = 255, .g = 0, .b = 0} :
                    (pixel_t) {0};
            textures[tex_coord][2] =
                (pixel_t) {.r = grad256, .g = grad256, .b = grad256};
            textures[tex_coord][3] =
                (((tex_coord / TEX_HEIGHT) % 16 && (tex_coord % TEX_HEIGHT) % 16)) ?
                    (pixel_t) {.r = 255, .g = 0, .b = 0} :
                    (pixel_t) {0};
        }
    }

}

void process_input(struct Window_Data_S *window_data) {
    SDL_Event event;

    while(SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            window_data->is_running = false;
            break;
        case SDL_KEYDOWN:
            if(event.key.keysym.scancode == SDL_SCANCODE_W)
                keys[0] = true;
            if(event.key.keysym.scancode == SDL_SCANCODE_S)
                keys[1] = true;
            if(event.key.keysym.scancode == SDL_SCANCODE_A)
                keys[2] = true;
            if(event.key.keysym.scancode == SDL_SCANCODE_D)
                keys[3] = true;
            break;

        case SDL_KEYUP:
            if(event.key.keysym.scancode == SDL_SCANCODE_W)
                keys[0] = false;
            if(event.key.keysym.scancode == SDL_SCANCODE_S)
                keys[1] = false;
            if(event.key.keysym.scancode == SDL_SCANCODE_A)
                keys[2] = false;
            if(event.key.keysym.scancode == SDL_SCANCODE_D)
                keys[3] = false;
            break;
        }
    }

    double rot_speed = .1;

    if(keys[0]){
        if(!map_data[(int)(player.pos.x + player.dir.x * 0.1)][(int)player.pos.y]) 
            player.pos.x += player.dir.x * 0.1;
        if(!map_data[(int)player.pos.x][(int)(player.pos.y + player.dir.y * 0.1)]) 
            player.pos.y += player.dir.y * 0.1;
    }
    if(keys[1]){
        if(!map_data[(int)(player.pos.x - player.dir.x * 0.1)][(int)player.pos.y]) 
            player.pos.x -= player.dir.x * 0.1;
        if(!map_data[(int)player.pos.x][(int)(player.pos.y - player.dir.y * 0.1)]) 
            player.pos.y -= player.dir.y * 0.1;
    }
    if(keys[2]){
        double old_dir_x = player.dir.x;
        player.dir.x = player.dir.x*cos(-rot_speed) - player.dir.y * sin(-rot_speed);
        player.dir.y = old_dir_x*sin(-rot_speed) + player.dir.y * cos(-rot_speed);
        double old_plane_x = player.plane.x;
        player.plane.x = player.plane.x*cos(-rot_speed) - player.plane.y * sin(-rot_speed);
        player.plane.y = old_plane_x*sin(-rot_speed) + player.plane.y * cos(-rot_speed);
    }
    if(keys[3]){
        double old_dir_x = player.dir.x;
        player.dir.x = player.dir.x*cos(rot_speed) - player.dir.y * sin(rot_speed);
        player.dir.y = old_dir_x*sin(rot_speed) + player.dir.y * cos(rot_speed);
        double old_plane_x = player.plane.x;
        player.plane.x = player.plane.x*cos(rot_speed) - player.plane.y * sin(rot_speed);
        player.plane.y = old_plane_x*sin(rot_speed) + player.plane.y * cos(rot_speed);
    }

}

void render(screen_t *screen) {
    memset(screen, 32, WIDTH * HEIGHT * sizeof(pixel_t));


    for (int16_t column = 0; column < WIDTH; column++) {
        //Normalise column to values between -1 to 1
        double camera_x = 2 * column / (double) WIDTH - 1; 
        
        //The direction the ray will travel
        v2f_t ray_dir = {
            .x = player.dir.x + player.plane.x * camera_x,
            .y = player.dir.y + player.plane.y * camera_x};

        //The players position in grid coordinates
        v2i_t grid_pos = {
            .x = (int) player.pos.x,
            .y = (int) player.pos.y};

        //The distance from one x or y-side to the next in the rays direction
        v2f_t delta_dist = {
            .x = (ray_dir.x == 0) ? INFINITY : fabs(1 / ray_dir.x),
            .y = (ray_dir.y == 0) ? INFINITY : fabs(1 / ray_dir.y)};

        //The direction to do x or y steps in grid coordinates
        v2i_t step = {
            .x = sign(ray_dir.x),
            .y = sign(ray_dir.y)};

        //The distance to the first side on the left or right(x) and top or bottom(y)
        v2f_t side_dist = {
            .x = delta_dist.x * (
                ray_dir.x < 0 ? 
                    (player.pos.x - grid_pos.x) : 
                    (grid_pos.x + 1.0 - player.pos.x)),
            .y = delta_dist.y * (
                ray_dir.y < 0 ?
                    (player.pos.y - grid_pos.y) :
                    (grid_pos.y + 1.0 - player.pos.y))};

        //Raycast towards next wall
        enum {HORIZONTAL, VERTICAL} side;
        bool hit = false;

        while(!hit)
        {
            if (side_dist.x < side_dist.y){
                side_dist.x += delta_dist.x;
                grid_pos.x += step.x;
                side = HORIZONTAL;
            }
            else {
                side_dist.y += delta_dist.y;
                grid_pos.y += step.y;
                side = VERTICAL;
            }

            if(map_data[grid_pos.x][grid_pos.y] > 0)
                hit = true;
        }

        double perp_wall_dist =
            (side == HORIZONTAL) ?
                (side_dist.x - delta_dist.x):
                (side_dist.y - delta_dist.y);

        int32_t line_height = (int) (HEIGHT / perp_wall_dist);

        int32_t line_top, line_bottom;
        if(perp_wall_dist <= 1) {
            line_top    = 0;
            line_bottom = HEIGHT -1;
        }
        else {
            line_top    = -line_height/2 + HEIGHT/2;
            line_bottom =  line_height/2 + HEIGHT/2;
        }

        int tex_index = map_data[grid_pos.x][grid_pos.y] -1;

        double wall_x = 
            (side == HORIZONTAL) ?
                player.pos.y + perp_wall_dist * ray_dir.y :
                player.pos.x + perp_wall_dist * ray_dir.x;
        wall_x -= floor(wall_x);

        int tex_x = (int) (wall_x * (double)TEX_WIDTH);

        if(side == HORIZONTAL && ray_dir.x > 0) tex_x = TEX_WIDTH - tex_x -1;
        if(side == VERTICAL   && ray_dir.y < 0) tex_x = TEX_WIDTH - tex_x -1;

        double tex_step = 1.0 * TEX_HEIGHT / line_height;
        double tex_pos = (line_top - HEIGHT/ 2 + line_height / 2) * tex_step;


        /*pixel_t color;
        switch(map_data[grid_pos.x][grid_pos.y]) {
        case 1: color = (pixel_t){.value = {255,0,0}}; break;
        case 2: color = (pixel_t){.value = {0,0,255}}; break;
        case 3: color = (pixel_t){.value = {0,255,0}}; break;
        }

        if(side == VERTICAL) {
            color.r /= 2;
            color.g /= 2;
            color.b /= 2;
        }*/
        
        for (int32_t h = line_top; h < line_bottom; h++) {
            int tex_y = (int) tex_pos & (TEX_HEIGHT-1);
            tex_pos += tex_step;
            pixel_t color = textures[TEX_WIDTH * tex_x + tex_y][tex_index];
            if(side == VERTICAL) {
                color.r /= 2;
                color.g /= 2;
                color.b /= 2;
            }  
            screen->pixels[h * WIDTH + column] = color;
        }

    }
}

int main (int argv, char **argc) {
    window_data.width = WIDTH*SCALE_FACTOR;
    window_data.height = HEIGHT*SCALE_FACTOR;
    window_data.name = NAME;
    window_data.window_flags = WINDOW_FLAGS;
    window_data.render_flags = RENDER_FLAGS;

    player = (struct Player_S) {
        .pos = {{1,2}}, 
        .dir = {{1,0}},
        .plane = {.x = 0,
                  .y = 0.66}};

    generate_textures(textures);

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
