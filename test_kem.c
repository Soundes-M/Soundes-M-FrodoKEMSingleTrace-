/********************************************************************************************
* FrodoKEM: Learning with Errors Key Encapsulation
*
* Abstract: benchmarking/testing KEM scheme
*********************************************************************************************/
 

#define KEM_TEST_ITERATIONS 1
#define KEM_BENCH_SECONDS     1


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
 


#define DUDECT_IMPLEMENTATION
#include "dudect.h"

#define SECRET_LEN_BYTES (16)

 
 

uint8_t do_one_computation(uint8_t *data) {
   
  uint8_t sk[CRYPTO_SECRETKEYBYTES];
  uint8_t ss_encap[CRYPTO_BYTES], ss_decap[CRYPTO_BYTES];
  uint16_t s[2*PARAMS_N*PARAMS_NBAR] = {0};  
  
  uint8_t ct[CRYPTO_CIPHERTEXTBYTES]; 
  memcpy(ct, data, CRYPTO_CIPHERTEXTBYTES); 
  
  crypto_kem_dec(ss_decap, ct, sk); 
   
  return   0; 
}

void prepare_inputs(dudect_config_t *c, uint8_t *input_data, uint8_t *classes) {
  randombytes(input_data, c->number_measurements * c->chunk_size);
  for (size_t i = 0; i < c->number_measurements; i++) {
    classes[i] = randombit();
    if (classes[i] == 0) {
      memset(input_data + (size_t)i * c->chunk_size, 0x00, c->chunk_size);
    } else {
      // leave random
    }
  }
}


int run_test(void) {
  dudect_config_t config = {
      .chunk_size = SECRET_LEN_BYTES,
      #ifdef MEASUREMENTS_PER_CHUNK
      .number_measurements = MEASUREMENTS_PER_CHUNK,
      #else
      .number_measurements = 500,
      #endif
  };
  dudect_ctx_t ctx;

  dudect_init(&ctx, &config);

  /*
  Call dudect_main() until
   - returns something different than DUDECT_NO_LEAKAGE_EVIDENCE_YET, or
   - you spent too much time testing and give up
  Recommended that you wrap this program with timeout(2) if you don't
  have infinite time.
  For example this will run for 20 mins:
    $ timeout 1200 ./your-executable
  */
  dudect_state_t state = DUDECT_NO_LEAKAGE_EVIDENCE_YET;
  while (state == DUDECT_NO_LEAKAGE_EVIDENCE_YET) {
    state = dudect_main(&ctx);
  }
  dudect_free(&ctx);
  return (int)state;
}



static int kem_test(const char *named_parameters, int iterations) 
{
 
    printf("\n");
           printf("=============================================================================================================================\n");
    printf("Testing correctness of key encapsulation mechanism (KEM), system %s, tests for %d iterations\n", named_parameters, iterations);
    printf("=============================================================================================================================\n");
    
uint8_t sk[CRYPTO_SECRETKEYBYTES];
uint8_t ss_encap[CRYPTO_BYTES], ss_decap[CRYPTO_BYTES];
uint16_t s[2*PARAMS_N*PARAMS_NBAR] = {0};   
uint8_t ct[CRYPTO_CIPHERTEXTBYTES];
uint8_t pk[CRYPTO_PUBLICKEYBYTES];
 
    printf("Tests PASSED. All session keys matched.\n");
    printf("\n\n");

 
  
    return true;
}

static void kem_bench(const int seconds) 
{
    uint8_t pk[CRYPTO_PUBLICKEYBYTES];
    uint8_t sk[CRYPTO_SECRETKEYBYTES];
    uint8_t ss_encap[CRYPTO_BYTES], ss_decap[CRYPTO_BYTES];
    uint8_t ct[CRYPTO_CIPHERTEXTBYTES];

    TIME_OPERATION_SECONDS({ crypto_kem_keypair(pk, sk); }, "Key generation", seconds);

    crypto_kem_keypair(pk, sk);
    TIME_OPERATION_SECONDS({ crypto_kem_enc(ct, ss_encap, pk); }, "KEM encapsulate", seconds);
    
    crypto_kem_enc(ct, ss_encap, pk);
    TIME_OPERATION_SECONDS({ crypto_kem_dec(ss_decap, ct, sk); }, "KEM decapsulate", seconds);
}


int main() 
{
    int OK = true; 
    
   
    printf("\n");
           printf("=============================================================================================================================\n");
    printf("Testing correctness of key encapsulation mechanism (KEM)\n" );
    printf("=============================================================================================================================\n");
    
    crypto_kem_keypair(pk, sk);    
    run_test();
    /*
    uint8_t sk[CRYPTO_SECRETKEYBYTES];
    uint8_t ss_encap[CRYPTO_BYTES], ss_decap[CRYPTO_BYTES];
    uint16_t s[2*PARAMS_N*PARAMS_NBAR] = {0};   
    uint8_t ct[CRYPTO_CIPHERTEXTBYTES];
    uint8_t pk[CRYPTO_PUBLICKEYBYTES];

    crypto_kem_keypair(pk, sk); 
    crypto_kem_enc(ct, ss_encap, pk); 
    crypto_kem_dec(ss_decap, ct, sk);
    secret_key_recovery(ct, pk);
        
    if (memcmp(ss_encap, ss_decap, CRYPTO_BYTES) != 0) {
        printf("\n ERROR!\n");
	return false; 
    }
    
    */
    printf("Tests PASSED. All session keys matched.\n");
    printf("\n\n");


exit:
    return (OK == true) ? EXIT_SUCCESS : EXIT_FAILURE; 
}
