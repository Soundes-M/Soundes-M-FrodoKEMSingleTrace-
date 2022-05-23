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


// Parameters for "FrodoKEM-640"
#define PARAMS_N 640
#define PARAMS_NBAR 8
#define PARAMS_LOGQ 15
#define PARAMS_Q (1 << PARAMS_LOGQ)
#define PARAMS_EXTRACTED_BITS 2
#define PARAMS_STRIPE_STEP 8
#define PARAMS_PARALLEL 4
#define BYTES_SEED_A 16
#define BYTES_MU (PARAMS_EXTRACTED_BITS*PARAMS_NBAR*PARAMS_NBAR)/8
#define BYTES_PKHASH CRYPTO_BYTES
#define CRYPTO_BYTES              16
    
uint8_t memory[BUFLEN];
uint8_t tmp[BUFLEN];
char asciibuf[BUFLEN];
uint8_t pt[1];


// CDF table
uint16_t CDF_TABLE[13] = {4643, 13363, 20579, 25843, 29227, 31145, 32103, 32525, 32689, 32745, 32762, 32766, 32767};
uint16_t CDF_TABLE_LEN = 13; 


void frodo_sample_n(uint16_t *s, const int n) 
{ // Fills vector s with n samples from the noise distribution which requires 16 bits to sample. 
  // The distribution is specified by its CDF.
  // Input: pseudo-random values (2*n bytes) passed in s. The input is overwritten by the output.
    unsigned int i, j;
 
    for (i = 0; i < n; ++i) {
        uint16_t sample = 0;
        uint16_t prnd = s[i] >> 1;    // Drop the least significant bit
        uint16_t sign = s[i] & 0x1;    // Pick the least significant bit

        // No need to compare with the last value.
        for (j = 0; j < (unsigned int)(CDF_TABLE_LEN - 1); j++) {
            // Constant time comparison: 1 if CDF_TABLE[j] < s, 0 otherwise. Uses the fact that CDF_TABLE[j] and s fit in 15 bits.
            sample += (uint16_t)(CDF_TABLE[j] - prnd) >> 15;
        }
        
        // Assuming that sign is either 0 or 1, flips sample iff sign = 1 
        s[i] = ((-sign) ^ sample) + sign; 
       
    }
}


void frodo_key_encode(uint16_t *out, const uint16_t *in) 
{ // Encoding
    unsigned int i, j, npieces_word = 8;
    unsigned int nwords = (PARAMS_NBAR*PARAMS_NBAR)/8;
    uint64_t temp, mask = ((uint64_t)1 << PARAMS_EXTRACTED_BITS) - 1;
    uint16_t* pos = out;

    for (i = 0; i < nwords; i++) {
        temp = 0;
        for(j = 0; j < PARAMS_EXTRACTED_BITS; j++) 
            temp |= ((uint64_t)((uint8_t*)in)[i*PARAMS_EXTRACTED_BITS + j]) << (8*j);
        for (j = 0; j < npieces_word; j++) { 
            *pos = (uint16_t)((temp & mask) << (PARAMS_LOGQ - PARAMS_EXTRACTED_BITS));  
            temp >>= PARAMS_EXTRACTED_BITS;
            pos++;
        }
    }
}

void frodo_key_decode(uint16_t *out, const uint16_t *in)
{ // Decoding
    unsigned int i, j, index = 0, npieces_word = 8;
    unsigned int nwords = (PARAMS_NBAR * PARAMS_NBAR) / 8;
    uint16_t temp, maskex=((uint16_t)1 << PARAMS_EXTRACTED_BITS) -1, maskq =((uint16_t)1 << PARAMS_LOGQ) -1;
    uint8_t  *pos = (uint8_t*)out;
    uint64_t templong;

    for (i = 0; i < nwords; i++) {
        templong = 0;
        for (j = 0; j < npieces_word; j++) {  // temp = floor(in*2^{-11}+0.5)
            temp = ((in[index] & maskq) + (1 << (PARAMS_LOGQ - PARAMS_EXTRACTED_BITS - 1))) >> (PARAMS_LOGQ - PARAMS_EXTRACTED_BITS);
            templong |= ((uint64_t)(temp & maskex)) << (PARAMS_EXTRACTED_BITS * j);
            index++;
        }
        
	for(j = 0; j < PARAMS_EXTRACTED_BITS; j++) 
	    pos[i*PARAMS_EXTRACTED_BITS + j] = (templong >> (8*j)) & 0xFF; 
    }
  
}

 


#if SS_VER == SS_VER_2_1
uint8_t get_pt(uint8_t cmd, uint8_t scmd, uint8_t len, uint8_t* pt)
#else
uint8_t get_pt(uint8_t* pt, uint8_t len)
#endif

{   
        for(int i=0;i<16;i++) pt[i] = 0;
        randombytes(pt, 16);     
	/**********************************
	* Start user-specific code here. */ 
	trigger_high(); 
	
         
        frodo_sample_n(pt,1);
                 
	trigger_low();
        
         
	/* End user-specific code here. *
	********************************/
	simpleserial_put('r', 1, pt);
	
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
