/*****************************************************************************/
/*                                                                           */
/* FILENAME                                                                  */
/* 	 main.c                                                                  */
/*                                                                           */
/* DESCRIPTION                                                               */
/*   TMS320C5505 USB Stick. Application 1. Getting started.                  */
/*   Take microphone input and send to headphones.                           */
/*                                                                           */
/* REVISION                                                                  */
/*   Revision: 1.00	                                                         */
/*   Author  : Richard Sikora                                                */
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* HISTORY                                                                   */
/*   Revision: 1.00                                                          */
/*   5th March 2010. Created by Richard Sikora from TMS320C5510 DSK code.    */
/*                                                                           */
/*****************************************************************************/
/*
 * Copyright (C) 2010 Texas Instruments Incorporated - http://www.ti.com/ 
 * 
 * 
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions 
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the   
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/

#include "stdio.h"
#include "usbstk5505.h"
#include "aic3204.h"
#include "PLL.h"
#include "stereo.h"

Int16 left_input;
Int16 right_input;
Int16 left_output;
Int16 right_output;
Int16 mono_input;


Int16 sin_tab_1000(void);
Int16 sin_tab_7500(void);


#define SAMPLES_PER_SECOND 48000

unsigned long int i = 0;

int sin_1k_q1_index = 0;
Int16 sin_1k_q1[48] = {0x0000,
                       0x0887,
                       0x10E8,
                       0x18FC,
                       0x209D,
                       0x27A9,
                       0x2E01,
                       0x3386,
                       0x381F,
                       0x3BB9,
                       0x3E41,
                       0x3FAD,
                       0x3FF6,
                       0x3F1B,
                       0x3D20,
                       0x3A0D,
                       0x35F1,
                       0x30DF,
                       0x2AEE,
                       0x2438,
                       0x1CDD,
                       0x14FE,
                       0x0CBF,
                       0x0446,
                       0xFBB9,
                       0xF340,
                       0xEB01,
                       0xE322,
                       0xDBC7,
                       0xD511,
                       0xCF20,
                       0xCA0E,
                       0xC5F2,
                       0xC2DF,
                       0xC0E4,
                       0xC009,
                       0xC052,
                       0xC1BE,
                       0xC446,
                       0xC7E0,
                       0xCC79,
                       0xD1FE,
                       0xD856,
                       0xDF62,
                       0xE703,
                       0xEF17,
                       0xF778,
                       0xFFFF};

int sin_75k_q1_index = 0;
Int16 sin_75k_q1[32] = {0x0000,
                        0x3650,
                        0x3975,
                        0x0679,
                        0xCD63,
                        0xC3FB,
                        0xF31D,
                        0x2E63,
                        0x3DF4,
                        0x1328,
                        0xD64F,
                        0xC0BC,
                        0xE6C2,
                        0x248F,
                        0x3FEA,
                        0x1F0F,
                        0xE0F0,
                        0xC015,
                        0xDB70,
                        0x193D,
                        0x3F43,
                        0x29B0,
                        0xECD7,
                        0xC20B,
                        0xD19C,
                        0x0CE2,
                        0x3C04,
                        0x329C,
                        0xF986,
                        0xC68A,
                        0xC9AF,
                        0x0000};


/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  main( )                                                                 *
 *                                                                          *
 * ------------------------------------------------------------------------ */
void main(void) {

    /* Initialize BSL */
    USBSTK5505_init( );
	
	/* Initialize PLL */
	pll_frequency_setup(100);

    /* Initialise hardware interface and I2C for code */
    aic3204_hardware_init();
    
    /* Initialise the AIC3204 codec */
	aic3204_init(); 


    printf("\n\nRunning Getting Started Project\n");
    printf( "<-> Audio Loopback from Stereo IN --> to HP/Lineout\n" );
	
	/* Setup sampling frequency, ADC gain [0,48], and DAC gain [-63, 24] */
    set_sampling_frequency_and_gain(SAMPLES_PER_SECOND, 10, -13);
  
    asm(" bclr XF"); // assembler code | bit clear XF -> steuert LED auf dem Board
    // bset XF
   
    for (i = 0; i < SAMPLES_PER_SECOND * 600000L; i++) {
        aic3204_codec_read(&left_input, &right_input); // Configured for one interrupt per two channels.

        mono_input = stereo_to_mono(left_input, right_input);

        //left_output =  left_input;            // Very simple processing. Replace with your own code!
        //right_output = right_input;          // Directly connect inputs to outputs.
        right_output = sin_tab_1000();
        left_output = sin_tab_7500();

        aic3204_codec_write(left_output, right_output);
    }

    /* Disable I2S and put codec into reset */
    aic3204_disable();

    printf( "\n***Program has Terminated***\n" );
    SW_BREAKPOINT;
}


Int16 sin_tab_1000() {
    if(sin_1k_q1_index > 47)
        sin_1k_q1_index = 0;
    return sin_1k_q1[sin_1k_q1_index++];
}

Int16 sin_tab_7500() {
    if(sin_75k_q1_index > 31)
        sin_75k_q1_index = 0;
    return sin_75k_q1[sin_75k_q1_index++];
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  End of main.c                                                           *
 *                                                                          *
 * ------------------------------------------------------------------------ */
