/********************************************************************************************
* FrodoKEM: Learning with Errors Key Encapsulation
*
* Abstract: noise sampling functions
*********************************************************************************************/

#include "sha3/fips202.h"
#include <assert.h>

//include files to call the new Gaussian sampler
#include "inner.h"
#include "Frodo.h"

/*
NON-AVX
void frodo_sample_n(uint16_t *s, const size_t n) 
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
*/

void frodo_sample_n(uint16_t *s, const size_t n)  
{
        inner_shake256_context rng;
	sampler_context sc;
	fpr isigma, mu, muinc;   

	inner_shake256_init(&rng);
	inner_shake256_inject(&rng, (const void *)"test sampler", 12);
	inner_shake256_flip(&rng);
	Zf(prng_init)(&sc.p, &rng);
	sc.sigma_min = fpr_sigma_min[9];

	isigma = fpr_div(fpr_of(10), fpr_of(28));// sigma is 2.8 for Frodo640
	mu = fpr_neg(fpr_one);
	for(int i=0;i<n;i++) 
		s[i] = Zf(sampler)(&sc, mu, isigma); 
	
	 
}
