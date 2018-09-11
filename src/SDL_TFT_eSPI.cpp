#include "SDL_TFT_eSPI.h"

#include "freertos/task.h"

TFT_eSPI *tft;

void SDL_WM_SetCaption(const char *title, const char *icon)
{

}

void SDL_Delay(Uint32 ms)
{
    const TickType_t xDelay = ms / portTICK_PERIOD_MS;
    vTaskDelay( xDelay );
}

char *SDL_GetError(void)
{
    return (char *)"";
}

char *SDL_GetKeyName(SDLKey key)
{
    return (char *)"";
}

SDL_Keymod SDL_GetModState(void)
{
    return (SDL_Keymod)0;
}

Uint32 SDL_GetTicks(void)
{
    
}

int SDL_Init(Uint32 flags)
{
    
}

void SDL_Quit(void)
{

}

Uint32 SDL_WasInit(Uint32 flags)
{
    return (tft == NULL);
}

int SDL_InitSubSystem(Uint32 flags)
{
    if(flags == SDL_INIT_VIDEO)
    {
        tft = new TFT_eSPI();
        tft->init();
        tft->setRotation(1);
        tft->fillScreen(TFT_BLACK);
    }
    return 0; // 0 = OK, -1 = Error
}

SDL_Surface *SDL_CreateRGBSurface(Uint32 flags, int width, int height, int depth, Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask)
{
    SDL_Surface* surface = new SDL_Surface
        {flags, 
        (SDL_PixelFormat *){NULL}, // format
        width, height, 
        (Uint16)depth, 
        (void *)NULL,   // pixels
        {0,0,width, height}, 
        1,      // ref count
        new TFT_eSprite(tft)
        };
        
    if(flags == SDL_SWSURFACE)
    {
        surface->sprite->setColorDepth(depth);
        surface->sprite->createSprite(width, height);
        surface->pixels = surface->sprite->frameBuffer(0);
    }
    return surface;
}

int SDL_FillRect(SDL_Surface *dst, SDL_Rect *dstrect, Uint32 color)
{
    if(dst == NULL || dst->sprite == NULL)
    {
        // Draw directly on screen
        tft->fillRect(dstrect->x, dstrect->y, dstrect->w, dstrect->h, color);
    } else {
        // Draw on surface
        dst->sprite->fillRect(dstrect->x, dstrect->y, dstrect->w, dstrect->h, color);
    }
    return 0;
}

SDL_Surface *SDL_GetVideoSurface(void)
{
    SDL_Surface* surface = new SDL_Surface
        {SDL_SWSURFACE, 
        (SDL_PixelFormat *){NULL}, // format
        TFT_WIDTH, TFT_HEIGHT, 
        (Uint16)16, 
        (void *)NULL,   // pixels
        {0, 0, TFT_WIDTH, TFT_HEIGHT}, 
        1,      // ref count
        (TFT_eSprite *){NULL}
        };
    return surface;
}

Uint32 SDL_MapRGB(SDL_PixelFormat *fmt, Uint8 r, Uint8 g, Uint8 b)
{
    if(fmt->BitsPerPixel == 16)
    {
        uint16_t bb = (b >> 3) & 0x1f;
        uint16_t gg = ((g >> 2) & 0x3f) << 5;
        uint16_t rr = ((r >> 3) & 0x1f) << 11;
        return (Uint32) (rr | gg | bb);        
    }
    return (Uint32)0;
}

int SDL_SetColors(SDL_Surface *surface, SDL_Color *colors, int firstcolor, int ncolors)
{

}

SDL_Surface *SDL_SetVideoMode(int width, int height, int bpp, Uint32 flags)
{

}

void SDL_FreeSurface(SDL_Surface *surface)
{

}

void SDL_QuitSubSystem(Uint32 flags)
{

}

int SDL_Flip(SDL_Surface *screen)
{

}

int SDL_VideoModeOK(int width, int height, int bpp, Uint32 flags)
{
    
}