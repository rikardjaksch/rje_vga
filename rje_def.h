#ifndef __RJE_DEF_H__
#define __RJE_DEF_H__

typedef enum {false, true} 	bool;
typedef unsigned char 		byte;
typedef unsigned short 		word;

#define WINDOW_WIDTH 320
#define WINDOW_HEIGHT 200
#define NUMBER_OF_COLORS 256
#define WINDOW_SIZE WINDOW_WIDTH * WINDOW_HEIGHT
#define HALF_WINDOW_SIZE WINDOW_SIZE / 2
#define FOURTH_WINDOW_SIZE WINDOW_SIZE / 4

#define rje_sgn(x) ((x < 0) ? -1 : ((x > 0) ? 1 : 0))
#define rje_abs(x) ((x)<0 ? -(x) : (x))

#endif // __RJE_DEF_H__
