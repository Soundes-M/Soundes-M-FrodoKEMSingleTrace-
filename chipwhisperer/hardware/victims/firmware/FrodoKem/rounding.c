#include <time.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <math.h> 

#include "rounding.h"

  
void arith_to_bool(int32_t *arr, int32_t *rearr) {
    int32_t rarr[NUM_SHARES];
    bool_mask(arr[0],rarr);	
    int32_t x[NUM_SHARES];
    for (int i = 1; i < NUM_SHARES; ++i) {
        bool_mask(arr[i],x);
        bool_add(rarr,x,rarr);
    }

	int32_t corr_arr[NUM_SHARES];
	bool_mask(-POLYRING_N,corr_arr);
	bool_add(rarr,corr_arr,rarr);

	for(int i=0; i < NUM_SHARES; i++){
		rearr[i] = rarr[i];
    	}

}

void bool_add(int32_t *arr1, int32_t *arr2, int32_t *rearr) {
	int32_t rarr[NUM_SHARES];
    int32_t p[NUM_SHARES];
    int32_t g[NUM_SHARES];
    bool_xor(arr1,arr2,p);
    bool_and(arr1,arr2,g);


    int32_t aux[NUM_SHARES];
    int32_t aux0[NUM_SHARES];
    int32_t pow;
    for (int i = 1; i < 5; ++i) {
        pow = 1 << (i-1);
        bool_lshift(pow,g,aux);
        bool_and(p,aux,aux);
        bool_xor(g,aux,g);
        bool_lshift(pow,p,aux0);
        bool_refresh(aux0);
        bool_and(p,aux0,p);

    }
    bool_lshift((1<<4),g,aux);
    bool_and(aux,p,aux);
    bool_xor(g,aux,g);
    bool_lshift(1,g,aux);
    bool_xor(arr1,arr2,rarr);
    bool_xor(rarr,aux,rarr);

	for(int i=0; i < NUM_SHARES; i++){
		rearr[i] = rarr[i];
    	}
}

void bool_and(int32_t *arr1, int32_t *arr2, int32_t *rearr) {

    int32_t rarr[NUM_SHARES];
    for (int i = 0; i < NUM_SHARES; ++i) {
        rarr[i] = arr1[i] & arr2[i];
    }
    
    int32_t z_ij;
    int32_t z_ji;
    int32_t x;
    for (int i = 0; i < NUM_SHARES; i++) {
        for (int j = i+1; j < NUM_SHARES; j++) {
            z_ij = rand() % 2147483648;
            z_ji = arr1[i] & arr2[j];
            z_ji = z_ij ^ z_ji;
            x = arr1[j] & arr2[i];
            z_ji = x ^ z_ji;
            rarr[i] = rarr[i] ^ z_ij;
            rarr[j] = rarr[j] ^ z_ji;
        }
    }

    for(int i=0; i < NUM_SHARES; i++){
	rearr[i] = rarr[i];
    }
}

void bool_mask(int32_t a, int32_t *arr){
	
   
    for (int i = 0; i < NUM_SHARES-1; ++i) {
        arr[i] = rand() % 2147483648;
        a = a ^ arr[i];
    }
    arr[NUM_SHARES-1] = a;
}

void bool_rshift(int32_t a, int32_t *arr, int32_t *rarr){
    for (int i = 0; i < NUM_SHARES; ++i) {
        rarr[i] = arr[i] >> a;
    }
}

void bool_lshift(int32_t a, int32_t *arr, int32_t *rarr){
    for (int i = 0; i < NUM_SHARES; ++i) {
        rarr[i] = arr[i] << a;
    }
}

void bool_neg(int32_t *arr, int32_t *rarr){
    for (int i = 0; i < NUM_SHARES; ++i) {
	if(!(arr[i] == 1)){
		rarr[i] = INT_MAX; 
	}
	else{
		rarr[i] = 0;
	}
	
        
    }
}

void bool_not(int32_t *arr, int32_t *rarr){
    int32_t c=INT_MAX;
print_shares(arr);
    for (int i = 0; i < NUM_SHARES; ++i) {
        rarr[i] = arr[i] ^ c;
    }
print_shares(rarr);
}

void bool_xor(int32_t *arr1, int32_t *arr2, int32_t *rarr){
    for (int i = 0; i < NUM_SHARES; ++i) {
        rarr[i] = arr1[i] ^ arr2[i];
    }
}

void bool_refresh(int32_t *arr){
	
    int32_t tmp;
    for(int i=1; i<NUM_SHARES; i++){
	tmp = rand() % 2147483648;
	arr[0] = arr[0] ^ tmp;
	arr[i] = arr[i] ^ tmp;
    }
}

void bool_mask_from_sign(int32_t *arr, int32_t *rarr){
	bool_rshift(31,arr,rarr);
	bool_neg(rarr,rarr);
}

void arith_to_bool_lowbits(int32_t *arr,int32_t beta, int32_t *rarr){
	arith_to_bool(arr,rarr);
	int32_t var_help = log(beta)/log(2);
	bool_lshift(32-var_help,rarr,rarr);
	int32_t  b[NUM_SHARES];
	bool_mask_from_sign(rarr,b);
	bool_lshift(var_help,b,b);
	bool_rshift(32-var_help,rarr,rarr);
	bool_xor(rarr,b,rarr);
}

void arith_to_bool_highbits(int32_t *arr,int32_t beta, int32_t *rarr){
	
	
	for(int i=0; i<NUM_SHARES;i++){
		arr[i] = arr[i] - (POLYRING_N/NUM_SHARES);
	}
	int32_t x[NUM_SHARES];
	bool_mask(arr[0],x);
	int32_t y[NUM_SHARES];
	
	for(int i=1; i<NUM_SHARES; i++){
		bool_mask(arr[i],y);
		bool_add(x,y,x);
	}
	
	int32_t var_help = log(beta)/log(2);
	bool_rshift(var_help,x,rarr);	
	
	
}



void arith_mask(int32_t a, int32_t *arr){
	int32_t sum = 0;
	
   	for (int i = 0; i < NUM_SHARES-1; ++i) {
        	arr[i] = rand() % (POLYRING_N/ NUM_SHARES);
		sum += (arr[i] % POLYRING_N);
        	
    	}
	sum = sum % POLYRING_N;
	int32_t r;
	if(a - sum >= 0){
		r= a-sum + POLYRING_N;
	}
	else{
		r= POLYRING_N-sum+a;
	}
    	arr[NUM_SHARES-1] = r;
}

void arith_add(int32_t *arr1, int32_t *arr2, int32_t *rarr) {
	for (int i = 0; i < NUM_SHARES; ++i) {
        	rarr[i] = arr1[i] + arr2[i];
		//rarr[i] = rarr[i] % POLYRING_N;
    	}
}

void arith_rshift(int32_t a, int32_t *arr, int32_t *rarr) {
	for (int i = 0; i < NUM_SHARES; ++i) {
        	rarr[i] = arr[i] >> a;
    	}
}

int32_t arith_unmask(int32_t *arr){
	int32_t a = 0;
	for(int i=0; i<NUM_SHARES; i++){
		a = a + arr[i] % POLYRING_N;

	}
	a = a % POLYRING_N;
	return a;
}

int32_t bool_unmask(int32_t *arr){
    int32_t a = arr[0];
    for (int i = 1; i < NUM_SHARES; ++i) {
        a = a ^ arr[i];
    }
    return a;
 
} 
