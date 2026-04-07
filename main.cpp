#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_video.h>
#include <stdio.h>
#include <tracy/Tracy.hpp>

SDL_Surface *framebuffer = NULL;
SDL_Texture *texture = NULL;
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
bool running = true;
double y_off = 0.0;
double x_off = 0.0;

typedef struct {
    uint32_t *framebuffer;
    int pitch_pixels;
    int width;
    int height;
    bool left;
    bool right;
    bool up;
    bool down;
    double dt_secs;
} Context_t;

void CreateFramebuffer(int w, int h) {
    framebuffer = SDL_CreateSurface(w, h, SDL_PIXELFORMAT_ARGB8888);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                                SDL_TEXTUREACCESS_STREAMING, w, h);
}

void DestroyFramebuffer(void) {
    SDL_DestroySurface(framebuffer);
    SDL_DestroyTexture(texture);
}

void updateAndRender(Context_t ctx) {
    float x_dir = 0;
    float y_dir = 0;
    if (ctx.left) {
        x_dir += 1;
    }
    if (ctx.right) {
        x_dir -= 1;
    }
    if (ctx.up) {
        y_dir += 1;
    }
    if (ctx.down) {
        y_dir -= 1;
    }
    x_off += x_dir;
    y_off += y_dir;
    for (int y = 0; y < ctx.height; y++) {
        for (int x = 0; x < ctx.width; x++) {
            Uint8 r = 0;
            Uint8 g = (y + (int)y_off) % 256;
            Uint8 b = (x + (int)x_off) % 256;

            ctx.framebuffer[y * ctx.pitch_pixels + x] =
                (255 << 24) | (r << 16) | (g << 8) | b;
        }
    }
}

int main(int argc, char *argv[]) {
    ZoneScoped;
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        printf("Failed to initialize SDL: %s\n", SDL_GetError());
        return 1;
    }

    window = SDL_CreateWindow("SDL3 Hyprland", 800, 600, SDL_WINDOW_RESIZABLE);
    if (!window) {
        printf("Failed to create window: %s\n", SDL_GetError());
        return 1;
    }

    renderer = SDL_CreateRenderer(window, NULL);
    if (!renderer) {
        printf("Failed to create renderer: %s\n", SDL_GetError());
        return 1;
    }
    if (!SDL_SetRenderVSync(renderer, 1)) {
        SDL_Log("VSync not supported: %s", SDL_GetError());
    }
    SDL_Event event;
    int win_w, win_h;
    SDL_GetWindowSizeInPixels(window, &win_w, &win_h);
    CreateFramebuffer(win_w, win_h);

    uint64_t last = 0;
    Context_t ctx = {.framebuffer = 0,
                     .pitch_pixels = 0,
                     .width = framebuffer->w,
                     .height = framebuffer->h,
                     .left = false,
                     .right = false,
                     .up = false,
                     .down = false,
                     .dt_secs = 0};
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
            if (event.type == SDL_EVENT_KEY_DOWN) {
                if (event.key.key == SDLK_ESCAPE) {
                    running = false;
                }
            }
            if (event.type == SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED) {
                int new_w = event.window.data1;
                int new_h = event.window.data2;
                /* Destroy old resources */
                DestroyFramebuffer();
                CreateFramebuffer(new_w, new_h);
            }
        }
        const bool *keys = SDL_GetKeyboardState(NULL);
        ctx.left = keys[SDL_SCANCODE_A];
        ctx.right = keys[SDL_SCANCODE_D];
        ctx.up = keys[SDL_SCANCODE_W];
        ctx.down = keys[SDL_SCANCODE_S];

        Uint64 now = SDL_GetTicksNS();
        if (last == 0)
            last = now;
        ctx.dt_secs = (now - last) / 1e9; // seconds
        last = now;

        ctx.framebuffer = (Uint32 *)framebuffer->pixels;
        ctx.pitch_pixels = framebuffer->pitch / 4;
        ctx.width = framebuffer->w;
        ctx.height = framebuffer->h;

        updateAndRender(ctx);

        SDL_UpdateTexture(texture, NULL, framebuffer->pixels,
                          framebuffer->pitch);

        SDL_RenderClear(renderer);
        SDL_RenderTexture(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
        FrameMark;
    }

    // May destroy the window and quit SDL before exiting
    return 0;
}
