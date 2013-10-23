#include "rje_vga.h"
#include <memory.h>
#include <malloc.h>

char old_video_mode = 255;

// Main video memory.
byte far* VGA = (byte far*)0xA0000000L;	

// Back-buffer
byte* BACK_BUFFER = 0;	

void rje_setupGraphics(void) {
	_asm {
		// Store current video-mode so that we can restore it later
		mov		ah, 0x0F
		int 	0x10
		mov 	old_video_mode, al

		// Go into mode13h (320x200x256)
		xor		ah, ah
		mov		al, 0x13
		int 	0x10
	}

	// Should we double-check here if we really made it to mode13h?
	BACK_BUFFER = (byte*)malloc(64000L);
}

void rje_destructGraphics(void) {
	free(BACK_BUFFER);
	BACK_BUFFER = 0;

	_asm {
		cmp 	old_video_mode, 255
		jz		reset_old_mode

	set_regular_text_mode:
		xor 	ah, ah
		mov 	al, 0x03
		int 	0x10
		jmp 	destruction_finished

	reset_old_mode:
		xor 	ah, ah
		mov 	al, old_video_mode
		int 	0x10

	destruction_finished:
	}
}

void rje_presentBackBuffer(void) {
	_asm {
		// Wait for vertical retrace
		mov 	dx, 0x03DA
	wait1:
		in 		al, dx
		and 	al, 0x08
		jnz 	wait1
	wait2:
		in 		al, dx
		and 	al, 0x08
		jz 		wait2

		// Blit backbuffer to vga-buffer, qword at a time
		push	ds
		mov		cx, FOURTH_WINDOW_SIZE	// Size of back-buffer in dwords
		les		di, VGA					// es:di is destination of memory move
		lds 	si, BACK_BUFFER 		// ds:si is source of memory move
		cld								// Make sure to move in the right direction
		rep		movsd					// Move all the data (dwords)
		pop 	ds
	}
}

void rje_waitForRetrace(void) {
	_asm {
		// Wait for vertical retrace
		mov 	dx, 0x03DA
	wait1:
		in 		al, dx
		and 	al, 0x08
		jnz 	wait1
	wait2:
		in 		al, dx
		and 	al, 0x08
		jz 		wait2
	}
}

void rje_fillScreen(byte color) {
	_asm {
		cld
		les		di, BACK_BUFFER			// es:di is target for memory set
		mov		cx, FOURTH_WINDOW_SIZE	// Size of back-buffer in qwords
		mov 	bl, color				// color is a byte, but we want to store it in a qword.
		mov 	al, bl 					// So use some bitwise magic
		mov 	bh, bl 					// 	Example
		mov   	ah, bl 					//		if byte is 12, qword will be 12121212
		shl 	ebx, 16 				//
		or 		eax, ebx 				//
		rep		stosd					// while (cx != 0) { ((es::di) += 4) = eax; cx--; }
	}
}

void rje_drawPixel(word x, word y, byte color) {
	BACK_BUFFER[(y << 8) + (y << 6) + x] = color;
}

void rje_drawLine(word startX, word startY, word endX, word endY, byte color) {
	int i, dx, dy, sdx, sdy, dxabs, dyabs, x, y, px, py;
	dx = endX - startX;
	dy = endY - startY;
	dxabs = rje_abs(dx);
	dyabs = rje_abs(dy);
	sdx = rje_sgn(dx);
 	sdy = rje_sgn(dy);
 	x = dyabs >> 1;
	y = dxabs >> 1;
	px = startX;
	py = startY;

	rje_drawPixel(px, py, color);

	if (dxabs >= dyabs) {
		for (i = 0; i < dxabs; i++) {
			y += dyabs;
			if (y >= dxabs) {
				y -= dxabs;
				py += sdy;
			}
			px += sdx;
			rje_drawPixel(px, py, color);
		}
	} else {
		for(i = 0; i < dyabs; i++) {
			x += dxabs;
			if (x >= dyabs) {
				x -= dyabs;
				px += sdx;
			}
			py += sdy;
			rje_drawPixel(px, py, color);
		}
	}
}

void rje_drawHorizontalLine(word x, word y, word width, byte color) {
	// Very basic clipping towards bottom, should move to asm code with full clipping
	if (x + width > WINDOW_WIDTH) {
		width = WINDOW_WIDTH - x;
	}

	_asm {
		les		di, BACK_BUFFER
		mov 	ax, y 					// ax = y
		mov 	bx, ax
		sal 	ax, 6
		sal		bx, 8
		add 	ax, bx					// ax = (y * 320)
		mov 	bx, x 					// bx = x
		add 	ax, bx					// ax = (y * 320 + x)
		add 	di, ax					// di = BACK_BUFFER[y * 320 + x]
		mov 	bh, color
		mov 	cx, width   
	cont_h_line:
		mov 	es:[di], bh 			// di = color
		inc 	di                  	// di++
		loop cont_h_line
	}
}

void rje_drawVerticalLine(word x, word y, word height, byte color) {
	// Very basic clipping towards bottom, should move to asm code with full clipping
	if (y + height > WINDOW_HEIGHT) {
		height = WINDOW_HEIGHT - y;
	}

	_asm {
		les		di, BACK_BUFFER
		mov 	ax, y
		mov 	bx, ax
		sal 	ax, 6
		sal		bx, 8
		add 	ax, bx
		mov 	bx, x
		add 	ax, bx;
		add 	di, ax
		mov 	cx, height
		inc 	cx
		mov 	bh, color
		mov 	ax, 320
	cont_v_line:
		mov 	es:[di], bh
		add 	di, ax
		loop cont_v_line
	}
}

void rje_drawRectangle(word x, word y, word width, word height, byte color) {
	rje_drawHorizontalLine(x, y, width, color);
	rje_drawHorizontalLine(x, y + height, width, color);
	rje_drawVerticalLine(x, y, height, color);
	rje_drawVerticalLine(x + width, y, height, color);
}

void rje_drawFilledRectangle(word x, word y, word width, word height, byte color) {
	int i = 0;
	for (i = 0; i < height; ++i) {
		rje_drawHorizontalLine(x, y + i, width, color);
	}
}
