/*
 * epd_wrapper.h
 *
 *  Created on: Aug 11, 2018
 *      Author: kkuwata
 */

#ifndef EPD_WRAPPER_H_
#define EPD_WRAPPER_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ePD ePD;

#include "msp.h"

ePD* EPDNewEPD(void);//

void EPDInit(ePD* d, const uint8_t* lut); //initialize the EPD object

void EPDSendCommand(ePD* d, uint8_t command);

void EPDSendData(ePD* d, uint8_t data);

void EPDWaitUntilIdle(ePD* d);//

void EPDReset(ePD* d); //

void EPDSetFrameMemory(ePD* d, const uint8_t* image_buffer, uint8_t x, uint8_t y, uint8_t image_width, uint8_t image_height);

void EPDSetFrameMemoryFull(ePD* d, const uint8_t* image_buffer); // does entire screen not just at location

void EPDClearFrameMemory(ePD* d, uint8_t colored);

void EPDDisplayFrame(ePD* d);

void EPDSleep(ePD* d);


/*
 * @name: sendCommand(byte command)
 * @brief: Sends a command byte to the ePaper
 *              - toggles the CS pin to be low
 *              - sets the command pin to low
 * @inputs: a valid command found in the data sheet and the header file
 * */
void SendCommand(uint8_t command);


/*
 * @name: sendData(byte data)
 * @brief: Sends a command byte to the ePaper
 *              - toggles the CS pin to be low
 *              - sets the command pin to HIGH
 * @inputs: send bytes of data to the ePaper
*/
void SendData(uint8_t data);






//do I do the private functions too? I don't think so..

#ifdef __cplusplus
}
#endif

#endif /* EPD_WRAPPER_H_ */
