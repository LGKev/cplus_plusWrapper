/*
 * paint_wrapper.h
 *
 *  Created on: Aug 11, 2018
 *      Author: kkuwata
 */

#ifndef PAINT_WRAPPER_H_
#define PAINT_WRAPPER_H_

#include "msp.h"
#include "fonts.h"


#ifdef __cplusplus
extern "C" {
#endif

	typedef struct gfx gfx;

	gfx* GFXPaintNewPaint(uint8_t* image, uint8_t width, uint8_t height);

	void GFXPaintClear(gfx* paint, uint8_t colored);
	uint8_t  GFXPaintGetWidth(gfx* paint);
	void GFXPaintSetWidth(gfx* paint, uint8_t width);
	uint8_t  GFXPaintGetHeight(gfx* paint);
	void GFXPaintSetHeight(gfx* paint, uint8_t height);
	uint8_t  GFXPaintGetRotate(gfx* paint);
	void GFXPaintSetRotate(gfx* paint, uint8_t rotate);
	uint8_t* GFXPaintGetImage(gfx* paint);
	void GFXPaintDrawAbsolutePixel(gfx* paint, uint8_t x, uint8_t y, uint8_t colored);
	void GFXPaintDrawPixel(gfx* paint, uint8_t x, uint8_t y, uint8_t colored);
	void GFXPaintDrawCharAt(gfx* paint, uint8_t x, uint8_t y, char ascii_char, sFONT* font, uint8_t colored);
	void GFXPaintDrawStringAt(gfx* paint, uint8_t x, uint8_t y, const char* text, sFONT* font, uint8_t colored);
	void  GFXPaintDrawLine(gfx* paint, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t colored);
	void  GFXPaintDrawHorizontalLine(gfx* paint, uint8_t x, uint8_t y, uint8_t width, uint8_t colored);
	void  GFXPaintDrawVerticalLine(gfx* paint, uint8_t x, uint8_t y, uint8_t height, uint8_t colored);
	void  GFXPaintDrawRectangle(gfx* paint, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t colored);
	void  GFXPaintDrawFilledRectangle(gfx* paint, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t colored);
	void  GFXPaintDrawCircle(gfx* paint, uint8_t x, uint8_t y, uint8_t radius, uint8_t colored);
	void  GFXPaintDrawFilledCircle(gfx* paint, uint8_t x, uint8_t y, uint8_t radius, uint8_t colored);



#ifdef __cplusplus
}
#endif

#endif /* PAINT_WRAPPER_H_ */
