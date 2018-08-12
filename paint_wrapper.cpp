/*
 * paint_wrapper.cpp
 *
 *  Created on: Aug 11, 2018
 *      Author: kkuwata
 */



#include "epd1in54.h"
#include "paint_wrapper.h"

extern "C" {

gfx* paint = GFXPaintNewPaint(gfx* paint, uint8_t* image, uint8_t width, uint8_t height){
	return new Paint(image, width, height);
}	


void GFXPaintDrawAbsolutePixel(gfx* paint,  uint8_t x, uint8_t y, uint8_t colored){
	paint->DrawAbsolutePixel(x,y,colored);	
}

uint8_t* GFXPaintGetImage(gfx* paint){
	return paint-> GetImage();
}

uint8_t GFXPaintGetWidth(gfx* paint){
	return paint->GetWidth();
}

uint8_t GFXPaintGetHeight(gfx* paint){
	return paint->GetHeight();
}

void GFXPaintSetWidth(gfx* paint, uint8_t width){
	 paint->SetWidth(width);
}

void GFXPaintSetHeight(gfx* paint, uint8_t height){
	 paint->SetHeight(height);
}

uint8_t GFXPaintGetRotate(gfx* paint){
	return paint->GetRotate();	
}

void GFXPaintSetRotate(gfx* paint, uint8_t rotate){
	paint->SetRotate(rotate);
}

void GFXPaintDrawPixel(gfx* paint, uint8_t x, uint8_t y, uint8_t colored){
	paint->DrawPixel(x,y,colored);
}

void GFXPaintDrawCharAt(gfx* paint, uint8_t x, uint8_t y, char ascii_char, sFONT* font, uint8_t colored){
	paint->DrawCharAt(x,y,ascii_char, font, colored);
}

void GFXPaintDrawStringAt(gfx* paint, uint8_t x, uint8_t y, const char* text, sFONT* font, uint8_t colored){
	paint->DrawStringAt(x, y, text, font, colored);
}

void GFXPaintDrawLine(gfx* paint, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t colord){
	paint->DrawLine(x0, y0, x1, y1);
}

void GFXPaintDrawHorizontalLine(gfx* paint, uint8_t x, uint8_t y, uint8_t lineWidth, uint8_t colored){
	paint->DrawHorizontalLine(x, y, lineWidth, colored);
}

void GFXPaintDrawVerticalLine(gfx* paint, uint8_t x, uint8_t y, uint8_t lineHeight, uint8_t colored){
	paint->DrawVerticalLine(x, y, lineHeight, colored);
}


void GFXPaintDrawRectangle(gfx* paint, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t colored){
	paint->DrawRectangle(x0, y0, x1, y1, colored);
}

void GFXPaintDrawFillRectangle(gfx* paint, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t colored){
	paint->DrawFillRectangle(x0, y0, x1, y1, colored);
} 

void GFXPaintDrawCircle(gfx* paint, uint8_t x, uint8_t y, uint8_t radius, uint8_t colored){
	paint->DrawCircle(x, y, radius, colored);
}

void GFXPaintDrawFilledCircle(gfx* paint, uint8_t x, uint8_t y, uint8_t radius, uint8_t colored){
	paint->DrawFilledCircle(x, y, radius, colored);
}

}//end extern "c"


