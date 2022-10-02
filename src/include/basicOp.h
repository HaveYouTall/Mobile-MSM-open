//
// Created by HaveYouTall
//
#ifndef HEADERS_BASIC_OPERATION_H
#define HEADERS_BASIC_OPERATION_H

#include "dataType.h"

////////////////////////////////
// 64-bit add and mul gadget. //
////////////////////////////////
extern "C" { uint8_t _addcarryx_u64(uint8_t carry, uint64_t a, uint64_t b, uint64_t* res); }
extern "C" {uint64_t _mulx_u64(uint64_t a, uint64_t b, uint64_t* hi); }


////////////////////////////////
//      Basic Operation.      //
////////////////////////////////

// ========== add ============ //

// add for u48, i.e., uint64_t [6]
// result = a + b
/// @return carry
uint8_t add(const uint64_t a[6], const uint64_t b[6], uint64_t result[6]);

// inc for u48, i.e., uint64_t [6]
// result = a + b, where b is at most 64-bit data. 
/// @return carry
uint8_t inc(const uint64_t a[6], const uint64_t b, uint64_t result[6]);

// ========== sub ============ //

// sub for u48, i.e., uint64_t [6]
// result = a - b
void sub(const u48* a, const u48* b, u48* result);
// CUDA_CALLABLE void uint64_sub(const uint64_t a[6], const uint64_t b[6], uint64_t result[6]);
// res = a - b
// return carry, if carry == 1, it means a >= b, otherwise b > a.
uint8_t u384_sub(const uint64_t a[6], const uint64_t b[6], uint64_t res[6]);

// neg for u48, i.e., uint64_t [6]
// input = -input
void neg(u48* input);

// ========== shift ============ //

// left shift for u48, i.e., uint64_t [6]
// input = input << 1
/// @return carry
u1 shl(u48* input);

// right shift for u48, i.e., uint64_t [6]
// input = input >> 1
/// @return carry
void shr(u48* input);

// ========== judge ============ //

// judge for u48, i.e., uint64_t [6]
/// @return true if a is greater than b or equal to b.
bool ge(const u48* a, const u48* b);
// CUDA_CALLABLE bool uint64_ge(const uint64_t a[6], const uint64_t b[6]);
bool u384_ge(const uint64_t a[6], const uint64_t b[6]);

// judge for u48, i.e., uint64_t [6]
/// @return true if a is greater than b. 
bool gr(const u48* a, const u48* b);

// judge for u48, i.e., uint64_t [6]
/// @return true if a is equal to b.
bool eq(const u48* a, const u48* b);

// judge for u48, i.e., uint64_t [6]
/// @return true if a is equal to zero. 
bool eq_zero(const u48* a);

// judge for u48, i.e., uint64_t [6]
/// @return true if a is equal to 1. 
bool u48_eq_one(const u48* a);
bool eq_one(const uint64_t a[6]);

// ========== mul ============ //

// Gadget for montg_mul_reduce
/// @return result = x * y mod R, where R here is 2^384.
extern "C" { void raw_mul_reduce_R(const uint64_t x[6], const uint64_t y[6], uint64_t result[6]);}

// 384-bit mul 384-bit, i.e., uint64_t [6] mul uint64_t [6].
/// @param result = x * y.
// CUDA_CALLABLE void raw_mul(const uint64_t x[6], const uint64_t y[6], uint64_t result[12]);
extern "C" { void raw_mul(const uint64_t x[6], const uint64_t y[6], uint64_t result[12]); }
// 384-bit power, i.e., uint64_t [6].
/// @param result = x * x.
// CUDA_CALLABLE void raw_pow(const uint64_t x[6], uint64_t result[12]);
extern "C" { void raw_pow(const uint64_t x[6], uint64_t result[12]); }

#endif