//
// Created by HaveYouTall
//

#include "montgomery.h"
// ===============================
// =      Gadget for montg       =
// ===============================

// uint64_t [12] + uint64_t [12].
// result = a + b
/// @return carry
uint8_t add_12_12(const uint64_t a[12], const uint64_t b[12], uint64_t result[12])
{
	u1 carry = 0;
	carry = _addcarryx_u64(carry, a[0], b[0], result + 0);
	carry = _addcarryx_u64(carry, a[1], b[1], result + 1);
	carry = _addcarryx_u64(carry, a[2], b[2], result + 2);
	carry = _addcarryx_u64(carry, a[3], b[3], result + 3);
	carry = _addcarryx_u64(carry, a[4], b[4], result + 4);
	carry = _addcarryx_u64(carry, a[5], b[5], result + 5);
    carry = _addcarryx_u64(carry, a[6], b[6], result + 6);
	carry = _addcarryx_u64(carry, a[7], b[7], result + 7);
	carry = _addcarryx_u64(carry, a[8], b[8], result + 8);
	carry = _addcarryx_u64(carry, a[9], b[9], result + 9);
	carry = _addcarryx_u64(carry, a[10], b[10], result + 10);
	carry = _addcarryx_u64(carry, a[11], b[11], result + 11);
	return carry;
}

// res = a * R (mod modulus)
void to_mont_slow(const uint64_t a[6], uint64_t res[6]){
    //res = a*R (mod q)
    uint64_t tmp[12];
    memcpy(tmp, a, sizeof(uint64_t) * 6);

    // res = ( ((tmp * 2^376) mod modulus) * 2^8 ) mod modulus = tmp * 2^384 mod modulus.
    lshift(tmp, 12, 376);
    barrett_384(tmp, res); // Input data should be less than 377+377 bits.
    
    memcpy(tmp, res, sizeof(uint64_t) * 6);
    memset(tmp+6, 0, sizeof(uint64_t) * 6);
    lshift(tmp, 12, 8); 

    barrett_384(tmp, res);
}

void to_mont_fast(const uint64_t a[6], uint64_t res[6]) {
#ifdef MONTG_ASM
	mont_mul_reduce_as(a, R2, res);
#else
	mont_mul_reduce(a, R2, res);
#endif
}

// res = a * R^-1 (mod modulus)
void mont_inv_slow(const uint64_t a[6], uint64_t res[6]){
    uint64_t tmp[12];
    raw_mul(a,R_1,tmp);
    barrett_384(tmp,res);
}

// R * R^-1 = 1 + p * p', where p_1 is p' here.
void mont_mul_reduce(const uint64_t a[6], const uint64_t b[6], uint64_t res[6]){
    uint64_t T[12];
    uint64_t tmp[12];
    //x = a * b       a and b are both mont form
    raw_mul(a, b, T);

    //1. res = (T(mod R))* p_1 (mod R)
    //  1) tmp = T mod R 
    #pragma unroll 6
    for(int i=0;i<6;i++){
        tmp[i+6] = 0;
        tmp[i] = T[i];
    }
    // 2) res = tmp * p_1 mod R 
    raw_mul_reduce_R(tmp, p_1, res);


    //2. tmp = T + res * q
    //  1) tmp = res * q
    raw_mul(res, MODULUS, tmp);
    add_12_12(T, tmp, tmp);

    //3. tmp = tmp/R
    rshift(tmp, 12, 384);

    if(u384_ge(tmp,MODULUS)==1) {
        u384_sub(tmp,MODULUS,res);
    }
    else {
        #pragma unroll 6
        for(size_t i = 0; i< 6; i++) {
            res[i] = tmp[i];
        }
    }
}
