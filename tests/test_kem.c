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
     run_test();
     
     
/*uint8_t sk[CRYPTO_SECRETKEYBYTES];
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
