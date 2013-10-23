#include <stdlib.h>
#include <conio.h>
#include "rje_vga.h"

int main(void) {	
	int x, y, color, x1, width, y1, height;
	unsigned short i;
	bool done = false;
	
	srand(12345);

	rje_setupGraphics();

	getch();

	while (done == false) {
		rje_fillScreen(15);	
		color = rand() % NUMBER_OF_COLORS;

		rje_drawFilledRectangle(10, 10, 100, 100, color);
		rje_drawFilledRectangle(210, 10, 100, 100, color);
		rje_drawFilledRectangle(10, 140, 300, 50, color);

		rje_presentBackBuffer();

		if (kbhit()) {
			if (getch() == 27) {
				done = true;
			}
		}
	}
	
	rje_destructGraphics();
	return 0;
}
