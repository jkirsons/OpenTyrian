#ifndef SDL_TFT_eSPI_H
#define SDL_TFT_eSPI_H
#include <TFT_eSPI.h>
#include "SDL.h"

#define SDL_INIT_VIDEO 1

#define SDL_SWSURFACE 1
#define SDL_HWSURFACE 2

#define SDL_HWPALETTE 1
#define SDL_FULLSCREEN 1


typedef struct{
  Uint8 r;
  Uint8 g;
  Uint8 b;
  Uint8 unused;
} SDL_Color;

typedef struct{
  int ncolors;
  SDL_Color *colors;
} SDL_Palette;

typedef struct SDL_PixelFormat {
  SDL_Palette *palette;
  Uint8  BitsPerPixel;
  Uint8  BytesPerPixel;
  Uint8  Rloss, Gloss, Bloss, Aloss;
  Uint8  Rshift, Gshift, Bshift, Ashift;
  Uint32 Rmask, Gmask, Bmask, Amask;
  Uint32 colorkey;
  Uint8  alpha;
} SDL_PixelFormat;

typedef struct{
  Sint16 x, y;
  Uint16 w, h;
} SDL_Rect;

typedef struct SDL_Surface {
        Uint32 flags;                           /* Read-only */
        SDL_PixelFormat *format;                /* Read-only */
        int w, h;                               /* Read-only */
        Uint16 pitch;                           /* Read-only */
        void *pixels;                           /* Read-write */

        /* clipping information */
        SDL_Rect clip_rect;                     /* Read-only */

        /* Reference count -- used when freeing surface */
        int refcount;                           /* Read-mostly */
        TFT_eSprite* sprite;

	/* This structure also contains private fields not shown here */
} SDL_Surface;

typedef int SDLKey;

void SDL_WM_SetCaption(const char *title, const char *icon);
void SDL_Delay(Uint32 ms);
Uint32 SDL_WasInit(Uint32 flags);
int SDL_Init(Uint32 flags);
int SDL_InitSubSystem(Uint32 flags);
char *SDL_GetError(void);
int SDL_FillRect(SDL_Surface *dst, SDL_Rect *dstrect, Uint32 color);
SDL_Surface *SDL_CreateRGBSurface(Uint32 flags, int width, int height, int depth, Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask);
char *SDL_GetKeyName(SDLKey key);
Uint32 SDL_GetTicks(void);
SDL_Keymod SDL_GetModState(void);
void SDL_Quit(void);
SDL_Surface *SDL_GetVideoSurface(void);
Uint32 SDL_MapRGB(SDL_PixelFormat *fmt, Uint8 r, Uint8 g, Uint8 b);
int SDL_SetColors(SDL_Surface *surface, SDL_Color *colors, int firstcolor, int ncolors);
SDL_Surface *SDL_SetVideoMode(int width, int height, int bpp, Uint32 flags);
void SDL_FreeSurface(SDL_Surface *surface);
void SDL_QuitSubSystem(Uint32 flags);
int SDL_Flip(SDL_Surface *screen);
int SDL_VideoModeOK(int width, int height, int bpp, Uint32 flags);

#endif
