#ifndef __RJE_VGA_H__
#define __RJE_VGA_H__

#include "rje_def.h"

// Setting up our graphics system
void rje_setupGraphics(void);
void rje_destructGraphics(void);

// Utilites for double-buffering
// 	(Note that presentBackBuffer will wait for a retrace)
void rje_presentBackBuffer(void); 
void rje_waitForRetrace(void);

// Clears the back-buffer to the specified color
void rje_fillScreen(byte color);

// Drawing standard primitives
void rje_drawPixel(word x, word y, byte color);
void rje_drawLine(word startX, word startY, word endX, word endY, byte color);
void rje_drawHorizontalLine(word x, word y, word width, byte color);
void rje_drawVerticalLine(word x, word y, word height, byte color);
void rje_drawRectangle(word x, word y, word width, word height, byte color);
void rje_drawFilledRectangle(word x, word y, word width, word height, byte color);

#endif // __RJE_VGA_H__
