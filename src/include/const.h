//
// Created by HaveYouTall
// 
#ifndef _CONST_H_
#define _CONST_H_

#include "dataType.h"

// Modulus for bls12-77 G1
extern const uint64_t MODULUS[6];
extern const u48 _MODULUS_;


// ============================= base now is choosen as 8 ==========================
// mu = floor ( b^2k/modulus ) 
// k = number of octonary limbs, which is 127 here
// b is base, which is 8 here.
// 0x2760d74bcf32791738a0406c331e9ae8a46e09d07fda82a52f7d1dc780a19de74e65c59e8163c701ec4f881fd59646e8
extern const uint64_t mu[6];

// R_1 represents R^-1, where R * R^-1 = 1 + p * p'
extern const uint64_t R_1[6]; 

    
// p_1 represents p', where R * R^-1 = 1 + p * p'
extern const uint64_t p_1[6];

// R2 represents R^2
extern const uint64_t R2[6];

// Inverse element for 2 in this field.
// i.e., 2 * inv_for_2 = 1 mod MODULUS.
extern const uint64_t inv_for_2[6];

extern const uint64_t montg_one[6];
extern const u48 montg_one_u48;


// ============================== BLS12-377 EdwardsParameters ============================

extern const uint64_t COEFF_D; // Should be in BLS12-377 BaseField (MODULUS field).
extern const uint64_t COEFF_A[6];
extern const uint64_t montg_COEFF_A[6];
extern const uint64_t montg_COEFF_D[6];

// ========================================================================================


#endif