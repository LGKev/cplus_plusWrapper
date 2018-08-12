/*
 * epd_wrapper.cpp
 *
 *  Created on: Aug 11, 2018
 *      Author: kkuwata
 */



#include "epd_wrapper.h"
#include "epd1in54.h"

//include the driver for spi /epaper

extern "C" {

ePD* EPDNewEPD(void){
	return new Epd();
}

// destructor?



void EPDInit(ePD* d, const uint8_t* lut){ //initialize the EPD object
	d->lut = lut; 
	Reset();
 
	SendCommand(DRIVER_OUTPUT_CONTROL);
	SendData((EPD_HEIGHT - 1) & 0xFF);
	SendData(((EPD_HEIGHT - 1) >> 8) & 0xFF);
	SendData(0x00);                     // GD = 0; SM = 0; TB = 0;
	SendCommand(BOOSTER_SOFT_START_CONTROL);
	SendData(0xD7);
	SendData(0xD6);
	SendData(0x9D);
	SendCommand(WRITE_VCOM_REGISTER);
	SendData(0xA8);                     // VCOM 7C
	SendCommand(SET_DUMMY_LINE_PERIOD);
	SendData(0x1A);                     // 4 dummy lines per gate
	SendCommand(SET_GATE_TIME);
	SendData(0x08);                     // 2us per line
	SendCommand(DATA_ENTRY_MODE_SETTING);
	SendData(0x03);                     // X increment; Y increment
	SetLut(lut);
	/* EPD hardware init end */
	return 0;

}


void EPDSendCommand( uint8_t command){
// include my implentation for spi send data/command	
}
	
void EPDSendData( uint8_t data){
// TODO: include implementation for spi command/data
}


void EPDWaitUntilIdle(ePD* d){
	d->WaitIntilIdle();
}


void EPDReset(ePD* d){
	d->Reset();
}


void EPDSetFrameMemory(ePD* d, const uint8_t* image_buffer, uint8_t x, uint8_t y, uint8_t image_width, uint8_t image_height){
	d->SetFrameMemory(x,y,image_width, image_height); 
}



void EPDSetFrameMemoryFull(ePD* d, const uint8_t* image_buffer){
//overloading done correctly?
// does entire screen not just at location
	d->SetFrameMemory(image_buffer);	
}


void EPDSleep(ePD* d){
	d->Sleep();
}

 
void EPDClearFrameMemory(ePD* d){
	d->ClearFrameMemory();
}

void EPDDisplayFrame(ePD* d){
	d->DisplayFrame();
}




} 
