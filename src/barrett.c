//
// Created by HaveYouTall
//
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "barrett.h"
#include "basicOp.h"
#include "const.h"
#include "utils.h"


void rshift(uint64_t *r, size_t nlimbs ,size_t n){ 
    size_t remove_limbs = n / DIGIT_BITS;
    n = n & DIGIT_MASK;
    nlimbs -= remove_limbs;

    memcpy(r, r + remove_limbs, sizeof(uint64_t)*nlimbs);
    memset(r + nlimbs, 0, sizeof(uint64_t)*remove_limbs);
    if(n == 0) { 
        return;
    }
#ifdef DEBUG
    printf("rshift res\n");
    for(int i =0; i< 17; i++){
        printf("%llx\n", r[i]);
    }
#endif 

    size_t i = 0;
    while(i + 1 < nlimbs){
        r[i] = (r[i] >> n) | (r[i + 1] << (64-n));
        i++;        
    }
    r[i] >>= n;
}

void lshift(uint64_t *r, size_t nlimbs ,size_t n){ 
    size_t remove_limbs = n / DIGIT_BITS;
    n = n & DIGIT_MASK;
    nlimbs -= remove_limbs;

    memcpy(r+remove_limbs, r, sizeof(uint64_t)*nlimbs);
    memset(r, 0, sizeof(uint64_t)*remove_limbs);
    if(n == 0) { 
        return;
    }
    size_t i = nlimbs + remove_limbs -1;
    while(i > remove_limbs){
        r[i] = (r[i] << n) | (r[i - 1] >> (64-n));
        i--;
    }
    r[i] <<= n;
}


// r = r mod (2^n)
void mod_2n(uint64_t *r, size_t nlimbs, size_t n){
    
    while(nlimbs >= 1 && r[nlimbs-1]==0){
        nlimbs--;
    }

    size_t reserve_limbs = n / DIGIT_BITS;
    n = n & DIGIT_MASK;
    
    if(nlimbs<=reserve_limbs){
        return;
    }

    uint64_t mask = ((uint64_t)1<<n) - 1;


    r[reserve_limbs] &= mask;
    memset(r + reserve_limbs + 1, 0, sizeof(uint64_t)*(nlimbs-reserve_limbs-1));

}



// Implementation for barrett_384 
// base b = 8
// k = 126
// input x should be 12 * 64 bit(at most 754 bit for bls12_377) 
// Specifically input x is at most 2*k - 1 () digits when base is b = 8 according to barrett algorithm.
inline void singleBarrett(const uint64_t* x, const uint64_t MODULUS[6], const uint64_t mu[6], int nlimbs, uint64_t *res)  //res = x mod MODULUS
{  
    if(nlimbs==0){
        memset(res, 0, sizeof(uint64_t) * 6);
        return;
    }

    uint64_t r1[12]; 
    uint64_t r2[12];
    uint64_t q2[12];
    
    /* 1. q1 = floor( x / b^(k-1) )
	 *    q2 = q1 * mu
	 *    q3 = floor( q2 / b^(k+1) )
	 * Actually, we don't need q1, q3, we can work direct on r1, q2
	 */

    // r1 = floor( x / b^(k-1) ), b^(k-1) = 8 ^ (k-1) = 2 ^ (3*(k-1)). 
    // r1 is at most 379 bit, i.e., 6 * 64 bit
    memcpy(r1, x, sizeof(uint64_t)*nlimbs);
    rshift(r1, nlimbs, 3 * (BARRETT_K - 1) ); 

    // q2 = r1 * mu, where r1 is 6 * 64 bit, mu is 6 * 64 bit. 
    // Specifically, r1 is 379 bit, mu is 380 bit.
    // So q2 is at most 12 * 64 bit (759 bit, 379 + 380 bit)   
    raw_mul(r1, mu, q2); 

    // q2 = q2 / b^(k+1)
    // q2 at most 6 * 64 bit 
    rshift(q2, 12, 3 * (BARRETT_K + 1)); 


    /* 
     * 2. r1 = x mod b^(k+1)
	 *	  r2 = q3 * m mod b^(k+1)
	 *	  r  = r1 - r2 
	 */
    memcpy(r1, x, sizeof(uint64_t)*nlimbs);
    mod_2n(r1, nlimbs, 3 * (BARRETT_K+1)); // now r1 is at most 6 * 64 bit
    raw_mul(q2, MODULUS, r2); // r2 = q2 * MODULUS
    mod_2n(r2, 12, 3 * (BARRETT_K+1)); // now r2 is 6 * 64 bit
    uint8_t c = u384_sub(r1, r2, res); // res = r1 - r2

    /*
     * 3. if r < 0 then  r = r + b^(k+1)
     */
    if(c==0)
        mod_2n(res, 6, 3 * (BARRETT_K+1));

    /* 4. while r >= m do r = r - m */
    while(u384_ge(res,MODULUS)==1){
        u384_sub(res, MODULUS, res);
    }
}


// This version has constant nlimbs 12 
/// @param x: is at most 12 * uint64_t 
/// @return res: it stores the final result, which is at most 6 * uint64_t
/// res = x mod MODULUS
void barrett_384(const uint64_t x[12], uint64_t res[6]) {
    singleBarrett(x, MODULUS, mu, 12, res);
}

