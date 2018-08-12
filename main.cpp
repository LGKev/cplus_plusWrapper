#include <epd1in54.hpp>
#include "epd_wrapper.h"
#include "paint_wrapper.h"
#include "font24.c"
//#include "epdif.h"  this is the inteface for spi, replace this with my commands with same header?
#include "ePaper.h" //has my defines and there, names should be similar...

// need to copy over the SendCommand, SendData





// TODO define these:
/*

	init? I have my own init.
	Reset,
	WaitUntilIdle

	Delay
	DigitalWrite?/ DigitalRead?
*/







uint8_t imageA[1024];

void main(void) {

    //dissable watchdog ...
    // other usual set up

// Setup
    struct ePD* ePaperDisplay = EPDNewEPD();
    struct gfx* gfx= GFXPaintNewPaint(imageA, 200, 200);

    uint8_t doOnce = 1;
    if(doOnce == 1) {
        EPDInit(ePaperDisplay, lut_full_update); //look up table for partial update?
        EPDClearFrameMemory(ePaperDisplay, 0xFF);
        EPDDisplayFrame(ePaperDisplay);
        EPDClearFrameMemory(ePaperDisplay, 0xFF);
        EPDDisplayFrame(ePaperDisplay);

        doOnce = 0;
        }


    // now need to do the paint section
    GFXPaintSetWidth(gfx, 32);
    GFXPaintSetHeight(gfx, 96);
    GFXPaintSetRotate(gfx, ROTATE_270);
    GFXPaintClear(gfx, UNCOLORED);
    GFXPaintDrawStringAt(gfx, 0, 4, "kevin!", &Font24, COLORED);

    EPDSetFrameMemory(ePaperDisplay, GFXPaintGetImage(gfx), 80, 72, GFXPaintGetWidth(gfx), GFXPaintGetHeight(gfx));
    EPDDisplayFrame(ePaperDisplay);
    void EPDSetFrameMemory(ePD* d, const uint8_t* image_buffer, uint8_t x, uint8_t y, uint8_t image_width, uint8_t image_height);

    }// end of main






