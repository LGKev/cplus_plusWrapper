/*
 * pauint8_t_wrapper.h
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

gfx* GFXPaintNewPaint(int8_t* image, uint8_t width, uint8_t height);
   
void GFXPauint8_tClear(gfx* pauint8_t, uint8_t colored);
void GFXPauint8_tClear(gfx* pauint8_t, uint8_t colored);
void GFXPauint8_tClear(gfx* pauint8_t, uint8_t colored);
void GFXPauint8_tClear(gfx* pauint8_t, uint8_t colored);
void GFXPauint8_tClear(gfx* pauint8_t, uint8_t colored);
uint8_t  GFXPauint8_tGetWidth(gfx* pauint8_t);
void GFXPauint8_tSetWidth(gfx* pauint8_t, uint8_t width);
uint8_t  GFXPauint8_tGetHeight(gfx* pauint8_t);
void GFXPauint8_tSetHeight(gfx* pauint8_t, uint8_t height);
uint8_t  GFXPauint8_tGetRotate(gfx* pauint8_t);
void GFXPauint8_tSetRotate(gfx* pauint8_t, uint8_t rotate);
unsigned char* GetImage(gfx* pauint8_t);
void GFXPauint8_tDrawAbsolutePixel(gfx* pauint8_t, uint8_t x, uint8_t y, uint8_t colored);
void GFXPauint8_tDrawPixel(gfx* pauint8_t, uint8_t x, uint8_t y, uint8_t colored);
void GFXPauint8_tDrawCharAt(gfx* pauint8_t, uint8_t x, uint8_t y, char ascii_char, sFONT* font, uint8_t colored);
void GFXPauint8_tDrawStringAt(gfx* pauint8_t, uint8_t x, uint8_t y, const char* text, sFONT* font, uint8_t colored);
void  GFXPauint8_tDrawLine(gfx* pauint8_t, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t colored);
void  GFXPauint8_tDrawHorizontalLine(gfx* pauint8_t, uint8_t x, uint8_t y, uint8_t width, uint8_t colored);
void  GFXPauint8_tDrawVerticalLine(gfx* pauint8_t, uint8_t x, uint8_t y, uint8_t height, uint8_t colored);
void  GFXPauint8_tDrawRectangle(gfx* pauint8_t, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t colored);
void  GFXPauint8_tDrawFilledRectangle(gfx* pauint8_t, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t colored);
void  GFXPauint8_tDrawCircle(gfx* pauint8_t, uint8_t x, uint8_t y, uint8_t radius, uint8_t colored);
void  GFXPauint8_tDrawFilledCircle(gfx* pauint8_t, uint8_t x, uint8_t y, uint8_t radius, uint8_t colored);



#ifdef __cplusplus
}
#endif

#endif /* PAINT_WRAPPER_H_ */
