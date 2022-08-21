#include "ui.h"
#include "include.h"
#include "bus.h"
#include "ppu.h"
#include "input.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

// GAME
SDL_Window *sdlWindow;
SDL_Renderer *sdlRenderer;
SDL_Texture *sdlTexture;
SDL_Surface *screen;
// DBG
SDL_Window *sdlDbgWindow;
SDL_Renderer *sdlDbgRenderer;
SDL_Texture *sdlDbgTexture;
SDL_Surface *DbgScreen;

static int scale = 4;

static unsigned long colors[4] = {0xFFFFFFFF, 0xFFAAAAAA, 0xFF555555, 0xFF000000};

void ui_init() 
{
    int x, y;
    SDL_Init(SDL_INIT_VIDEO);
    printf("SDL INIT\n");
    TTF_Init();
    printf("TTF INIT\n");

    // EMULATOR WINDOW
    SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &sdlWindow, &sdlRenderer);
    screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    sdlTexture = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);

    // DEBUG WINDOW
    /*SDL_CreateWindowAndRenderer(16 * 8 * scale, 32 * 8 * scale, 0, &sdlDbgWindow, &sdlDbgRenderer);
    DbgScreen = SDL_CreateRGBSurface(0, (16 * 8 * scale) + (16 * scale), (32 * 8 * scale) + (64 * scale), 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    sdlDbgTexture = SDL_CreateTexture(sdlDbgRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, (16 * 8 * scale) + (16 * scale), (32 * 8 * scale) + (64 * scale));
*/
    SDL_GetWindowPosition(sdlWindow, &x, &y);
  //  SDL_SetWindowPosition(sdlDbgWindow, x+SCREEN_WIDTH + 10, y);
}

void delay(u32 ms)
{
    SDL_Delay(ms);
}

u32 get_ticks()
{
    return SDL_GetTicks();
}

void display_tile(SDL_Surface *surface, u16 startLoc, u16 tileN, int x, int y)
{
    SDL_Rect rc;
    for (int tileY = 0; tileY < 16; tileY +=2) {
        u8 b1 = bus_read(startLoc + (tileN * 16) + tileY);
        u8 b2 = bus_read(startLoc + (tileN * 16) + tileY + 1);
        for (int bit = 7; bit >= 0; bit--) {
            u8 hi = !!(b1 & (1 << bit)) << 1;
            u8 lo = !!(b2 & (1 << bit));
            u8 color = hi | lo;
            rc.x = x + ((7 - bit) * scale);
            rc.y = y + (tileY / 2 * scale);
            rc.w = scale;
            rc.h = scale;
            SDL_FillRect(surface, &rc, colors[color]);
        }
    }
}

void update_dbg_win()
{
    int xDraw = 0;
    int yDraw = 0;
    int tileN = 0;

    SDL_Rect rc;
    rc.x = 0;
    rc.y = 0;
    rc.w = DbgScreen->w;
    rc.h = DbgScreen->h;
    SDL_FillRect(DbgScreen, &rc, 0xFF111111);

    u16 addr = 0x8000;

    // 24 x 16 Tiles
    for (int y = 0; y < 24; y++) {
        for (int x = 0; x < 16; x++) {
            display_tile(DbgScreen, addr, tileN, xDraw + (x * scale), yDraw + (y * scale));
            xDraw += (8 * scale);
            tileN++;
        }
        yDraw += (8 * scale);
        xDraw = 0;
    }
    SDL_UpdateTexture(sdlDbgTexture, NULL, DbgScreen->pixels, DbgScreen->pitch);
    SDL_RenderClear(sdlDbgRenderer);
    SDL_RenderCopy(sdlDbgRenderer, sdlDbgTexture, NULL, NULL);
    SDL_RenderPresent(sdlDbgRenderer);
}

void ui_update() 
{
    SDL_Rect rc;
    rc.x = rc.y = 0;
    rc.w = rc.h = 2048;

    u32 *video_buffer = ppu_get_ctx()->vbuffer;

    for (int line_num = 0; line_num < YRES; line_num++) {
        for (int x = 0; x < XRES; x++) {
            rc.x = x * scale;
            rc.y = line_num * scale;
            rc.w = scale;
            rc.h = scale;

            SDL_FillRect(screen, &rc, video_buffer[x + (line_num * XRES)]);
        }
    }

    SDL_UpdateTexture(sdlTexture, NULL, screen->pixels, screen->pitch);
    SDL_RenderClear(sdlRenderer);
    SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, NULL);
    SDL_RenderPresent(sdlRenderer);

    //update_dbg_win();
}

void ui_on_key(bool down, u32 key_code)
{
    switch(key_code) {
        case SDLK_z: input_get_state()->b = down; break;
        case SDLK_x: input_get_state()->a = down; break;
        case SDLK_RETURN: input_get_state()->start = down; break;
        case SDLK_TAB: input_get_state()->select = down; break;
        case SDLK_UP: input_get_state()->up = down; break;
        case SDLK_DOWN: input_get_state()->down = down; break;
        case SDLK_LEFT: input_get_state()->left = down; break;
        case SDLK_RIGHT: input_get_state()->right = down; break;
    }
}

void ui_handle_event()
{
    SDL_Event e;
    while (SDL_PollEvent(&e) > 0) {
        if (e.type == SDL_KEYDOWN) {
            ui_on_key(true, e.key.keysym.sym);
        }
        if (e.type == SDL_KEYUP) {
            ui_on_key(false, e.key.keysym.sym);
        }        
        if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_CLOSE) {
            emu_get_context()->die = true;
        }
    }
    
}
