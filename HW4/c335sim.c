#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <SDL/SDL.h>
#include <SDL/SDL_rotozoom.h>
#include <c335sim.h>
/* #include "ff.h" */
/* #include "diskio.h" */


enum ORIENT { portrait = 0, landscape = 1 };

static SDL_Surface* screen = NULL;
static SDL_Surface* window = NULL;
static enum ORIENT orientation = portrait;
static int madx, mady, madv = 0;
static int xindx, yindx = 0;
static int xoff, yoff = 0;
static SDL_Rect  destRect;

#define SDL_main main

static zoom = 2;

static int half = 0;

static Uint32 signal_handler(Uint32 interval, void *param)
{
  SDL_Flip(screen);
  return interval;
}

static void init_screen(enum ORIENT landscape)
{
  if (landscape) {
    screen = SDL_SetVideoMode(ST7735_height * zoom, ST7735_width * zoom, 24, SDL_SWSURFACE);
    orientation = landscape;
  }
  else {
    screen = SDL_SetVideoMode(ST7735_width * zoom, ST7735_height * zoom, 24, SDL_SWSURFACE);
    orientation = portrait;
  }
}

static void draw_screen(SDL_Surface *src)
{
  SDL_Surface *tmp = rotozoomSurface(src, 90.0 * orientation, (float) zoom , SMOOTHING_ON);
  SDL_BlitSurface(tmp, 0, screen, &destRect);
  SDL_FreeSurface(tmp);
}

void Delay(int millis)
{
  SDL_Delay(millis);
}

int SDL_main(int argc, char *argv[])
{

  int result;

  //Start SDL

  //  SDL_Init( SDL_INIT_EVERYTHING );
  SDL_Init( SDL_INIT_TIMER |
	    SDL_INIT_VIDEO | 
	    SDL_INIT_JOYSTICK);

  //Set up screen

  init_screen(portrait);
  // init_screen(landscape);

  SDL_AddTimer(50, signal_handler, 0);

  result = c335_main(argc, argv);
  SDL_Quit();

  return result;
}

void ST7735_setAddrWindow(uint16_t xs, uint16_t ys,
			  uint16_t xe, uint16_t ye,
			  uint8_t madctl)
{

  int w = xe-xs+1;  // 7735 xs..xe  inclusive
  int h = ye-ys+1;  //      ys..ye  inclusive

  if (window)
    SDL_FreeSurface(window);
  window = SDL_CreateRGBSurface(0, w, h , 16, 0xF800, 0x07E0, 0x001F, 0);

  mady = (madctl >> 2) & 1;
  madx = ((madctl >> 1) & 1);
  madv = madctl & 1;
  xoff = xs*zoom;
  yoff = ys*zoom;

  xindx = madx ? 0:w-1;
  yindx = mady ? 0:h-1;

  destRect.x = xs*zoom;
  destRect.y = ys*zoom;

  //  printf("set addr, %d %d %d %d\n", x0, y0, x1, y1);
}

static void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
  int bpp = surface->format->BytesPerPixel;
  /* Here p is the address to the pixel we want to set */
  Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
  Uint16 tmp;

  switch(bpp) {
  case 1:
    *p = pixel;
    break;

  case 2:
    tmp =  ((pixel&BLUE) >> 11) | ((pixel&RED) << 11) | (pixel&GREEN);
    *(Uint16 *)p = tmp;
    break;

  case 3:
    if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
      p[0] = pixel & 0xff;
      p[1] = (pixel >> 8) & 0xff;
      p[2] = (pixel >> 16) & 0xff;
    } else {
      p[0] = (pixel >> 16) & 0xff;
      p[1] = (pixel >> 8) & 0xff;
      p[2] = pixel & 0xff;
    }
    break;

  case 4:
    *(Uint32 *)p = pixel;
    break;
  }
}


void ST7735_pushColor(uint16_t *color, int cnt)
{
  uint16_t *pixels = window->pixels;
  int w = window->w;
  int h = window->h;

  int xinc = madx ? 1 : -1;
  int yinc = mady ? 1 : -1;

  SDL_LockSurface(window);

  while (cnt--)
    {
      // need madv case

      putpixel(window, xindx, yindx, *color);
      color++;
      xindx += xinc;
      if (xindx == w)
	{
	  xindx = 0;
	  yindx += yinc;
	}
      if (xindx < 0)
	{
	  xindx = w - 1;
	  yindx += yinc;
	}
      if (yindx == h)
	yindx = 0;
      if (yindx < 0)
	yindx = h - 1;

    }

  SDL_UnlockSurface(window);
  draw_screen(window);
}
