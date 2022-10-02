#ifndef _MONTGOMERY_H_
#define _MONTGOMERY_H_
#include "dataType.h"
#include "barrett.h"
#include "basicOp.h"
#include "const.h"


#include "utils.h"
// ===============================
// =      Gadget for montg       =
// ===============================

// uint64_t [12] + uint64_t [12].
// result = a + b
/// @return carry
uint8_t add_12_12(const uint64_t a[12], const uint64_t b[12], uint64_t result[12]);

// res = a * R (mod modulus)
void to_mont_slow(const uint64_t a[6], uint64_t res[6]);
void to_mont_fast(const uint64_t a[6], uint64_t res[6]);

// Use fast mont transform algorithm
#define to_mont to_mont_fast

// res = a * R^-1 (mod modulus)
void mont_inv_slow(const uint64_t a[6], uint64_t res[6]);

// Use fast mont transform algorithm
// mont_inv_as test ERROR, so we still use mont_inv_slow
#define mont_inv mont_inv_slow

void mont_mul_reduce(const uint64_t a[6], const uint64_t b[6], uint64_t res[6]);

extern "C" {void mont_mul_reduce_as(const uint64_t a[6], const uint64_t b[6], uint64_t result[6]);}
extern "C" {void mont_sqr_reduce_as(const uint64_t a[6], uint64_t result[6]);}

#endif