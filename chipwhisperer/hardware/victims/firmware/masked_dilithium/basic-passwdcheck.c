/*
    This file is part of the ChipWhisperer Example Targets
    Copyright (C) 2012-2015 NewAE Technology Inc.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "hal.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "simpleserial.h"
#include "rounding.h"
#include "randombytes.h"
#define BETA1 343467 //8388608

#define IDLE 0
#define KEY 1
#define PLAIN 2

#define BUFLEN 64

uint8_t memory[BUFLEN];
uint8_t tmp[BUFLEN];
char asciibuf[BUFLEN];
uint8_t pt[1];



#if SS_VER == SS_VER_2_1
uint8_t get_pt(uint8_t cmd, uint8_t scmd, uint8_t len, uint8_t* pt)
#else
uint8_t get_pt(uint8_t* pt, uint8_t len)
#endif

{
 uint8_t res[1]; 
 
 //fix a random number to decompose
 int32_t value = pt[0];
 value = value >>10;

  
int32_t lowbits_arr[NUM_SHARES];
int32_t a[NUM_SHARES];
	
	
	/**********************************
	* Start user-specific code here. */ 
	trigger_high(); 
	
	arith_mask(value,a);
	arith_to_bool_lowbits(a,BETA1,lowbits_arr);
	for (int j=0;j<10;j++) 		asm("nop");
		
	 
	trigger_low();
        
        res[0] = (uint8_t)lowbits_arr;
	/* End user-specific code here. *
	********************************/
	simpleserial_put('r', 1, res);
	return 0x00;
}

uint8_t reset(uint8_t* x, uint8_t len)
{
	// Reset key here if needed
	return 0x00;
}


int main(void)
{
        platform_init();
	init_uart();
	trigger_setup();

 	/* Uncomment this to get a HELLO message for debug */
	 
	putch('h');
	putch('e');
	putch('l');
	putch('l');
	putch('o');
	putch('\n');
	 
	simpleserial_init();
	simpleserial_addcmd('p', 1, get_pt);  
#if SS_VER != SS_VER_2_1 
	simpleserial_addcmd('x', 0, reset);
#endif
	while(1)
		simpleserial_get();
}
