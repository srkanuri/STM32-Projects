/* screen.c --- 
 * 
 * Filename: screen.c
 * Description: 
 * Author: Bryce Himebaugh
 * Maintainer: 
 * Created: Mon Aug 11 11:02:01 2014
 * Last-Updated: 
 *           By: 
 *     Update #: 0
 * Keywords: 
 * Compatibility: 
 * 
 */

/* Commentary: 
 * 
 * 
 * 
 */

/* Change log:
 * 
 * 
 */

/* Copyright (c) 2004-2007 The Trustees of Indiana University and 
 * Indiana University Research and Technology Corporation.  
 * 
 * All rights reserved. 
 * 
 * Additional copyrights may follow 
 */

/* Code: */

#include "screen.h"

const int cw = 5;
const int ch = 8;

extern uint8_t ASCII[];

/*fills the screen with a color*/

void fillScreen(uint16_t color) {
  uint8_t x,y;
  uint16_t buf[ST7735_width];
  ST7735_setAddrWindow(0, 0, ST7735_width-1, ST7735_height-1, MADCTLGRAPHICS);
  for (y=0; y < ST7735_height; y++) {
    for (x=0; x < ST7735_width; x++) {
      buf[x] = color;
    }
    ST7735_pushColor(buf,ST7735_width);
  }
}
/*draws a character. takes x and y coordinate, the character to draw and the color of the character and background color*/
void drawChar(uint8_t x, uint8_t y, unsigned char c, uint16_t color, uint16_t background) {
  int i, j;

  uint16_t buf[cw * ch];

  ST7735_setAddrWindow(x, y, x+cw-1, y+ch-1, MADCTLGRAPHICS);

  for (i = 0; i < cw*ch; i++)
    ((uint16_t *)buf)[i] = background;

  for (i = 0; i < cw; i++) {
    uint8_t byte = ASCII[c*cw + i];
    for (j = 0; j < ch ; j++){
      if (byte & 1)
	buf[j*cw  + i] =  color;
      byte >>= 1;
    }
  }
  ST7735_pushColor(buf,cw*ch);
}
/*draws the string using draw character*/
void drawString(uint8_t x, uint8_t y, char *c, uint16_t color, uint16_t background) {
  while (c && *c) {
    drawChar(x, y, *c++, color, background);
    x += cw + 1;
    if (x + cw >= ST7735_width) {
      y += ch+2;
      x = 0;
    }
  }
}
/*draws a rectangle with specified coordinates*/
void drawRect(uint8_t x, uint8_t y, uint8_t width, uint8_t depth, uint16_t color) {
  int i;
  uint16_t buf[width*depth];
  ST7735_setAddrWindow(x, y, x+width-1, y+depth-1, MADCTLGRAPHICS);
  for (i = 0; i < width*depth; i++) {
    buf[i] = color;
  }
  ST7735_pushColor(buf,width*depth);
}






/* screen.c ends here */
