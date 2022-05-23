#ifndef ROUNDING_H
#define ROUNDING_H

#include <stdint.h>
 
#define NUM_SHARES 10
#define POLYRING_N  256   

void arith_to_bool(int32_t *arr, int32_t *rearr);
void bool_add(int32_t *arr1, int32_t *arr2, int32_t *rearr);
void bool_and(int32_t *arr1, int32_t *arr2, int32_t *rearr);
void bool_mask(int32_t a, int32_t *arr);
void bool_rshift(int32_t a, int32_t *arr, int32_t *rarr);
void bool_lshift(int32_t a, int32_t *arr, int32_t *rarr);
void bool_neg(int32_t *arr, int32_t *rarr);
void bool_not(int32_t *arr, int32_t *rarr);
void bool_xor(int32_t *arr1, int32_t *arr2, int32_t *rarr);
void bool_refresh(int32_t *arr);
void bool_mask_from_sign(int32_t *arr, int32_t *rarr);
void arith_to_bool_lowbits(int32_t *arr,int32_t beta, int32_t *rarr);
void arith_to_bool_highbits(int32_t *arr,int32_t beta, int32_t *rarr);
void arith_mask(int32_t a, int32_t *arr);
void arith_add(int32_t *arr1, int32_t *arr2, int32_t *rarr);
void arith_rshift(int32_t a, int32_t *arr, int32_t *rarr);
int32_t arith_unmask(int32_t *arr);


int32_t bool_unmask(int32_t *arr);
void print_shares(int32_t *arr);

#endif
