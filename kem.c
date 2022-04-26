/********************************************************************************************
* FrodoKEM: Learning with Errors Key Encapsulation
*
* Abstract: Key Encapsulation Mechanism (KEM) based on Frodo
*********************************************************************************************/

#include <string.h>
#include "sha3/fips202.h"

 

uint16_t Sp_side_channel[(2*PARAMS_N+PARAMS_NBAR)*PARAMS_NBAR] = {0};  // contains secret data
uint16_t Epp_side_channel[2*PARAMS_N*PARAMS_NBAR] = {0};

//uint16_t *Epp_side_channel = (uint16_t *)&Sp_side_channel[2*PARAMS_N*PARAMS_NBAR];  // contains secret data


// This is added by Soundes
int gaussian_sampler_test()
{ 
    uint16_t S[2*PARAMS_N*PARAMS_NBAR] = {0};               // contains secret data
    
    uint8_t randomness[2*CRYPTO_BYTES + BYTES_SEED_A];      // contains secret data via randomness_s and randomness_seedSE
               
    uint8_t *randomness_seedSE = &randomness[CRYPTO_BYTES]; // contains secret data
    
    uint8_t shake_input_seedSE[1 + CRYPTO_BYTES];           // contains secret data

      
     
    shake_input_seedSE[0] = 0x5F;
    memcpy(&shake_input_seedSE[1], randomness_seedSE, CRYPTO_BYTES);
    shake((uint8_t*)S, 2*PARAMS_N*PARAMS_NBAR*sizeof(uint16_t), shake_input_seedSE, 1 + CRYPTO_BYTES);
    frodo_sample_n(S, PARAMS_N*PARAMS_NBAR);
    

    return 1;

}


int crypto_kem_keypair(unsigned char* pk, unsigned char* sk)
{ // FrodoKEM's key generation
  // Outputs: public key pk (               BYTES_SEED_A + (PARAMS_LOGQ*PARAMS_N*PARAMS_NBAR)/8 bytes)
  //          secret key sk (CRYPTO_BYTES + BYTES_SEED_A + (PARAMS_LOGQ*PARAMS_N*PARAMS_NBAR)/8 + 2*PARAMS_N*PARAMS_NBAR + BYTES_PKHASH bytes)
    uint8_t *pk_seedA = &pk[0];
    uint8_t *pk_b = &pk[BYTES_SEED_A];
    uint8_t *sk_s = &sk[0];
    uint8_t *sk_pk = &sk[CRYPTO_BYTES];
    uint8_t *sk_S = &sk[CRYPTO_BYTES + CRYPTO_PUBLICKEYBYTES];
    uint8_t *sk_pkh = &sk[CRYPTO_BYTES + CRYPTO_PUBLICKEYBYTES + 2*PARAMS_N*PARAMS_NBAR];
    uint16_t B[PARAMS_N*PARAMS_NBAR] = {0};
    uint16_t S[2*PARAMS_N*PARAMS_NBAR] = {0};               // contains secret data
    uint16_t *E = (uint16_t *)&S[PARAMS_N*PARAMS_NBAR];     // contains secret data
    uint8_t randomness[2*CRYPTO_BYTES + BYTES_SEED_A];      // contains secret data via randomness_s and randomness_seedSE
    uint8_t *randomness_s = &randomness[0];                 // contains secret data
    uint8_t *randomness_seedSE = &randomness[CRYPTO_BYTES]; // contains secret data
    uint8_t *randomness_z = &randomness[2*CRYPTO_BYTES];
    uint8_t shake_input_seedSE[1 + CRYPTO_BYTES];           // contains secret data

    // Generate the secret value s, the seed for S and E, and the seed for the seed for A. Add seed_A to the public key
    randombytes(randomness, CRYPTO_BYTES + CRYPTO_BYTES + BYTES_SEED_A);
    shake(pk_seedA, BYTES_SEED_A, randomness_z, BYTES_SEED_A);

    // Generate S and E, and compute B = A*S + E. Generate A on-the-fly
    shake_input_seedSE[0] = 0x5F;
    memcpy(&shake_input_seedSE[1], randomness_seedSE, CRYPTO_BYTES);
    shake((uint8_t*)S, 2*PARAMS_N*PARAMS_NBAR*sizeof(uint16_t), shake_input_seedSE, 1 + CRYPTO_BYTES);
    frodo_sample_n(S, PARAMS_N*PARAMS_NBAR);
    frodo_sample_n(E, PARAMS_N*PARAMS_NBAR);
    frodo_mul_add_as_plus_e(B, S, E, pk);

    // Encode the second part of the public key
    frodo_pack(pk_b, CRYPTO_PUBLICKEYBYTES - BYTES_SEED_A, B, PARAMS_N*PARAMS_NBAR, PARAMS_LOGQ);

    // Add s, pk and S to the secret key
    memcpy(sk_s, randomness_s, CRYPTO_BYTES);
    memcpy(sk_pk, pk, CRYPTO_PUBLICKEYBYTES);
    memcpy(sk_S, S, 2*PARAMS_N*PARAMS_NBAR);

    // Add H(pk) to the secret key
    shake(sk_pkh, BYTES_PKHASH, pk, CRYPTO_PUBLICKEYBYTES);

    // Cleanup:
    clear_bytes((uint8_t *)S, PARAMS_N*PARAMS_NBAR*sizeof(uint16_t));
    clear_bytes((uint8_t *)E, PARAMS_N*PARAMS_NBAR*sizeof(uint16_t));
    clear_bytes(randomness, 2*CRYPTO_BYTES);
    clear_bytes(shake_input_seedSE, 1 + CRYPTO_BYTES);
    return 0;
}


int crypto_kem_enc(unsigned char *ct, unsigned char *ss, const unsigned char *pk)
{ // FrodoKEM's key encapsulation
    const uint8_t *pk_seedA = &pk[0];
    const uint8_t *pk_b = &pk[BYTES_SEED_A];
    uint8_t *ct_c1 = &ct[0];
    uint8_t *ct_c2 = &ct[(PARAMS_LOGQ*PARAMS_N*PARAMS_NBAR)/8];
    uint16_t B[PARAMS_N*PARAMS_NBAR] = {0};
    uint16_t V[PARAMS_NBAR*PARAMS_NBAR]= {0};                 // contains secret data
    uint16_t C[PARAMS_NBAR*PARAMS_NBAR] = {0};
    uint16_t Bp[PARAMS_N*PARAMS_NBAR] = {0};
    uint16_t Sp[(2*PARAMS_N+PARAMS_NBAR)*PARAMS_NBAR] = {0};  // contains secret data
    uint16_t *Ep = (uint16_t *)&Sp[PARAMS_N*PARAMS_NBAR];     // contains secret data
    uint16_t *Epp = (uint16_t *)&Sp[2*PARAMS_N*PARAMS_NBAR];  // contains secret data
    uint8_t G2in[BYTES_PKHASH + BYTES_MU];                    // contains secret data via mu
    uint8_t *pkh = &G2in[0];
    uint8_t *mu = &G2in[BYTES_PKHASH];                        // contains secret data
    uint8_t G2out[2*CRYPTO_BYTES];                            // contains secret data
    uint8_t *seedSE = &G2out[0];                              // contains secret data
    uint8_t *k = &G2out[CRYPTO_BYTES];                        // contains secret data
    uint8_t Fin[CRYPTO_CIPHERTEXTBYTES + CRYPTO_BYTES];       // contains secret data via Fin_k
    uint8_t *Fin_ct = &Fin[0];
    uint8_t *Fin_k = &Fin[CRYPTO_CIPHERTEXTBYTES];            // contains secret data
    uint8_t shake_input_seedSE[1 + CRYPTO_BYTES];             // contains secret data 
    // pkh <- G_1(pk), generate random mu, compute (seedSE || k) = G_2(pkh || mu)
    shake(pkh, BYTES_PKHASH, pk, CRYPTO_PUBLICKEYBYTES);
    randombytes(mu, BYTES_MU);
    shake(G2out, CRYPTO_BYTES + CRYPTO_BYTES, G2in, BYTES_PKHASH + BYTES_MU);

    // Generate Sp and Ep, and compute Bp = Sp*A + Ep. Generate A on-the-fly
    shake_input_seedSE[0] = 0x96;
    memcpy(&shake_input_seedSE[1], seedSE, CRYPTO_BYTES);
    shake((uint8_t*)Sp, (2*PARAMS_N+PARAMS_NBAR)*PARAMS_NBAR*sizeof(uint16_t), shake_input_seedSE, 1 + CRYPTO_BYTES);
    frodo_sample_n(Sp, PARAMS_N*PARAMS_NBAR);
    frodo_sample_n(Ep, PARAMS_N*PARAMS_NBAR);
    frodo_mul_add_sa_plus_e(Bp, Sp, Ep, pk_seedA);
    frodo_pack(ct_c1, (PARAMS_LOGQ*PARAMS_N*PARAMS_NBAR)/8, Bp, PARAMS_N*PARAMS_NBAR, PARAMS_LOGQ);

    // Generate Epp, and compute V = Sp*B + Epp
    frodo_sample_n(Epp, PARAMS_NBAR*PARAMS_NBAR);
    frodo_unpack(B, PARAMS_N*PARAMS_NBAR, pk_b, CRYPTO_PUBLICKEYBYTES - BYTES_SEED_A, PARAMS_LOGQ);
    frodo_mul_add_sb_plus_e(V, B, Sp, Epp);

    // Encode mu, and compute C = V + enc(mu) (mod q)
    frodo_key_encode(C, (uint16_t*)mu);
    frodo_add(C, V, C);
    frodo_pack(ct_c2, (PARAMS_LOGQ*PARAMS_NBAR*PARAMS_NBAR)/8, C, PARAMS_NBAR*PARAMS_NBAR, PARAMS_LOGQ);

    // Compute ss = F(ct||KK)
    memcpy(Fin_ct, ct, CRYPTO_CIPHERTEXTBYTES);
    memcpy(Fin_k, k, CRYPTO_BYTES);
    shake(ss, CRYPTO_BYTES, Fin, CRYPTO_CIPHERTEXTBYTES + CRYPTO_BYTES);

    // Cleanup:
    clear_bytes((uint8_t *)V, PARAMS_NBAR*PARAMS_NBAR*sizeof(uint16_t));
    clear_bytes((uint8_t *)Sp, PARAMS_N*PARAMS_NBAR*sizeof(uint16_t));
    clear_bytes((uint8_t *)Ep, PARAMS_N*PARAMS_NBAR*sizeof(uint16_t));
    clear_bytes((uint8_t *)Epp, PARAMS_NBAR*PARAMS_NBAR*sizeof(uint16_t));
    clear_bytes(mu, BYTES_MU);
    clear_bytes(G2out, 2*CRYPTO_BYTES);
    clear_bytes(Fin_k, CRYPTO_BYTES);
    clear_bytes(shake_input_seedSE, 1 + CRYPTO_BYTES);
    return 0;
}


int crypto_kem_dec(unsigned char *ss, const unsigned char *ct, const unsigned char *sk)
{ // FrodoKEM's key decapsulation
    uint16_t B[PARAMS_N*PARAMS_NBAR] = {0};
    uint16_t Bp[PARAMS_N*PARAMS_NBAR] = {0};
    uint16_t W[PARAMS_NBAR*PARAMS_NBAR] = {0};                // contains secret data
    uint16_t C[PARAMS_NBAR*PARAMS_NBAR] = {0};
    uint16_t CC[PARAMS_NBAR*PARAMS_NBAR] = {0};
    uint16_t BBp[PARAMS_N*PARAMS_NBAR] = {0};
    uint16_t Sp[(2*PARAMS_N+PARAMS_NBAR)*PARAMS_NBAR] = {0};  // contains secret data
    uint16_t *Ep = (uint16_t *)&Sp[PARAMS_N*PARAMS_NBAR];     // contains secret data
    uint16_t *Epp = (uint16_t *)&Sp[2*PARAMS_N*PARAMS_NBAR];  // contains secret data
    const uint8_t *ct_c1 = &ct[0];
    const uint8_t *ct_c2 = &ct[(PARAMS_LOGQ*PARAMS_N*PARAMS_NBAR)/8];
    const uint8_t *sk_s = &sk[0];
    const uint8_t *sk_pk = &sk[CRYPTO_BYTES];
    const uint16_t *sk_S = (uint16_t *) &sk[CRYPTO_BYTES + CRYPTO_PUBLICKEYBYTES];
    const uint8_t *sk_pkh = &sk[CRYPTO_BYTES + CRYPTO_PUBLICKEYBYTES + 2*PARAMS_N*PARAMS_NBAR];
    const uint8_t *pk_seedA = &sk_pk[0];
    const uint8_t *pk_b = &sk_pk[BYTES_SEED_A];
    uint8_t G2in[BYTES_PKHASH + BYTES_MU];                   // contains secret data via muprime
    uint8_t *pkh = &G2in[0];
    uint8_t *muprime = &G2in[BYTES_PKHASH];                  // contains secret data
    uint8_t G2out[2*CRYPTO_BYTES];                           // contains secret data
    uint8_t *seedSEprime = &G2out[0];                        // contains secret data
    uint8_t *kprime = &G2out[CRYPTO_BYTES];                  // contains secret data
    uint8_t Fin[CRYPTO_CIPHERTEXTBYTES + CRYPTO_BYTES];      // contains secret data via Fin_k
    uint8_t *Fin_ct = &Fin[0];
    uint8_t *Fin_k = &Fin[CRYPTO_CIPHERTEXTBYTES];           // contains secret data
    uint8_t shake_input_seedSEprime[1 + CRYPTO_BYTES];       // contains secret data

    // Compute W = C - Bp*S (mod q), and decode the randomness mu
    frodo_unpack(Bp, PARAMS_N*PARAMS_NBAR, ct_c1, (PARAMS_LOGQ*PARAMS_N*PARAMS_NBAR)/8, PARAMS_LOGQ);
 
    frodo_unpack(C, PARAMS_NBAR*PARAMS_NBAR, ct_c2, (PARAMS_LOGQ*PARAMS_NBAR*PARAMS_NBAR)/8, PARAMS_LOGQ);
     
    frodo_mul_bs(W, Bp, sk_S);
    frodo_sub(W, C, W);
    frodo_key_decode((uint16_t*)muprime, W);
    //for(int i=0;i<BYTES_PKHASH;i++)  printf("%ld :", muprime[i]);
    
    // Generate (seedSE' || k') = G_2(pkh || mu')
    memcpy(pkh, sk_pkh, BYTES_PKHASH);
    shake(G2out, CRYPTO_BYTES + CRYPTO_BYTES, G2in, BYTES_PKHASH + BYTES_MU);

    // Generate Sp and Ep, and compute BBp = Sp*A + Ep. Generate A on-the-fly
    shake_input_seedSEprime[0] = 0x96;
    memcpy(&shake_input_seedSEprime[1], seedSEprime, CRYPTO_BYTES);
    shake((uint8_t*)Sp, (2*PARAMS_N+PARAMS_NBAR)*PARAMS_NBAR*sizeof(uint16_t), shake_input_seedSEprime, 1 + CRYPTO_BYTES);
    frodo_sample_n(Sp, PARAMS_N*PARAMS_NBAR);
    
    //Analyse the power consumption and get the S' values
    get_side_channel_Sp(Sp); 
    
    frodo_sample_n(Ep, PARAMS_N*PARAMS_NBAR);
    
    
    frodo_mul_add_sa_plus_e(BBp, Sp, Ep, pk_seedA);

    // Generate Epp, and compute W = Sp*B + Epp
    frodo_sample_n(Epp, PARAMS_NBAR*PARAMS_NBAR);   
    
    //Analyse the power consumption and get the E'' values
    get_side_channel_Epp(Epp);  
    
    frodo_unpack(B, PARAMS_N*PARAMS_NBAR, pk_b, CRYPTO_PUBLICKEYBYTES - BYTES_SEED_A, PARAMS_LOGQ);
    frodo_mul_add_sb_plus_e(W, B, Sp, Epp);

    // Encode mu, and compute CC = W + enc(mu') (mod q)
    frodo_key_encode(CC, (uint16_t*)muprime);  
    
    frodo_add(CC, W, CC);

    // Prepare input to F
    memcpy(Fin_ct, ct, CRYPTO_CIPHERTEXTBYTES);

    // Reducing BBp modulo q
    for (int i = 0; i < PARAMS_N*PARAMS_NBAR; i++) BBp[i] = BBp[i] & ((1 << PARAMS_LOGQ)-1);
    
    /*************constitue the two equations for the attack**********************/
     
 
    
    
 
    // If (Bp == BBp & C == CC) then ss = F(ct || k'), else ss = F(ct || s)
    // Needs to avoid branching on secret data as per:
    //     Qian Guo, Thomas Johansson, Alexander Nilsson. A key-recovery timing attack on post-quantum 
    //     primitives using the Fujisaki-Okamoto transformation and its application on FrodoKEM. In CRYPTO 2020.
    int8_t selector = ct_verify(Bp, BBp, PARAMS_N*PARAMS_NBAR) | ct_verify(C, CC, PARAMS_NBAR*PARAMS_NBAR);
    // If (selector == 0) then load k' to do ss = F(ct || k'), else if (selector == -1) load s to do ss = F(ct || s)
    ct_select((uint8_t*)Fin_k, (uint8_t*)kprime, (uint8_t*)sk_s, CRYPTO_BYTES, selector);
    shake(ss, CRYPTO_BYTES, Fin, CRYPTO_CIPHERTEXTBYTES + CRYPTO_BYTES);

/*
    printf("\n");
    printf("Key printed during the decryption:\n");
    for (int i=0;i<CRYPTO_BYTES; i++) printf("%d ", ss[i]);
  */  
    // Cleanup:
    clear_bytes((uint8_t *)W, PARAMS_NBAR*PARAMS_NBAR*sizeof(uint16_t));
    clear_bytes((uint8_t *)Sp, PARAMS_N*PARAMS_NBAR*sizeof(uint16_t));
    clear_bytes((uint8_t *)Ep, PARAMS_N*PARAMS_NBAR*sizeof(uint16_t));
    clear_bytes((uint8_t *)Epp, PARAMS_NBAR*PARAMS_NBAR*sizeof(uint16_t));
    clear_bytes(muprime, BYTES_MU);
    clear_bytes(G2out, 2*CRYPTO_BYTES);
    clear_bytes(Fin_k, CRYPTO_BYTES);
    clear_bytes(shake_input_seedSEprime, 1 + CRYPTO_BYTES);
    return 0;
}

void get_side_channel_Sp(uint16_t* Sp){

 for(int i=0;i<PARAMS_N*PARAMS_NBAR;i++) Sp_side_channel[i] = Sp[i];
 
}


void get_side_channel_Epp(uint16_t* Epp){

 for(int i=0;i<PARAMS_N*PARAMS_NBAR;i++) Epp_side_channel[i] = Epp[i];

}

unsigned char * secret_key_recovery(const unsigned char *ct, const unsigned char *pk)
{

    uint16_t C[PARAMS_NBAR*PARAMS_NBAR] = {0};
    const uint8_t *ct_c2 = &ct[(PARAMS_LOGQ*PARAMS_N*PARAMS_NBAR)/8];
    const uint8_t *pk_b = &pk[BYTES_SEED_A];
    uint16_t B[PARAMS_N*PARAMS_NBAR] = {0}; 
    uint16_t Bp[PARAMS_N*PARAMS_NBAR] = {0}; 
    uint16_t V[PARAMS_NBAR*PARAMS_NBAR] = {0};        
    uint16_t Wp[PARAMS_NBAR*PARAMS_NBAR] = {0};

    
  
    unsigned char ss[CRYPTO_BYTES];

    uint8_t G2in[BYTES_PKHASH + BYTES_MU];                   // contains secret data via muprime
    uint8_t *pkh = &G2in[0];
    uint8_t *muprime = &G2in[BYTES_PKHASH];                  // contains secret data
    uint8_t G2out[2*CRYPTO_BYTES];                           // contains secret data 
    uint8_t *seedSEprime = &G2out[0];                        // contains secret data
    uint8_t *kprime = &G2out[CRYPTO_BYTES];                  // contains secret data
    
    
    const uint8_t *ct_c1 = &ct[0];                    // contains secret data via mu
 
    uint8_t Fin[CRYPTO_CIPHERTEXTBYTES + CRYPTO_BYTES];      // contains secret data via Fin_k
    uint8_t *Fin_ct = &Fin[0];
    uint8_t *Fin_k = &Fin[CRYPTO_CIPHERTEXTBYTES];           // contains secret data
    
    
    frodo_unpack(C, PARAMS_NBAR*PARAMS_NBAR, ct_c2, (PARAMS_LOGQ*PARAMS_NBAR*PARAMS_NBAR)/8, PARAMS_LOGQ);
    frodo_unpack(B, PARAMS_N*PARAMS_NBAR, pk_b, CRYPTO_PUBLICKEYBYTES - BYTES_SEED_A, PARAMS_LOGQ);
   //We know that C=C'
   //unpack(c2)=Sp*B + Epp+Encode(mu')
   //unpack(c2)-Sp*B-Epp = Encode(mu')
   // wp= Encode(mu')
    frodo_mul_add_sb_plus_e(V, B, Sp_side_channel, Epp_side_channel);
    frodo_sub(Wp, C, V);  
    frodo_key_decode((uint16_t*)muprime, Wp);
    frodo_unpack(Bp, PARAMS_N*PARAMS_NBAR, ct_c1, (PARAMS_LOGQ*PARAMS_N*PARAMS_NBAR)/8, PARAMS_LOGQ);
  
    // Calculate -(Wp-c) 
    //frodo_sub(V, C, Wp);
    //for (int i = 0; i < PARAMS_N*PARAMS_NBAR; i++) V[i] = V[i] & ((1 << PARAMS_LOGQ)-1);
   
    shake(pkh, BYTES_PKHASH, pk, CRYPTO_PUBLICKEYBYTES);
    shake(G2out, CRYPTO_BYTES + CRYPTO_BYTES, G2in, BYTES_PKHASH + BYTES_MU);

    //prepare ct||Kprime
    memcpy(Fin_ct, ct, CRYPTO_CIPHERTEXTBYTES);
    for(int i=0;i<CRYPTO_BYTES;i++) Fin_k[i]=kprime[i];
    // Compute the hash  ss = F(ct||KK) 
    shake(ss, CRYPTO_BYTES, Fin, CRYPTO_CIPHERTEXTBYTES + CRYPTO_BYTES);
    /*
    printf("\n");
    printf("Key recovery:\n");
    for (int i=0;i<CRYPTO_BYTES; i++) printf("%d ", ss[i]);
    printf("\n");
     */
       
    
    // Cleanup:
   clear_bytes((uint8_t *)V, PARAMS_NBAR*PARAMS_NBAR*sizeof(uint16_t));
   clear_bytes((uint8_t *)Wp, PARAMS_NBAR*PARAMS_NBAR*sizeof(uint16_t));
   clear_bytes((uint8_t *)Sp_side_channel, PARAMS_N*PARAMS_NBAR*sizeof(uint16_t)); 
   clear_bytes((uint8_t *)Epp_side_channel, PARAMS_NBAR*PARAMS_NBAR*sizeof(uint16_t));
   
  
   return ss;
   
}


