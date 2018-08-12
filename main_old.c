/*
 * main.c
 * Author: Kevin Kuwata
 * Date: 8/11/18
 *
 *  I am trying to see if I can use Little VGL as my library.
 *
 *  I think I have the display drivers written, just needs to be registered. This file was copied from the epaper_basic project
 *
 * This file is to be paired with the hardware V2 watch. To develop the device, I will be using the launchpad as the main platform.
 * This means that the pins for the ePaper are going to be the pins that are readily available on the launchpad and not the PCB.
 * Differentiate which code by using the #defines PCB_FW and Launchpad_FW

    update: 8/5/18 how funny the date: month and day are switched, anyway....
I think that I can actually still do a partial update just by doing the set fram ememory. I wonder what clear does. or maybe clear is used in
the set fram memory? or display frame but either way set frame memory is constant2

my sendtTo display is the same as the arduino sketch so it has to be the other 3 functions called clear, string, or frame memory.

its probably the setFrame memory I currently  have it forced as 0,0 and full image refresh!
    and we dont have the clear function. draw string is equivalent of having a known image already.
        so drawstring takes a string, and converts it to a bitmap.

PAINT.cpp
==========
    this can set the area of the memory write, clear it with a color, just filling in the arraw with drawAbsolutePixel
    drawString eventually calls DrawAbsolute pixel, therefore just writing the array again.
    Clear Display, just makes the entire area of the pixel box blank. then it writes the new image to that same small area. then we do display. because everything
    is the same except what was newly written/cleared so only a small portion of the screen looks like it changed!


    Paint is definitly used to set the area in memory and how to clear it and how to write pixels and strings/primatives. to not use c++ I would have to make pass
    structs around with the info about like height and width. and then pass the frame buffer global.

    clear would take in a struct for the image, frame buffer, and x, y coordinates, and color. so it will flush only a specific area.

    implement: drawAbsolute pixel

    pretty much implement every function call for a C++ object and make them available in epaper.h and call them as they are requried.

    I'M THINKING A struct might be like image{
        width,
        hegiht,
        x,
        y,
    } image_t


I also think its pretty clear that with paint and epd.h this is pretty much alawys only doing partial updates.
or maybe its doing full, but because of the way clear works it still looks partial.

The look up table is for partial update. so this is a true implentation of a partial update!


    currently I do something like:
    clear entire screen
        show new array to screen
    clear entire screen
        show new array to screen

 ~ ~ ~~ ~ ~~ ~ ~~ ~ ~~ ~ ~~ ~ ~~ ~ ~~ ~ ~~ ~ ~~ ~ ~ I think I should try this instead:
    clear portion of the screen  (probably also changes to take in args for the x, y coordinates)
        update portion of screen (changes with the set area and memory)
        show entire screen (stays the same I think)


 */
#include "msp.h"
#include "ePaper.h"
#include "lvgl/lvgl.h"


/* GLOBAL ARRAY for Buffered Image */
uint8_t bufferedImage[1000]; //what is a valid size?
// wait we are using the virtual display buffer. VDB which is non zero in conf. so
//how do i modify my driver to write to that array?


/*
 * @name: initEpaper
 * @brief: will initialize GPIO and the SPI Pins to communicate to the ePaper Display.
 *              The SPI Pins are configured for Primary mode.
 * */
void initEpaper(void);

/*
 * @name: sendCommand(byte command)
 * @brief: Sends a command byte to the ePaper
 *              - toggles the CS pin to be low
 *              - sets the command pin to low
 * @inputs: a valid command found in the data sheet and the header file
 * */
void sendCommand(uint8_t command);



/*
 * @name: sendData(byte data)
 * @brief: Sends a command byte to the ePaper
 *              - toggles the CS pin to be low
 *              - sets the command pin to HIGH
 * @inputs: send bytes of data to the ePaper
 *
 * This is better suited for a single data byte instead of multiple say for instance you had to send the LUT
 * this would not work because CS would be toggling between transmits.
     TODO if this proves not correct to initialize the screen I will come back and make one where data is transmitted and CS stays low the entire time
     // looking at other examples of the driver it looks like some keep CS low the entire data transfer of LUT and others don't, ie let it toggle.
*/
void sendData(uint8_t data);

/*
 *  @name: void setLut(const unsigned char* lut)
 *  @brief: sends the look up table (LUT) to ePaper
 *  @input: partial or full update LUT ** provided by venord **
 * */
void setLUT(const unsigned char* lut);

/*
 * @name: setMemoryArea(uint8_t x_start, uint8_t y_start, uint8_t x_end, uint8_t y_end)
 * @brief: writes to the specified memory area ( x,y) start, and (xy) end
 * @inputs: x, y start, and xy end, consult an image of the specified area of the graphics display from data sheet
 * */
void setMemoryArea(uint8_t x_start, uint8_t y_start, uint8_t x_end, uint8_t y_end);


/*  @name: setMemoryPointer(uint8_t x, uint8_t y);
 *  @brief: sets the start location in memory
 *  @inputs: x, y starting location
 *
 * */
void setMemoryPointer(uint8_t x, uint8_t y);



/*
 * @name: void clearFrameMemory(unsigned char color) {
 * @brief: writes to the RAM by choosing a color, black or white, 1 or 0.
 * @inputs: color: 1 or 0
 * */
void clearFrameMemory(unsigned char color) ;


/*
 * @name: setFrameMemory(const unsigned char*);
 * @brief: loads wave share default image
 * @input constant unsigned char array (pixel data)
 * */
void setFrameMemory(const unsigned char* image_buffer);

/*
 *  @name: sendToDisplay()
 *  @brief: you must call this after updating the RAM otherwise nothing will be pushed to the display
 *  @inputs: none
 * */
void sendToDisplay(void);


/*
 *  @name: void waitNotBusy(void)
 *  @brief: checks the busy pin with a Port Read, holds us in a while loop until busy pin goes low.
 *  @input: none
 * */
void waitNotBusy(void);


/*
 * @name: void drawAbsolutePixel
 * @brief: places a pixel at the xy location into the global array
 * @ input, x, y coordinates, rotations are not accounted for here.
 * */
void drawAbsolutePixel(uint8_t x, uint8_t y, unsigned char color);

/*
 *  NOTE
 *  So I am trying to figure out how the process goes for displaying an image
 *  there is a global image array of bytes which represents the entire screen
 *      **************
 *      *            *
 *      *            *
 *      *            *
 *      *            *
 *      *            *
 *      *            *
 *      **************
 * Writing the massive array allows parts of the display to be updated without changing whats on the screen already
 *      (this gives the impression it never updated, but  it really only updated  a portion).
 *         I mean that instead of the entire screen flashing, only the part of the screen that changes gets cleared
 *         and updated
 *
 *
 * I am thinking of a watch face right now, maybe If i chop up the screen realestate for known values
 * like the battery meter, bluetooth connected, and all the things that would pretty much be constant.
 *
 * Then the center of the screen has time and date
 *
 *              **REMEMBER**
 *  Byte:     _ _ _ _     _ _ _ _
 *     1's are off _
 *     0's are on #
 *            0000 0000   => ########
 *            0001 0000   => ###_####
 *            0001 1000   => ##__####
 *
 *
 * It appears the flow is something like this:
 *          1) create a big ass array
 *          2) show the array to the screen (this represents the first screen)
 *          3) update new screen _in the array!
 *              a) clear the portion of the screen that needs to be updated
 *                  - requires us to know what the possible things to be shown (or at least height and width).
 *               b) put the pixel data into the array, know that 1 byte is 8 bits.
 *               4) load the newest part of the array to the RAM
 *                      a) somehow translate the array position to a position on the display
 *                      b) have to clear out just that area which has the newest data
 *                      c) update the ram
 *               5) finally display the ram to the screen, put ram data to the screen.
 *
 *
 *          one question I have is this row major or column major?
 *          I know that we are X+, y+ in terms of data entry
 *          I was reading somewhere about using a column for fonts.
 *          a byte is 8 bits... and 8 pixels
 *          200 pixels width
 *              200 pexles / 8 pixels/bits is 25 (like a 200yrd lmao). so 25 bytes determine a row and but there are stil 200 rows
 *              so how many bytes?
 *                  25 bytes/ row * 200 is 5k bytes (that seems insane, not 5k bits?!), nah thats right cuz think of the pixel bmp to lcd conversion array, endded up to be 5000 bytes
 *
 *              (0,0) is the origin, I would expect that to be the 0th index, of the array
 *              (1,0); is 1 over in the X and so do i expect that to be the 0th element ... well yea cuz its an array of bytes.
 *                  so taht means I wouldn't expect to see index to increment until its a multiple of 8.
 *             so
 *             points (0,0) through (0,7) are the first byte
 *             and points (0,0) through 25 next bytes is the first row (0,200);
 *
 *             so I am imaginging an array that is 25 width, and 200 height.
 *              or a table, 25 columns, 200 rows.
 *
 *
 *              lets analyze the arduino code:
 *                          image[(x + y * this->width) / 8] |= 0x80 >> (x % 8);            translates: image[x + y*25]
 *
 *                               image is the global array, x and y is the coordinates of the point.
 *                               lets say we want to light up (1,1) we would expect the index to be 0.
 *                                  1 + 1*200 / 8 ===>  26   but goes to 25 which makes me think the origin is not top left but top right...
 *                                          then we or with 0x80 >> (x % 8)
 *                                          so if x is 1 1 mod 8 is 0. so we don't shift any amount.
 *                                          0x80 is 1000 0000 in binary.
 *
 *                                         so or image[26] with 1000 0000 and zero shift
 *
 *                              what about point (25,1)?
 *                                  I would expect the index to be 0.
 *                                  25 + 200 / 8 so thats 50?
 *
 *                                 point(0,0) ==> 0/8 is 0 so index = 0; image[0]
 *
 *                                 point(1,0) ==> 1 + 0 so image [1] ???
 *
 *                                 point(2,0) ==> image[2]
 *
 *                                 point(0,1) ==> image[25]
 *                                 point(0,2) == image[50]
 *
 *                          appears that this is using columns instead of rows.
 *                          so we have 25 columns, that are 200 long.
 *
 *                          a column is a byte
 *                          a byte is 8 bits
 *                          a column is 8 bits.
 *
 *                          the 0x80 makes sense: 1000 0000
 *                             because then the  >> is either 0, 1, .... or 7
 *                          say the shift was 1, meaning x was a 1
 *                          0x80 becomes 0100 0000 or 0x40 remember  >> is div 2
 *
 *                          x is 25???
 *                          mod 8 is 3
 *                          so 0x80 becomes 0x10
 *                          1000 0000 >> >> >> 0001 000
 *
 *                          lets say x is 7, then 0x80 becomes 0x01
 *                          1000 0000 >> x7 == 0000 0001 = 0x01
 *                          image[7 + 25] = image[32]
 *
 *                         ######     <<< lets say # is 8 bits. we need 25 of htem to make an entire row
 *
 *                         how do these relate to the index?
 *
 *
 *
 *
 *
 * */


/* == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == *//* == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == */
/* == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == *//* == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == */
//                                                          MAIN.c
/* == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == *//* == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == */
//* == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == *//* == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == */
void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    __enable_interrupts();


    P9SEL0 &= ~BIT3;
    P9SEL1 &= ~BIT3; //3.1

    P3SEL0 =0;
    P3SEL1 =0; //3.0 THROUGH 3.7

    P3DIR |= 0xFF;
    P9DIR |= BIT3; // led

    P3OUT |= 0xff; // PIN 3.1 APPEARS TO BE BLOWN. DOES NOT GO HIGH OR LOW. STUCK AT LOW.
    //yes pin 3.1 is blown. 3.0 through 3.7 all work except 3.1 THATS THE CLOCK LINE!!
    // DO I REPLACE THE CHIP?
    P9OUT |= BIT3;



    /***********************
     * Initialize LittlevGL
     ***********************/
    lv_init();

    /***********************
     * Tick interface
     ***********************/
    /* Initialize a Timer for 1 ms period and
     * in its interrupt call
     * lv_tick_inc(1); */


    /***********************
     * Display interface
     ***********************/
    initEpaper();                                   /*Initialize your display*/

    lv_disp_drv_t disp_drv;                         /*Descriptor of a display driver*/
    lv_disp_drv_init(&disp_drv);                    /*Basic initialization*/

    /*Set up the functions to access to your display*/
    disp_drv.disp_flush = ex_disp_flush;            /*Used in buffered mode (LV_VDB_SIZE != 0  in lv_conf.h)*/

    /*  not using gpu  */

    /*Finally register the driver*/
    lv_disp_drv_register(&disp_drv);


    /*************************
     * Input device interface
     *************************/
    /*Add a touchpad in the example*/
    /*touchpad_init();*/                            /*Initialize your touchpad*/
//    lv_indev_drv_t indev_drv;                       /*Descriptor of an input device driver*/
//    lv_indev_drv_init(&indev_drv);                  /*Basic initialization*/
//    indev_drv.type = LV_INDEV_TYPE_POINTER;         /*The touchpad is pointer type device*/
//    indev_drv.read = ex_tp_read;                 /*Library ready your touchpad via this function*/
//    lv_indev_drv_register(&indev_drv);              /*Finally register the driver*/
//

    /*************************************
     * Run the task handler of LittlevGL
     *************************************/
    while(1) {
        /* Periodically call this function.
         * The timing is not critical but should be between 1..10 ms */
        lv_task_handler();
        /*delay_ms(5)*/
    }








    clearFrameMemory(0xFF); /// we are getting stuck in the while loop checking busy line here.
    //0x3b command because the phase was wrong! for spi config
    sendToDisplay();

    clearFrameMemory(0xFF);
    sendToDisplay();

    uint32_t i = 0;






        setFrameMemory(alarm_a);
        sendToDisplay();
        setFrameMemory(alarm_a);
        sendToDisplay();

         for(i = 0; i< 2000; i++);

        // clearFrameMemory(0xFF);
       //  sendToDisplay();
       // clearFrameMemory(0xFF);
        sendToDisplay();
        setFrameMemory(alarm_b);
        sendToDisplay();
        setFrameMemory(alarm_b);
        sendToDisplay();


} //end of main



 void setMemoryArea(uint8_t x_start, uint8_t y_start, uint8_t x_end, uint8_t y_end){
         sendCommand(CMD_X_ADDR_START);
         sendData((x_start >> 3) & 0xFF);
         sendData((x_end >> 3) & 0xFF); //>>3 is div 8!

         sendCommand(CMD_Y_ADDR_START);
         sendData(y_start & 0xFF);
         sendData((y_start >> 8) & 0xFF);
         sendData(y_end & 0xFF);
         sendData((y_end >> 8) & 0xFF);

         waitNotBusy(); //we get stuck right here. TODO: why do we get stuck after this command? 0x45 start y address
          //wait for busy line... but busy line never goes low!
 }

 void setMemoryPointer(uint8_t x, uint8_t y){
         sendCommand(CMD_X_COUNTER);
         sendData((x >> 3) & 0xFF);

         sendCommand(CMD_Y_COUNTER);
         sendData(y & 0xFF);
         sendData((y >> 8) & 0xFF);
         waitNotBusy();
 }

 void setFrameMemory(const unsigned char* image_buffer){
         setMemoryArea(0, 0, LCD_HORIZONTAL_MAX- 1, LCD_VERTICAL_MAX - 1);
         setMemoryPointer(0, 0);
         sendCommand(CMD_WRITE_RAM);
         /* send the image data */
         uint16_t i = 0;
         for (i = 0; i < (200 / 8 ) * 200; i++) {
             sendData(image_buffer[i]);
         }
 }


 void clearFrameMemory(unsigned char color) {
     setMemoryArea(0, 0, LCD_HORIZONTAL_MAX, LCD_VERTICAL_MAX);
     setMemoryPointer(0, 0);
     sendCommand(CMD_WRITE_RAM);
     /* send the color data */
     uint16_t i =0;
  for (i = 0; i < ((200/8)*200); i++) {
     //for(i=0; i<200; i++)
     sendData(color);
     }
 }

 void sendToDisplay(){

     sendCommand(CMD_DISPLAY_UPDATE_CTRL2);
     sendData(0xC4);
     sendCommand(CMD_MASTER_ACTV);
     sendCommand(CMD_NOP);

    waitNotBusy();
 }

 void setLUT(const unsigned char* lut){
     sendCommand(CMD_WRITE_LUT);
     uint8_t i = 0;
     for(i = 0; i< 30; i++){
         sendData(lut[i]);
     }
 }


/*============================================================================================================*/
/*============================================================================================================*/
/*============================================================================================================*/
#define LAUNCHPAD_FW
#ifdef LAUNCHPAD_FW
/*
 *  Port 10 for the LaunchPad
 *  GPIO        10.0        =        Data Command pin, Data is HIGH     == Green Ch5 on Logic Analyzer (LA)
 *  PRIMARY     10.1        =        SCK         ==          yellow wire connected to channel 3 on Logic Analyzer
 *  PRIMARY     10.2        =        MISO        ==          BLUE - logic analyzer Ch. 8
 *  GPIO        10.3        =        CS          ==          ORANGE, channel 4 on Logic analyzer
 *
 *  GPIO 7.1    =   ePaper RESET     =      WHITE, CH7 LA //NOTE active LOW
 *  GPIO 7.2    = ePaper BUSY        =      PURPLE, CH4   //NOTE active HIGH
 *
 *  SPI module on UCB3
 *  Polarity: rising edge
 *  clock is low when idle
 *  master only, and STE is not used
 */

void initEpaper(void){
       EUSCI_B3_SPI->CTLW0 |= UCSWRST; // set to a 1, unlock
       EUSCI_B3_SPI->CTLW0 &= ~(UCCKPL  | UC7BIT | UCMODE0   ); // polarity:0, phase:0, 8 bits, spi mode (vs i2c)
       EUSCI_B3_SPI->CTLW0 |= (UCCKPH | UCMSB | UCMST |  UCSYNC | UCSSEL__SMCLK); // MSB, master, sync (vs uart), system clock : 3Mhz

       /* NOTE:
        * very interesting. I am monitoring with a Logic Analyzer (LA) for the arduino code.
        *       the settings:   MSB, 8 bits, clock polarity is 0, low when idle
        *                           DATA valid on leading edge,
        *                           enable line is active low.
        *    The code appears to be talking to the display (changes color).
        *    Busy line goes high at 0x20 command
        *
        *   But when I use these same settings for the SPI hardware on the MSP
        *   I get the busy line going high in the middle randomly suggesting that the chip is getting a command incorrectly.
        *   I then change the polarity to 1 or data shifted out on the "falling edge" (figure 23-4) and the busy line seemed to
        *   behave correctly, not going high unitl command:0x4E which is the CMD_X_Counter command....
        *
        *
        *   BUT WHEN I CHANGE THIS I have to change the settings in the LA, which doesn't make sense. there is a conflict.
        *   I should be able to communicate the exact same way and not change the LA to see decoding.
        *
        *   so i know that we want the clock to be LOW when idle. meaning looking at figure 23-4 tech ref
        *   the only options are the first one or the 3rd one with phase 1.
        *
        *   so now i don't have to change the logic analyzer and the busy line goes high at 0x20 command!!!
        *
        *   the next issue is the why the send data isn't sending data. >>> ioverflow, too small of type, u8t vs u16t. fixed it.
        *
        *
        *
        * */


       //configure the pins
          P7SEL0 &=~(BIT1 | BIT2);        // busy and reset
          P7SEL1  &= ~(BIT1 | BIT2);

          //pull down on BUSY
          P7REN |= BIT2;
          P7OUT &= ~BIT2; // pulldown

          P10SEL0 &=~(BIT0 | BIT3); // D/C and CS
          P10SEL1 &=~(BIT0 | BIT3);

          P10SEL0 |= (BIT1 | BIT2); //primary mode for MISO and SCK
          P10SEL1 &= ~(BIT1 | BIT2);

          P7DIR &= ~(BIT2); //busy is an input
          P7DIR |= BIT1;
          P10DIR |= BIT0 | BIT1 | BIT2 | BIT3;

          P10OUT |= (BIT0 | BIT1 | BIT2 | BIT3); // set all high according to data sheet 21/29 good display
          P7OUT &= ~(BIT2);   //busy is active high!
          P7OUT |= (BIT1);      //reset is active low!

          //may need pull down resistor on the Busy pin, and pull up on the RESET

          //TODO: prescaler if too fast for display, the waveshare code transmits at 2Mhz
      //    EUSCI_B3->BRW |= BIT1; // default reset value is 0... odd.

          EUSCI_B3_SPI->CTLW0 &= ~UCSWRST; // set to a 0 lock

          //enable interrupt for the TX
          EUSCI_B3_SPI ->IFG = 0;     //clear any interrupts
          EUSCI_B3_SPI -> IE |= UCTXIE;

          NVIC_EnableIRQ(EUSCIB3_IRQn);
          //reset epaper 10ms
          uint16_t delay = 0;
          P7OUT &=~BIT1;
          for(delay = 0; delay < 12000; delay++);
          P7OUT |= BIT1;
          for(delay = 0; delay < 12000; delay++); //double check with LA to see if equal or greater than 10mS



          //ePaper Init Sequence
              sendCommand(CMD_DRIVER_OUTPUT_CONTROL);
              sendData((LCD_VERTICAL_MAX - 1) & 0xFF);
              sendData(((LCD_HORIZONTAL_MAX - 1) >> 8) & 0xFF);
              sendData(0x00);                     // GD = 0; SM = 0; TB = 0;
              sendCommand(CMD_BOOSTER_SOFT_START_CONTROL);
              sendData(0xD7);
              sendData(0xD6);
              sendData(0x9D);
              sendCommand(CMD_VCOM);
              sendData(0xA8);                     // VCOM 7C
              sendCommand(CMD_DUMMY_LINE);
              sendData(0x1A);                     // 4 dummy lines per gate
              sendCommand(CMD_GATE_TIME); // the busy line is being set high here.
              sendData(0x08);                    // 2us per line
              sendCommand(CMD_DATA_ENTRY);
              sendData(0x03);                     // X increment; Y increment

              //send LUT
              setLUT(lut_full_update);

}


//launchpad
void sendCommand(uint8_t command){
    while(EUSCI_B3_SPI->IFG & UCTXIFG);
    P10OUT &= ~BIT0;     // D/C
    P10OUT &= ~BIT3;     // CS
    EUSCI_B3_SPI->TXBUF = command;
    P10OUT |= BIT3;
}

void sendData(uint8_t data){
    while(EUSCI_B3_SPI->IFG & UCTXIFG);
    P10OUT |=   BIT0;     // D/C
    P10OUT &= ~BIT3;     // CS
    EUSCI_B3_SPI->TXBUF = data;
    P10OUT |= BIT3;
}

void waitNotBusy(){

    uint8_t check = P7IN & BIT2;
    while(P7IN & BIT2);

}


/* ================================================================ */
//                              ISRs
/* ================================================================ */
/* ================================================================ */

void EUSCIB3_IRQHandler(void)
{
    if(EUSCI_B3_SPI->IFG & EUSCI_B_IFG_TXIFG){
        EUSCI_B3_SPI->IFG &= ~EUSCI_B_IFG_TXIFG;
    }
}

#endif //END OF LAUNCHPAD FIRMWARE

/*&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&*/
/*&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&*/
/*&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&*/
/*&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&*/
/*&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&*/
/*&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&*/

//#define PCB_FIRMWARE
#ifdef PCB_FIRMWARE
/*
 *  Port 3 for the PCB
 *  GPIO        3.0         =        D/C         ==             //HIGH = Data
 *  GPIO        3.2         =        CS
 *  PRIMARY     3.1         =        SCK         ==
 *  PRIMARY     3.3         =        MOSI        ==
 *
 *  GPIO        7.7         =    ePaper RESET    ==
 *  GPIO        8.1         =    ePaper BUSY     ==
 *  GPIO        8.0         =    ePaper BS1 // ties low for 4 wire spi !!! this is not required for the Breakout Board but it is for my PCB
 *
 *  SPI module on UA2
 *  Polarity: rising edge
 *  Clock Phase: !!! Curcial, has  to be set as the third option in the Tech Ref, this was what was causing all the problems. I arrived at this conclusion after looking at what the idle states had to be. only 2 options that made idle LOW.
 *          Tech Ref 23.4
 *  clock is low when idle
 *  master only, and STE is not used
 * */

void initEpaper(void){
       EUSCI_A2_SPI->CTLW0 |= UCSWRST; // set to a 1, unlock
       EUSCI_A2_SPI->CTLW0 &= ~(UCCKPL  | UC7BIT | UCMODE0   ); // polarity:0, 8 bits, spi mode (vs i2c)
       EUSCI_A2_SPI->CTLW0 |= (UCCKPH | UCMSB | UCMST |  UCSYNC | UCSSEL__SMCLK); // MSB, master, sync (vs uart), system clock : 3Mhz, phase: 1 Note: this was the key.

       //configure the pins
          P7SEL0 &=~(BIT7);        // reset 7.7
          P7SEL1  &= ~(BIT7);

          P8SEL0 &=~(BIT0| BIT1);        // busy 8.1,   BS1 (8.0)
          P8SEL1  &= ~(BIT0 | BIT1);

          //pull down on BUSY
          P8REN |= BIT1;
          P8OUT &= ~BIT1; // pulldown

          P3SEL0 &=~(BIT0 | BIT2); // D/C (3.0) and CS(3.2)
          P3SEL1 &=~(BIT0 | BIT2);

          P3SEL0 |= (BIT3 | BIT1); //primary mode for MOSI (3.3) and SCK (3.1)
          P3SEL1 &= ~(BIT3 | BIT1);

          P3DIR |= BIT0|BIT1| BIT2 | BIT3;
          P7DIR |= BIT7;    //reset
          P8DIR |= BIT0;    //BS1
          P8DIR &= ~(BIT1); //busy is an input


          P3OUT |= (BIT0 | BIT1 | BIT2 | BIT3); // set all high according to data sheet 21/29 "good display pdf"
          P7OUT |= (BIT7);      //reset is active low!
          P8OUT &= ~(BIT1);   //busy is active high!

          //4 wire spi for ePaper (RTFM)
          P8OUT &= ~BIT0;



      //    EUSCI_B3->BRW |= BIT1; // default reset value is 0... weird. also we could change system clock to be faster so we could transfer faster than 3Mhz!
          EUSCI_A2_SPI->CTLW0 &= ~UCSWRST; // set to a 0 lock

          //enable interrupt for the TX
          EUSCI_A2_SPI ->IFG = 0;     //clear any interrupts
          EUSCI_A2_SPI -> IE |= UCTXIE;

          NVIC_EnableIRQ(EUSCIA2_IRQn);


          //reset epaper 10ms
          uint16_t delay = 0;
          P7OUT &=~BIT7;
          for(delay = 0; delay < 12000; delay++);
          P7OUT |= BIT7;
          for(delay = 0; delay < 12000; delay++); //double check with LA to see if equal or greater than 10mS

          //ePaper Init Sequence
              sendCommand(CMD_DRIVER_OUTPUT_CONTROL);
              sendData((LCD_VERTICAL_MAX - 1) & 0xFF);
              sendData(((LCD_HORIZONTAL_MAX - 1) >> 8) & 0xFF);
              sendData(0x00);                     // GD = 0; SM = 0; TB = 0;
              sendCommand(CMD_BOOSTER_SOFT_START_CONTROL);
              sendData(0xD7);
              sendData(0xD6);
              sendData(0x9D);
              sendCommand(CMD_VCOM);
              sendData(0xA8);                     // VCOM 7C
              sendCommand(CMD_DUMMY_LINE);
              sendData(0x1A);                     // 4 dummy lines per gate
              sendCommand(CMD_GATE_TIME); // the busy line is being set high here.
              sendData(0x08);                    // 2us per line
              sendCommand(CMD_DATA_ENTRY);
              sendData(0x03);                     // X increment; Y increment

              //send LUT
              setLUT(lut_full_update);

}

// these functions change depending on what the platform is: PCB or LaunchPad
// these 3 are the only ones to change so far...
void sendCommand(uint8_t command){
    while(EUSCI_A2_SPI->IFG & UCTXIFG);
    P3OUT &= ~BIT0;     // D/C
    P3OUT &= ~BIT2;     // CS
    EUSCI_A2_SPI->TXBUF = command;
    P3OUT |= BIT2;
}

void sendData(uint8_t data){
    while(EUSCI_A2_SPI->IFG & UCTXIFG);
    P3OUT |=   BIT0;     // D/C
    P3OUT &= ~BIT2;     // CS
    EUSCI_B3_SPI->TXBUF = data;
    P3OUT |= BIT2;
}

void waitNotBusy(){
 //   while(P8IN & BIT1);
}


/* ================================================================ */
//                              ISRs
/* ================================================================ */
/* ================================================================ */

void EUSCIA2_IRQHandler(void)
{
    if(EUSCI_A2_SPI->IFG & EUSCI_A_IFG_TXIFG){
        EUSCI_A2_SPI->IFG &= ~EUSCI_A_IFG_TXIFG;
    }
}

#endif  //END OF PCB FIRMWARE


/*&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&*/
/*&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&*/
/*&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&*/
/*&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&*/
/*&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&*/
/*&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&*/



//#define DATASHEET_INIT
#ifdef DATASHEET_INIT
void initEpaper(void){

    EUSCI_B3_SPI->CTLW0 |= UCSWRST; // set to a 1, unlock
    EUSCI_B3_SPI->CTLW0 &= ~(UCCKPH | UCCKPL | UC7BIT | UCMODE0   ); // polarity:0, phase:0, 8 bits, spi mode (vs i2c)
    EUSCI_B3_SPI->CTLW0 |= (UCMSB | UCMST |  UCSYNC | UCSSEL__SMCLK); // MSB, master, sync (vs uart), system clock : 3Mhz
    //TODO: think about using defines like #define CS_PIN   (BIT0) so I don't have to write so much code twice
    //maybe to make better you could use a define for pins and just redefine depending on what you're doing ie LaunchPad or the PCB

    //configure the pins
    P7SEL0 &=~(BIT1 | BIT2);        // busy and reset
    P7SEL1  &= ~(BIT1 | BIT2);

    P10SEL0 &=~(BIT0 | BIT3); // D/C and CS
    P10SEL1 &=~(BIT0 | BIT3);

    P10SEL0 |= (BIT1 | BIT2); //primary mode for MISO and SCK
    P10SEL1 &= ~(BIT1 | BIT2);

    P7DIR &= ~(BIT2); //busy is an input
    P7DIR |= BIT1;
    P10DIR |= BIT0 | BIT1 | BIT2 | BIT3;

    P10OUT |= (BIT0 | BIT1 | BIT2 | BIT3); // set all high according to data sheet 21/29 good display
    P7OUT &= ~(BIT2);   //busy is active high!
    P7OUT |= (BIT1);      //reset is active low!

    //may need pull down resistor on the Busy pin, and pull up on the RESET

    //TODO: prescaler if too fast for display, the waveshare code transmits at 2Mhz
    //EUSCI_B3->BRW |= 1; // default reset value is 0... odd.

    EUSCI_B3_SPI->CTLW0 &= ~UCSWRST; // set to a 0 lock

    //enable interrupt for the TX
    EUSCI_B3_SPI ->IFG = 0;     //clear any interrupts
    EUSCI_B3_SPI -> IE |= UCTXIE;

    NVIC_EnableIRQ(EUSCIB3_IRQn);


    //Panel Reset
    uint16_t delay = 0;
    P7OUT &=~BIT1;
    for(delay = 0; delay < 6000; delay++);
    P7OUT |= BIT1;
    for(delay = 0; delay < 6000; delay++); //double check with LA to see if equal or greater than 10mS

    //Gate number and scan order setting
    sendCommand(CMD_DRIVER_OUTPUT_CONTROL);
    sendData((LCD_VERTICAL_MAX - 1) & 0xFF); // Thanks Yehui from Waveshare!
    sendData(((LCD_HORIZONTAL_MAX - 1) >> 8) & 0xFF);// Thanks Yehui from Waveshare!
    sendCommand(0x00);

    sendCommand(CMD_BOOSTER_SOFT_START_CONTROL);
    sendData(0xD7);
    sendData(0xD6);
    sendData(0x9D);

    //set scan frequency 50hz
    sendCommand(CMD_DUMMY_LINE); //dummy line wdith
    sendData(0x1B); // default value
    sendCommand(CMD_GATE_TIME);
    sendData(0x0B); //default value
    //data entry sequence (y-, x+)/// oof that is gonna trip up my brain
   sendCommand(CMD_DATA_ENTRY);
   sendData(0x01); // note: what would change when we make this bot y+ x+ where would origin be
   //set ram X start/end
   //look on section 8.3 of the IL3820 datasheet shows a diagram of the way data is written
   sendCommand(CMD_X_ADDR_START);
   sendData(0x00);
   sendData(0x18); //specified in the data sheet, interesting, so 200/8 is 25_base10, but 0x19... and 0 to 0x18 is 24
   //set ram Y start end
   sendCommand(CMD_Y_ADDR_START);
   sendData(0xC7); //199 because 0 to 199 is 200 points //oh! because a row is 1 pixel and we have 200 rows?
   sendData(0x00); //why not 25 row?
   sendData(0x00);
   sendData(0x00); //might be one extra, but specified in the datasheet

   //vcom setting
   sendCommand(CMD_VCOM);
   sendData(0xA8); // Thanks Yehui from Waveshare!
   //wavfrom setting
   sendCommand(CMD_WRITE_LUT);
   uint8_t lutCount = 0;
   for(lutCount = 0; lutCount < 30; lutCount++){
       sendData(lut_full_update[lutCount]);
   }
//set ramX address counter
   sendCommand(CMD_X_COUNTER);
   sendData(0x00); //note: from datasheet
//set ramY address counter\
   sendCommand(CMD_Y_COUNTER);
   sendData(0xC7); // note from datasheet
   sendData(0x00);





   //image data download!
   sendCommand(CMD_WRITE_RAM);
   uint16_t fillBlankCount = 0;
   for(fillBlankCount = 0; fillBlankCount < 200 / 8* 200; fillBlankCount++){
       sendData(WHITE);
   }

   //display update sequence setting: use waveform from ram
   sendCommand(CMD_DISPLAY_UPDATE_CTRL2); //note from datasheet
   sendData(0xC7);

   //image update
   sendCommand(CMD_MASTER_ACTV);

   //wait until not busy, or when busy pin goes LOW
  // while(P7IN&BIT2);
   sendCommand(CMD_DEEP_SLEEP);
   sendData(0x01); //note specified by datasheet



}
#endif



/*
 * WELCOME TO THE LITTLEVGL TUTORIAL
 *
 *-------------------------------------------------------------------------------
 * Now you will learn how to port the Littlev Graphics Library to your system
 * ------------------------------------------------------------------------------
 *
 * 1. Before start download or clone the lvgl folder: https://github.com/littlevgl/lvgl.git
 * 2. Copy 'lvgl' into your project folder
 * 3. Copy 'lvgl/lv_conf_templ.h' and 'lv_conf.h' next to 'lvgl' and remove the first and last #if and  #endif
 * 4. To initialize the library your main.c should look like this file
 *
 *Here are some info about the drawing modes in the library:
 *
 * BUFFERED DRAWING
 * The graphics library works with an internal buffering mechanism to
 * create advances graphics features with only one frame buffer.
 * The internal buffer is called VDB (Virtual Display Buffer) and its size can be adjusted in lv_conf.h.
 * When LV_VDB_SIZE not zero then the internal buffering is used and you have to provide a function
 * which flushes the buffers content to your display.

 */



/**********************
 *  STATIC PROTOTYPES
 **********************/
static void ex_disp_flush(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t * color_p);
static void ex_disp_map(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t * color_p);
static void ex_disp_fill(int32_t x1, int32_t y1, int32_t x2, int32_t y2,  lv_color_t color);
#if USE_LV_GPU
static void ex_mem_blend(lv_color_t * dest, const lv_color_t * src, uint32_t length, lv_opa_t opa);
static void ex_mem_fill(lv_color_t * dest, uint32_t length, lv_color_t color);
#endif
static bool ex_tp_read(lv_indev_data_t * data);



/**********************
 *   STATIC FUNCTIONS
 *
 *   I fill these definitions out with the function to drive the display.
 **********************/

void drawAbsolutePixel(uint8_t x, uint8_t y, unsigned char color){
    if(x < 0 || y < 0 || x > LCD_HORIZONTAL_MAX || y > LCD_VERTICAL_MAX) return; //invalid

             if (color) {
                 image[(x + y * LCD_HORIZONTAL_MAX) / 8] &= ~(0x80 >> (x % 8));
             } else {
                 image[(x + y * LCD_HORIZONTAL_MAX) / 8] |= 0x80 >> (x % 8);
             }
             //there should be an invereted case here too

}

/* Flush the content of the internal buffer the specific area on the display
 * You can use DMA or any hardware acceleration to do this operation in the background but
 * 'lv_flush_ready()' has to be called when finished
 * This function is required only when LV_VDB_SIZE != 0 in lv_conf.h*/
static void ex_disp_flush(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t * color_p)
{
    /*The most simple case (but also the slowest) to put all pixels to the screen one-by-one*/

    int32_t x;
    int32_t y;
    for(y = y1; y <= y2; y++) {
        for(x = x1; x <= x2; x++) {
            /* Put a pixel to the display. For example: */
            /* put_px(x, y, *color_p)*/
            color_p++;
        }
    }

    /* IMPORTANT!!!
     * Inform the graphics library that you are ready with the flushing*/
    lv_flush_ready();
}





