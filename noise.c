/********************************************************************************************
* FrodoKEM: Learning with Errors Key Encapsulation
*
* Abstract: noise sampling functions
*********************************************************************************************/

#include "sha3/fips202.h"
#include <assert.h>
#include <math.h>

//include files to call the new Gaussian sampler
#include "inner.h"
#include "Frodo.h"



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <emmintrin.h>
#ifdef __SSE4_1__
#include <smmintrin.h>
#endif


__m128 fast_exp_sse (__m128 x)
{
    __m128 t, f, e, p, r;
    __m128i i, j;
    __m128 l2e = _mm_set1_ps (1.442695041f);  /* log2(e) */
    __m128 c0  = _mm_set1_ps (0.3371894346f);
    __m128 c1  = _mm_set1_ps (0.657636276f);
    __m128 c2  = _mm_set1_ps (1.00172476f);

    /* exp(x) = 2^i * 2^f; i = floor (log2(e) * x), 0 <= f <= 1 */   
    t = _mm_mul_ps (x, l2e);             /* t = log2(e) * x */
#ifdef __SSE4_1__
    e = _mm_floor_ps (t);                /* floor(t) */
    i = _mm_cvtps_epi32 (e);             /* (int)floor(t) */
#else /* __SSE4_1__*/
    i = _mm_cvttps_epi32 (t);            /* i = (int)t */
    j = _mm_srli_epi32 (_mm_castps_si128 (x), 31); /* signbit(t) */
    i = _mm_sub_epi32 (i, j);            /* (int)t - signbit(t) */
    e = _mm_cvtepi32_ps (i);             /* floor(t) ~= (int)t - signbit(t) */
#endif /* __SSE4_1__*/
    f = _mm_sub_ps (t, e);               /* f = t - floor(t) */
    p = c0;                              /* c0 */
    p = _mm_mul_ps (p, f);               /* c0 * f */
    p = _mm_add_ps (p, c1);              /* c0 * f + c1 */
    p = _mm_mul_ps (p, f);               /* (c0 * f + c1) * f */
    p = _mm_add_ps (p, c2);              /* p = (c0 * f + c1) * f + c2 ~= 2^f */
    j = _mm_slli_epi32 (i, 23);          /* i << 23 */
    r = _mm_castsi128_ps (_mm_add_epi32 (j, _mm_castps_si128 (p))); /* r = p * 2^i*/
    return r;
}
 
 

float CalculateExp(float start)
{ 
    __m128 x, y;

     union {
        float f[4];
        unsigned int i[4];
    } arg, res;
   
 
    arg.f[0] = start; 
    memcpy (&x, &arg, sizeof(x));
    y = fast_exp_sse (x);
    memcpy (&res, &y, sizeof(y));  
   
   return (res.f[0]);


}

 
void frodo_sample_n(uint16_t *s, const size_t n) 
{ // Fills vector s with n samples from the noise distribution which requires 16 bits to sample. 
  // The distribution is specified by its CDF.
  // Input: pseudo-random values (2*n bytes) passed in s. The input is overwritten by the output.
   
    unsigned int i, j, y;
    int Bexp=0;  
    for (i = 0; i < n; ++i) {
        uint16_t sample = 0;
        uint16_t prnd = s[i] >> 1;    // Drop the least significant bit
        uint16_t sign = s[i] & 0x1;    // Pick the least significant bit 
       // No need to compare with the last value. 
       
       for (j = 0; j < (unsigned int)(CDF_TABLE_LEN_NEW - 1); j++) {
            // Constant time comparison: 1 if CDF_TABLE[j] < s, 0 otherwise. Uses the fact that CDF_TABLE[j] and s fit in 15 bits.
            sample += (uint16_t)(CDF_TABLE_NEW[j] - prnd) >> 15;
        }
        // Assuming that sign is either 0 or 1, flips sample iff sign = 1
        
         //sample = y+2*sample;
         // assign the sign
         s[i] = ((-sign) ^ sample) + sign; 
         
     
    }
 
/*
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
 

   // The distribution is specified by its CDF.
  // Input: pseudo-random values (2*n bytes) passed in s. The input is overwritten by the output.
    unsigned int i, j;


    for (i = 0; i < n; ++i) {
        uint16_t sample = 0;
        uint16_t prnd = s[i] >> 1;    // Drop the least significant bit
        uint16_t sign = s[i] & 0x1;    // Pick the least significant bit

        // No need to compare with the last value. 
         sample = SAMPLES_TABLE[prnd];
        // Assuming that sign is either 0 or 1, flips sample iff sign = 1
        s[i] = ((-sign) ^ sample) + sign; 
    }
 */
 
}
